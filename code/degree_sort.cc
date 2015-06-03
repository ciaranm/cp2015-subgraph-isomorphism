/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#include "degree_sort.hh"

#include <vector>
#include <algorithm>

auto degree_sort(const Graph & graph, std::vector<int> & p, bool reverse) -> void
{
    // pre-calculate degrees
    std::vector<int> degrees;
    std::transform(p.begin(), p.end(), std::back_inserter(degrees),
            [&] (int v) { return graph.degree(v); });

    // sort on degree
    std::sort(p.begin(), p.end(),
            [&] (int a, int b) { return (! reverse) ^ (degrees[a] < degrees[b] || (degrees[a] == degrees[b] && a > b)); });
}

