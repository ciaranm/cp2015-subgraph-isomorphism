/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include "argraph.h"
#include "argedit.h"
#include "argloader.h"
#include "match.h"
#include "vf2_mono_state.h"

#include <fstream>
#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <cstdlib>
#include <chrono>
#include <vector>

using std::chrono::milliseconds;
using std::chrono::steady_clock;
using std::chrono::duration_cast;

namespace
{
    void load(const std::string & file, ARGEdit & ed, std::vector<int> & loops)
    {
        std::ifstream f(file.c_str());

        int nv;
        f >> nv;
        loops.resize(nv);
        for (int i = 0 ; i < nv ; ++i)
            ed.InsertNode(&loops[i]);

        for (int r = 0 ; r < nv ; ++r) {
            int nn;
            f >> nn;
            for (int n = 0 ; n < nn ; ++n) {
                int nb;
                f >> nb;
                if (r == nb)
                    loops[r] = true;
                else
                    ed.InsertEdge(r, nb, NULL);
            }
        }

        if (! f)
            throw 0;
    }

    struct OurComparator : AttrComparator
    {
        virtual bool compatible(void *attr1, void *attr2)
        {
            return *static_cast<int *>(attr1) == *static_cast<int *>(attr2);
        }
    };
}

int main(int argc, char * argv[])
{
    if (argc != 4)
        return EXIT_FAILURE;

    std::ifstream pattern_in(argv[1], std::ios::in | std::ios::binary);
    std::ifstream target_in(argv[2], std::ios::in | std::ios::binary);

    int timeout = atoi(argv[3]);

    ARGEdit pattern_ed, target_ed;
    std::vector<int> pattern_loops, target_loops;

    load(argv[1], pattern_ed, pattern_loops);
    load(argv[2], target_ed, target_loops);

    int n;
    node_id ni1[8000], ni2[8000];

    std::thread timeout_thread;
    std::mutex timeout_mutex;
    std::condition_variable timeout_cv;

    std::atomic<int> state;
    state.store(0);

    /* Start the clock */
    auto start_time = steady_clock::now();

    timeout_thread = std::thread([&] {
        auto abort_time = steady_clock::now() + std::chrono::seconds(timeout);
        {
            /* Sleep until either we've reached the time limit,
             * or we've finished all the work. */
            std::unique_lock<std::mutex> guard(timeout_mutex);
            while (0 == state.load()) {
                if (std::cv_status::timeout == timeout_cv.wait_until(guard, abort_time)) {
                    /* We've woken up, and it's due to a timeout. */
                    int exp = 0;
                    if (state.compare_exchange_strong(exp, 2)) {
                        auto overall_time = duration_cast<milliseconds>(steady_clock::now() - start_time);
                        std::cout << "aborted" << std::endl;
                        std::cout << std::endl;
                        std::cout << overall_time.count() << std::endl;
                        exit(EXIT_SUCCESS);
                    }
                    break;
                }
            }
        }
    });

    Graph pattern_g(&pattern_ed), target_g(&target_ed);

    pattern_g.SetNodeComparator(new OurComparator);
    target_g.SetNodeComparator(new OurComparator);

    VF2MonoState s0(&pattern_g, &target_g);

    bool result = match(&s0, &n, ni1, ni2);

    auto overall_time = duration_cast<milliseconds>(steady_clock::now() - start_time);

    int exp = 0;
    if (state.compare_exchange_strong(exp, 1)) {
        if (! result) {
            std::cout << "false" << std::endl;
            std::cout << std::endl;
            std::cout << overall_time.count() << std::endl;
        }
        else {
            std::cout << "true" << std::endl;
            for (int i = 0 ; i < n ; i++)
                std::cout << "(" << ni1[i] << "=" << ni2[i] << ") ";
            std::cout << std::endl;
            std::cout << overall_time.count() << std::endl;
        }

        /* Clean up the timeout thread */
        if (timeout_thread.joinable()) {
            {
                std::unique_lock<std::mutex> guard(timeout_mutex);
                timeout_cv.notify_all();
            }
            timeout_thread.join();
        }

        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}

