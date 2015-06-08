/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include "threaded.hh"
#include "bit_graph.hh"
#include "template_voodoo.hh"
#include "degree_sort.hh"

#include <algorithm>
#include <limits>
#include <random>
#include <future>
#include <map>
#include <cassert>

using std::chrono::milliseconds;
using std::chrono::duration_cast;
using std::chrono::steady_clock;

const constexpr int split_levels = 5;

auto atomic_min(std::atomic<int> & a, int v) -> void
{
    while (true) {
        int cur_v = a.load(std::memory_order_seq_cst);
        if (v < cur_v) {
            if (a.compare_exchange_strong(cur_v, v, std::memory_order_seq_cst))
                return;
        }
        else
            return;
    }
}

struct Tasks
{
    std::vector<std::thread> threads;
    std::atomic<bool> finish;
    std::atomic<int> busy;
    std::list<std::function<void ()> > pending;
    std::mutex mutex;
    std::condition_variable cv;

    std::list<milliseconds> times;

    Tasks(unsigned n_threads)
    {
        finish = false;
        busy = 0;

        for (unsigned t = 0 ; t < n_threads ; ++t)
            threads.emplace_back([this] {
                    milliseconds total_work_time = milliseconds::zero();

                    while (true) {
                        std::unique_lock<std::mutex> guard(mutex);
                        if (finish.load())
                            break;

                        if (! pending.empty()) {
                            auto f = std::move(*pending.begin());
                            pending.pop_front();
                            ++busy;
                            guard.unlock();

                            auto start_work_time = steady_clock::now(); // local start time

                            f();

                            auto work_time = duration_cast<milliseconds>(steady_clock::now() - start_work_time);
                            total_work_time += work_time;

                            guard.lock();
                            --busy;
                            cv.notify_all();
                        }
                        else
                            cv.wait(guard);
                    }

                    std::unique_lock<std::mutex> guard(mutex);
                    times.push_back(total_work_time);
                });
    }

    auto kill_workers() -> void
    {
        {
            std::unique_lock<std::mutex> guard(mutex);
            finish = true;
            cv.notify_all();
        }

        for (auto & t : threads)
            t.join();

        threads.clear();
    }

    ~Tasks()
    {
        kill_workers();
    }

    Tasks(const Tasks &) = delete;

    auto add(std::function<void ()> && f) -> void
    {
        std::unique_lock<std::mutex> guard(mutex);
        pending.push_back(std::move(f));
        cv.notify_all();
    }

    auto complete() -> void
    {
        std::unique_lock<std::mutex> guard(mutex);
        while ((! pending.empty()) || (busy > 0))
            cv.wait(guard);
    }
};

struct Position
{
    std::array<unsigned, split_levels + 1> values;

    Position()
    {
        std::fill(values.begin(), values.end(), 0);
    }

    bool operator< (const Position & other) const
    {
        for (unsigned p = 0 ; p < split_levels + 1 ; ++p)
            if (values.at(p) < other.values.at(p))
                return true;
            else if (values.at(p) > other.values.at(p))
                return false;

        return false;
    }
};

struct HelpPoints
{
    struct Task
    {
        const std::function<void ()> * func;
        int pending;
    };

    std::mutex mutex;
    std::condition_variable cv;
    std::map<Position, Task> tasks;
    std::atomic<bool> finish;

    std::vector<std::thread> threads;

    std::list<milliseconds> times;

    HelpPoints(int n_threads) :
        finish(false)
    {
        for (int t = 0 ; t < n_threads ; ++t)
            threads.emplace_back([this, n_threads, t] {
                    milliseconds total_work_time = milliseconds::zero();
                    while (! finish.load()) {
                        std::unique_lock<std::mutex> guard(mutex);
                        bool did_something = false;
                        for (auto task = tasks.begin() ; task != tasks.end() ; ++task) {
                            if (task->second.func) {
                                auto f = task->second.func;
                                ++task->second.pending;
                                guard.unlock();

                                auto start_work_time = steady_clock::now(); // local start time

                                (*f)();

                                auto work_time = duration_cast<milliseconds>(steady_clock::now() - start_work_time);
                                total_work_time += work_time;

                                guard.lock();
                                task->second.func = nullptr;
                                if (0 == --task->second.pending)
                                    cv.notify_all();

                                did_something = true;
                                break;
                            }
                        }

                        if (! did_something)
                            cv.wait(guard);
                    }

                    std::unique_lock<std::mutex> guard(mutex);
                    times.push_back(total_work_time);
                    });
    }

    auto kill_workers() -> void
    {
        {
            std::unique_lock<std::mutex> guard(mutex);
            finish.store(true);
            cv.notify_all();
        }

        for (auto & t : threads)
            t.join();

        threads.clear();
    }

    ~HelpPoints()
    {
        kill_workers();
    }

    HelpPoints(const HelpPoints &) = delete;
};

enum class Search
{
    Aborted,
    Unsatisfiable,
    Satisfiable
};

enum class AssignAndSearch
{
    Aborted,
    Satisfiable,
    AssignFailed,
    Backjump,
    TryNext
};

template <unsigned n_words_, bool backjump_, int k_, int l_>
struct ThreadedSubgraphIsomorphism
{
    struct Domain
    {
        unsigned v;
        unsigned popcount;
        FixedBitSet<n_words_> values;
    };

    using Domains = std::vector<Domain>;
    using Assignments = std::array<unsigned, n_words_ * bits_per_word>;

    struct RealFailedVariables
    {
        FixedBitSet<n_words_> variables;

        auto independent_of(const Domains & old_domains, const Domains & new_domains) -> bool
        {
            auto vc = variables;
            for (int v = vc.first_set_bit() ; v != -1 ; v = vc.first_set_bit()) {
                vc.unset(v);

                auto o = std::find_if(old_domains.begin(), old_domains.end(), [v] (const Domain & d) { return d.v == unsigned(v); });
                auto n = std::find_if(new_domains.begin(), new_domains.end(), [v] (const Domain & d) { return d.v == unsigned(v); });

                unsigned opc = (o == old_domains.end() ? 1 : o->popcount);
                unsigned npc = (n == new_domains.end() ? 1 : n->popcount);
                if (opc != npc)
                    return false;
            }

            return true;
        }

        auto add(unsigned dv) -> void
        {
            variables.set(dv);
        }

        auto add(const RealFailedVariables & d) -> void
        {
            variables.union_with(d.variables);
        }
    };

    using FailedVariables = RealFailedVariables;
    using SubproblemResult = std::tuple<AssignAndSearch, FailedVariables, Assignments>;

    struct Subproblem
    {
        unsigned branch_v;
        int f_v;
    };

    const Params & params;

    static constexpr int max_graphs = 1 + (l_ - 1) * k_;
    std::array<FixedBitGraph<n_words_>, max_graphs> target_graphs;
    std::array<FixedBitGraph<n_words_>, max_graphs> pattern_graphs;

    std::vector<int> pattern_order, target_order, isolated_vertices;
    std::array<int, n_words_ * bits_per_word> pattern_degree_tiebreak;

    unsigned pattern_size, full_pattern_size, target_size;

    Result result;

    std::atomic<bool> someone_found_a_solution{ false };

    Tasks tasks;
    HelpPoints help_points;

    ThreadedSubgraphIsomorphism(const Graph & target, const Graph & pattern, const Params & a) :
        params(a),
        target_order(target.size()),
        pattern_size(pattern.size()),
        full_pattern_size(pattern.size()),
        target_size(target.size()),
        tasks(params.n_threads),
        help_points(params.n_threads - 1)
    {
        // strip out isolated vertices in the pattern
        for (unsigned v = 0 ; v < full_pattern_size ; ++v)
            if (0 == pattern.degree(v)) {
                isolated_vertices.push_back(v);
                --pattern_size;
            }
            else
                pattern_order.push_back(v);

        // recode pattern to a bit graph
        pattern_graphs.at(0).resize(pattern_size);
        for (unsigned i = 0 ; i < pattern_size ; ++i)
            for (unsigned j = 0 ; j < pattern_size ; ++j)
                if (pattern.adjacent(pattern_order.at(i), pattern_order.at(j)))
                    pattern_graphs.at(0).add_edge(i, j);

        // determine ordering for target graph vertices
        std::iota(target_order.begin(), target_order.end(), 0);

        std::mt19937 g;
        std::shuffle(target_order.begin(), target_order.end(), g);
        degree_sort(target, target_order, false);

        // recode target to a bit graph
        target_graphs.at(0).resize(target_size);
        for (unsigned i = 0 ; i < target_size ; ++i)
            for (unsigned j = 0 ; j < target_size ; ++j)
                if (target.adjacent(target_order.at(i), target_order.at(j)))
                    target_graphs.at(0).add_edge(i, j);

        for (unsigned j = 0 ; j < pattern_size ; ++j)
            pattern_degree_tiebreak.at(j) = pattern_graphs.at(0).degree(j);
    }

    auto parallel_build_supplemental_graphs(int n_threads) -> void
    {
        for (int g = 1 ; g < max_graphs ; ++g)
            pattern_graphs.at(g).resize(pattern_size);

        for (int g = 1 ; g < max_graphs ; ++g)
            target_graphs.at(g).resize(target_size);

        std::atomic<unsigned> pos2p, pos3p, pos2t, pos3t;
        pos2p = 0; pos3p = 0; pos2t = 0; pos3t = 0;

        for (int t = 0 ; t < n_threads ; ++t) {
            tasks.add([&] {
                if (l_ >= 2) {
                    for (unsigned v ; ((v = pos2p++)) < pattern_size ; ) {
                        auto nv = pattern_graphs.at(0).neighbourhood(v);
                        for (int c = nv.first_set_bit() ; c != -1 ; c = nv.first_set_bit()) {
                            nv.unset(c);
                            auto nc = pattern_graphs.at(0).neighbourhood(c);
                            for (int w = nc.first_set_bit() ; w != -1 && unsigned(w) <= v ; w = nc.first_set_bit()) {
                                nc.unset(w);
                                if (k_ >= 3 && pattern_graphs.at(2).adjacent(v, w))
                                    pattern_graphs.at(3).add_edge_atomic(v, w);
                                else if (k_ >= 2 && pattern_graphs.at(1).adjacent(v, w))
                                    pattern_graphs.at(2).add_edge_atomic(v, w);
                                else if (k_ >= 1)
                                    pattern_graphs.at(1).add_edge_atomic(v, w);
                            }
                        }
                    }
                }

                if (l_ >= 3) {
                    for (unsigned v ; ((v = pos3p++)) < pattern_size ; ) {
                        auto nv = pattern_graphs.at(0).neighbourhood(v);
                        for (int c = nv.first_set_bit() ; c != -1 ; c = nv.first_set_bit()) {
                            nv.unset(c);
                            auto nc = pattern_graphs.at(0).neighbourhood(c);
                            for (int d = nc.first_set_bit() ; d != -1 ; d = nc.first_set_bit()) {
                                nc.unset(d);
                                if (unsigned(d) == v)
                                    continue;

                                auto nd = pattern_graphs.at(0).neighbourhood(d);
                                for (int w = nd.first_set_bit() ; w != -1 && unsigned(w) <= v ; w = nd.first_set_bit()) {
                                    nd.unset(w);
                                    if (w == c)
                                        continue;

                                    if (k_ >= 3 && pattern_graphs.at(k_ + 2).adjacent(v, w))
                                        pattern_graphs.at(k_ + 3).add_edge_atomic(v, w);
                                    else if (k_ >= 2 && pattern_graphs.at(k_ + 1).adjacent(v, w))
                                        pattern_graphs.at(k_ + 2).add_edge_atomic(v, w);
                                    else if (k_ >= 1)
                                        pattern_graphs.at(k_ + 1).add_edge_atomic(v, w);
                                }
                            }
                        }
                    }
                }

                if (l_ >= 2) {
                    for (unsigned v ; ((v = pos2t++)) < target_size ; ) {
                        auto nv = target_graphs.at(0).neighbourhood(v);
                        for (int c = nv.first_set_bit() ; c != -1 ; c = nv.first_set_bit()) {
                            nv.unset(c);
                            auto nc = target_graphs.at(0).neighbourhood(c);
                            for (int w = nc.first_set_bit() ; w != -1 && unsigned(w) <= v ; w = nc.first_set_bit()) {
                                nc.unset(w);
                                if (k_ >= 3 && target_graphs.at(2).adjacent(v, w))
                                    target_graphs.at(3).add_edge_atomic(v, w);
                                else if (k_ >= 2 && target_graphs.at(1).adjacent(v, w))
                                    target_graphs.at(2).add_edge_atomic(v, w);
                                else if (k_ >= 1)
                                    target_graphs.at(1).add_edge_atomic(v, w);
                            }
                        }
                    }
                }

                if (l_ >= 3) {
                    for (unsigned v ; ((v = pos3t++)) < target_size ; ) {
                        auto nv = target_graphs.at(0).neighbourhood(v);
                        for (int c = nv.first_set_bit() ; c != -1 ; c = nv.first_set_bit()) {
                            nv.unset(c);
                            auto nc = target_graphs.at(0).neighbourhood(c);
                            for (int d = nc.first_set_bit() ; d != -1 ; d = nc.first_set_bit()) {
                                nc.unset(d);
                                if (unsigned(d) == v)
                                    continue;

                                auto nd = target_graphs.at(0).neighbourhood(d);
                                for (int w = nd.first_set_bit() ; w != -1 && unsigned(w) <= v ; w = nd.first_set_bit()) {
                                    nd.unset(w);
                                    if (w == c)
                                        continue;

                                    if (k_ >= 3 && target_graphs.at(k_ + 2).adjacent(v, w))
                                        target_graphs.at(k_ + 3).add_edge_atomic(v, w);
                                    else if (k_ >= 2 && target_graphs.at(k_ + 1).adjacent(v, w))
                                        target_graphs.at(k_ + 2).add_edge_atomic(v, w);
                                    else if (k_ >= 1)
                                        target_graphs.at(k_ + 1).add_edge_atomic(v, w);
                                }
                            }
                        }
                    }
                }
            });
        }

        tasks.complete();
    }

    auto assign(Domains & new_domains, unsigned branch_v, unsigned f_v, int g_end, FailedVariables & failed_variables) -> bool
    {
        // for each remaining domain...
        for (auto & d : new_domains) {
            // all different
            d.values.unset(f_v);

            // for each graph pair...
            for (int g = 0 ; g < g_end ; ++g) {
                // if we're adjacent...
                if (pattern_graphs.at(g).adjacent(branch_v, d.v)) {
                    // ...then we can only be mapped to adjacent vertices
                    target_graphs.at(g).intersect_with_row(f_v, d.values);
                }
            }

            // we might have removed values
            d.popcount = d.values.popcount();
            if (0 == d.popcount) {
                failed_variables.add(d.v);
                return false;
            }
        }

        FailedVariables all_different_failed_variables;
        if (! cheap_all_different(new_domains, all_different_failed_variables)) {
            failed_variables.add(all_different_failed_variables);
            return false;
        }

        return true;
    }

    auto get_help_with(const Position & position, const std::function<void ()> & func) -> void
    {
        std::map<Position, HelpPoints::Task>::iterator task;
        {
            std::unique_lock<std::mutex> guard(help_points.mutex);
            auto r = help_points.tasks.emplace(position, HelpPoints::Task{ &func, 0 });
            assert(r.second);
            task = r.first;
            help_points.cv.notify_all();
        }

        func();

        {
            std::unique_lock<std::mutex> guard(help_points.mutex);
            while (0 != task->second.pending)
                help_points.cv.wait(guard);
            help_points.tasks.erase(task);
        }
    }

    auto search(
            Assignments & assignments,
            Domains & domains,
            std::atomic<unsigned long long> & nodes,
            const int g_end,
            unsigned effective_depth,
            const Position & position,
            const std::function<bool()> & kill_function
            ) -> std::pair<Search, FailedVariables>
    {
        if (params.abort->load() || someone_found_a_solution.load() || kill_function())
            return std::make_pair(Search::Aborted, FailedVariables());

        ++nodes;

        Domain * branch_domain = nullptr;

        for (auto & d : domains)
            if ((! branch_domain) ||
                    d.popcount < branch_domain->popcount ||
                    (d.popcount == branch_domain->popcount && pattern_degree_tiebreak.at(d.v) > pattern_degree_tiebreak.at(branch_domain->v)))
                branch_domain = &d;

        if (! branch_domain) {
            someone_found_a_solution.store(true);
            return std::make_pair(Search::Satisfiable, FailedVariables());
        }

        auto remaining = branch_domain->values;
        auto branch_v = branch_domain->v;

        FailedVariables shared_failed_variables;
        shared_failed_variables.add(branch_domain->v);

        std::array<int, n_words_ * bits_per_word> branch;
        int branch_end = 0;
        for (int f_v = remaining.first_set_bit() ; f_v != -1 ; f_v = remaining.first_set_bit()) {
            remaining.unset(f_v);
            branch[branch_end++] = f_v;
        }

        std::atomic<int> shared_b{ 0 }, kill_after{ branch_end + 1 };

        using ThisThreadData = std::tuple<
            std::pair<Search, FailedVariables>,
            bool,
            FailedVariables,
            Assignments>;

        std::array<ThisThreadData, n_words_ * bits_per_word> all_threads_data;

        auto this_thread_function = [&] () {
            for (int b = shared_b++ ; b < branch_end ; b = shared_b++) {
                std::pair<Search, FailedVariables> & this_thread_result = std::get<0>(all_threads_data.at(b));
                bool & this_thread_keep_going = std::get<1>(all_threads_data.at(b));
                FailedVariables & this_thread_failed_variables = std::get<2>(all_threads_data.at(b));
                Assignments & this_thread_assignments = std::get<3>(all_threads_data.at(b));

                this_thread_failed_variables = FailedVariables();
                this_thread_assignments = assignments;
                this_thread_keep_going = true;

                int f_v = branch[b];

                /* try assigning f_v to v */
                this_thread_assignments.at(branch_v) = f_v;

                /* set up new domains */
                Domains new_domains;
                new_domains.reserve(domains.size() - 1);
                for (auto & d : domains)
                    if (d.v != branch_v)
                        new_domains.push_back(d);

                /* assign and propagate */
                if (! assign(new_domains, branch_v, f_v, g_end, this_thread_failed_variables))
                    continue;

                unsigned new_effective_depth = (0 == branch_end ? effective_depth : effective_depth + 1);
                Position child_position = position;
                child_position.values.at(effective_depth) = (b + 1);
                auto search_result = (new_effective_depth) >= split_levels ?
                    search_nopar(this_thread_assignments, new_domains, nodes, g_end, [&] () -> bool {
                            return (kill_function() || b >= kill_after.load());
                            }) :
                    search(this_thread_assignments, new_domains, nodes, g_end, new_effective_depth, child_position, [&] () -> bool {
                            return (kill_function() || b >= kill_after.load());
                            });

                switch (search_result.first) {
                    case Search::Satisfiable:
                        this_thread_result.first = Search::Satisfiable;
                        this_thread_keep_going = false;
                        break;

                    case Search::Aborted:
                        this_thread_result.first = Search::Aborted;
                        this_thread_keep_going = false;
                        break;

                    case Search::Unsatisfiable:
                        // can we backjump?
                        if (search_result.second.independent_of(domains, new_domains)) {
                            this_thread_result = std::move(search_result);
                            this_thread_keep_going = false;
                        }
                        else
                            this_thread_failed_variables.add(search_result.second);
                        break;
                }

                if (! this_thread_keep_going) {
                    // all jobs to our left have already been started.
                    // don't start any jobs to our right, since we won't
                    // use the result.
                    shared_b.store(branch_end + 1);

                    // try to kill any threads to our right
                    atomic_min(kill_after, b);

                    break;
                }
            }
        };

        if (0 == branch_end)
            this_thread_function();
        else
            get_help_with(position, this_thread_function);

        if (someone_found_a_solution.load()) {
            // one of our children might have succeeded, cancelling stuff
            // to its left. fish around.
            for (int b = 0 ; b < branch_end ; ++b)
                if (Search::Satisfiable == std::get<0>(all_threads_data.at(b)).first) {
                    assignments = std::get<3>(all_threads_data.at(b));
                    return std::get<0>(all_threads_data.at(b));
                }
        }

        for (int b = 0 ; b < branch_end ; ++b) {
            std::pair<Search, FailedVariables> & this_thread_result = std::get<0>(all_threads_data.at(b));
            bool & this_thread_keep_going = std::get<1>(all_threads_data.at(b));
            FailedVariables & this_thread_failed_variables = std::get<2>(all_threads_data.at(b));
            Assignments & this_thread_assignments = std::get<3>(all_threads_data.at(b));

            shared_failed_variables.add(this_thread_failed_variables);

            if (! this_thread_keep_going) {
                assignments = this_thread_assignments;
                return this_thread_result;
            }
        }

        return std::make_pair(Search::Unsatisfiable, shared_failed_variables);
    }

    auto search_nopar(
            Assignments & assignments,
            Domains & domains,
            std::atomic<unsigned long long> & nodes,
            const int g_end,
            const std::function<bool()> & kill_function
            ) -> std::pair<Search, FailedVariables>
    {
        if (params.abort->load() || someone_found_a_solution.load() || kill_function())
            return std::make_pair(Search::Aborted, FailedVariables());

        ++nodes;

        Domain * branch_domain = nullptr;

        for (auto & d : domains)
            if ((! branch_domain) ||
                    d.popcount < branch_domain->popcount ||
                    (d.popcount == branch_domain->popcount && pattern_degree_tiebreak.at(d.v) > pattern_degree_tiebreak.at(branch_domain->v)))
                branch_domain = &d;

        if (! branch_domain) {
            someone_found_a_solution.store(true);
            return std::make_pair(Search::Satisfiable, FailedVariables());
        }

        auto remaining = branch_domain->values;
        auto branch_v = branch_domain->v;

        FailedVariables shared_failed_variables;
        shared_failed_variables.add(branch_domain->v);

        std::array<int, n_words_ * bits_per_word> branch;
        int branch_end = 0;
        for (int f_v = remaining.first_set_bit() ; f_v != -1 ; f_v = remaining.first_set_bit()) {
            remaining.unset(f_v);
            branch[branch_end++] = f_v;
        }

        std::pair<Search, FailedVariables> this_thread_result;
        bool this_thread_keep_going = true;
        FailedVariables this_thread_failed_variables;
        Assignments this_thread_assignments = assignments;

        for (int b = 0 ; b < branch_end ; ++b) {
            int f_v = branch[b];

            /* try assigning f_v to v */
            this_thread_assignments.at(branch_v) = f_v;

            /* set up new domains */
            Domains new_domains;
            new_domains.reserve(domains.size() - 1);
            for (auto & d : domains)
                if (d.v != branch_v)
                    new_domains.push_back(d);

            /* assign and propagate */
            if (! assign(new_domains, branch_v, f_v, g_end, shared_failed_variables))
                continue;

            auto search_result = search_nopar(this_thread_assignments, new_domains, nodes, g_end, kill_function);

            switch (search_result.first) {
                case Search::Satisfiable:
                    this_thread_result = std::make_pair(Search::Satisfiable, FailedVariables());
                    this_thread_keep_going = false;
                    break;

                case Search::Aborted:
                    this_thread_result = std::make_pair(Search::Aborted, FailedVariables());
                    this_thread_keep_going = false;
                    break;

                case Search::Unsatisfiable:
                    if (search_result.second.independent_of(domains, new_domains)) {
                        this_thread_result = search_result;
                        this_thread_keep_going = false;
                    }
                    else
                        this_thread_failed_variables.add(search_result.second);
                    break;
            }

            if (! this_thread_keep_going)
                break;
        }

        shared_failed_variables.add(this_thread_failed_variables);
        assignments = this_thread_assignments;

        if (this_thread_keep_going)
            return std::make_pair(Search::Unsatisfiable, shared_failed_variables);
        else
            return this_thread_result;
    }

    auto initialise_domains(Domains & domains, int g_end) -> bool
    {
        unsigned remaining_target_vertices = target_size;
        FixedBitSet<n_words_> allowed_target_vertices;
        allowed_target_vertices.set_up_to(target_size);

        while (true) {
            std::array<std::vector<int>, max_graphs> patterns_degrees;
            std::array<std::vector<int>, max_graphs> targets_degrees;

            for (int g = 0 ; g < g_end ; ++g) {
                patterns_degrees.at(g).resize(pattern_size);
                targets_degrees.at(g).resize(target_size);
            }

            /* pattern and target degree sequences */
            for (int g = 0 ; g < g_end ; ++g) {
                for (unsigned i = 0 ; i < pattern_size ; ++i)
                    patterns_degrees.at(g).at(i) = pattern_graphs.at(g).degree(i);

                for (unsigned i = 0 ; i < target_size ; ++i) {
                    FixedBitSet<n_words_> remaining = allowed_target_vertices;
                    target_graphs.at(g).intersect_with_row(i, remaining);
                    targets_degrees.at(g).at(i) = remaining.popcount();
                }
            }

            /* pattern and target neighbourhood degree sequences */
            std::array<std::vector<std::vector<int> >, max_graphs> patterns_ndss;
            std::array<std::vector<std::vector<int> >, max_graphs> targets_ndss;

            for (int g = 0 ; g < g_end ; ++g) {
                patterns_ndss.at(g).resize(pattern_size);
                targets_ndss.at(g).resize(target_size);
            }

            std::atomic<unsigned> posi1, posi2;
            posi1 = 0, posi2 = 0;

            for (unsigned t = 0 ; t < params.n_threads ; ++t) {
                tasks.add([&] {
                    for (unsigned i ; ((i = posi1++)) < pattern_size ; ) {
                        for (int g = 0 ; g < g_end ; ++g) {
                            for (unsigned j = 0 ; j < pattern_size ; ++j) {
                                if (pattern_graphs.at(g).adjacent(i, j))
                                    patterns_ndss.at(g).at(i).push_back(patterns_degrees.at(g).at(j));
                            }
                            std::sort(patterns_ndss.at(g).at(i).begin(), patterns_ndss.at(g).at(i).end(), std::greater<int>());
                        }
                    }

                    for (unsigned i ; ((i = posi2++)) < target_size ; ) {
                        for (int g = 0 ; g < g_end ; ++g) {
                            for (unsigned j = 0 ; j < target_size ; ++j) {
                                if (target_graphs.at(g).adjacent(i, j))
                                    targets_ndss.at(g).at(i).push_back(targets_degrees.at(g).at(j));
                            }
                            std::sort(targets_ndss.at(g).at(i).begin(), targets_ndss.at(g).at(i).end(), std::greater<int>());
                        }
                    }
                });
            }

            tasks.complete();

            std::atomic<unsigned> posi;
            posi = 0;

            for (unsigned t = 0 ; t < params.n_threads ; ++t) {
                tasks.add([&] {
                    for (unsigned i ; ((i = posi++)) < pattern_size ; ) {
                        domains.at(i).v = i;
                        domains.at(i).values.unset_all();

                        for (unsigned j = 0 ; j < target_size ; ++j) {
                            bool ok = true;

                            for (int g = 0 ; g < g_end ; ++g) {
                                if (! allowed_target_vertices.test(j)) {
                                    ok = false;
                                }
                                else if (pattern_graphs.at(g).adjacent(i, i) && ! target_graphs.at(g).adjacent(j, j)) {
                                    ok = false;
                                }
                                else if (targets_ndss.at(g).at(j).size() < patterns_ndss.at(g).at(i).size()) {
                                    ok = false;
                                }
                                else {
                                    for (unsigned x = 0 ; ok && x < patterns_ndss.at(g).at(i).size() ; ++x) {
                                        if (targets_ndss.at(g).at(j).at(x) < patterns_ndss.at(g).at(i).at(x))
                                            ok = false;
                                    }
                                }

                                if (! ok)
                                    break;
                            }

                            if (ok)
                                domains.at(i).values.set(j);
                        }

                        domains.at(i).popcount = domains.at(i).values.popcount();
                    }
                });
            }

            tasks.complete();

            FixedBitSet<n_words_> domains_union;
            for (auto & d : domains)
                domains_union.union_with(d.values);

            unsigned domains_union_popcount = domains_union.popcount();
            if (domains_union_popcount < unsigned(pattern_size)) {
                return false;
            }
            else if (domains_union_popcount == remaining_target_vertices)
                break;

            allowed_target_vertices.intersect_with(domains_union);
            remaining_target_vertices = allowed_target_vertices.popcount();
        }

        return true;
    }

    auto cheap_all_different(Domains & domains, FailedVariables & failed_variables) -> bool
    {
        // pick domains smallest first, with tiebreaking
        std::array<int, n_words_ * bits_per_word> domains_order;
        std::iota(domains_order.begin(), domains_order.begin() + domains.size(), 0);

        std::sort(domains_order.begin(), domains_order.begin() + domains.size(),
                [&] (int a, int b) {
                return (domains.at(a).popcount < domains.at(b).popcount) ||
                (domains.at(a).popcount == domains.at(b).popcount && pattern_degree_tiebreak.at(domains.at(a).v) > pattern_degree_tiebreak.at(domains.at(b).v));
                });

        // counting all-different
        FixedBitSet<n_words_> domains_so_far, hall;
        unsigned neighbours_so_far = 0;

        for (int i = 0, i_end = domains.size() ; i != i_end ; ++i) {
            auto & d = domains.at(domains_order.at(i));

            failed_variables.add(d.v);

            d.values.intersect_with_complement(hall);
            d.popcount = d.values.popcount();

            if (0 == d.popcount)
                return false;

            domains_so_far.union_with(d.values);
            ++neighbours_so_far;

            unsigned domains_so_far_popcount = domains_so_far.popcount();
            if (domains_so_far_popcount < neighbours_so_far) {
                return false;
            }
            else if (domains_so_far_popcount == neighbours_so_far) {
                neighbours_so_far = 0;
                hall.union_with(domains_so_far);
                domains_so_far.unset_all();
            }
        }

        return true;
    }

    auto prepare_for_search(Domains & domains) -> void
    {
        for (auto & d : domains)
            d.popcount = d.values.popcount();
    }

    auto save_result(Assignments & assignments, Result & result) -> void
    {
        for (unsigned v = 0 ; v < pattern_size ; ++v)
            result.isomorphism.emplace(pattern_order.at(v), target_order.at(assignments.at(v)));

        int t = 0;
        for (auto & v : isolated_vertices) {
            while (result.isomorphism.end() != std::find_if(result.isomorphism.begin(), result.isomorphism.end(),
                        [&t] (const std::pair<int, int> & p) { return p.second == t; }))
                    ++t;
            result.isomorphism.emplace(v, t);
        }
    }

    auto run() -> Result
    {
        if (full_pattern_size > target_size) {
            /* some of our fixed size data structures will throw a hissy
             * fit. check this early. */
            return result;
        }

        parallel_build_supplemental_graphs(params.n_threads);

        Domains domains(pattern_size);

        if (! initialise_domains(domains, max_graphs))
            return result;

        FailedVariables dummy_failed_variables;
        if (! cheap_all_different(domains, dummy_failed_variables))
            return result;

        prepare_for_search(domains);

        Assignments assignments;
        std::atomic<unsigned long long> nodes{ 0 };
        Position position;
        switch (search(assignments, domains, nodes, max_graphs, 0, position, [] () -> bool { return false; }).first) {
            case Search::Satisfiable:
                save_result(assignments, result);
                break;

            case Search::Unsatisfiable:
                break;

            case Search::Aborted:
                break;
        }

        tasks.kill_workers();
        for (auto & t : tasks.times)
            result.times.push_back(t);

        help_points.kill_workers();
        for (auto & t : help_points.times)
            result.times.push_back(t);

        result.nodes = nodes.load();

        return result;
    }
};

template <template <unsigned, bool, int, int> class SGI_, bool b_, int n_, int m_>
struct Apply
{
    template <unsigned size_, typename> using Type = SGI_<size_, b_, n_, m_>;
};

auto threaded_subgraph_isomorphism(const std::pair<Graph, Graph> & graphs, const Params & params) -> Result
{
    if (graphs.first.size() > graphs.second.size())
        return Result{ };
    return select_graph_size<Apply<ThreadedSubgraphIsomorphism, true, 3, 3>::template Type, Result>(
            AllGraphSizes(), graphs.second, graphs.first, params);
}

