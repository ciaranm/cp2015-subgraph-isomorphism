/* vim: set sw=4 sts=4 et foldmethod=syntax : */

#ifndef GUARD_SEQUENTIAL_HH
#define GUARD_SEQUENTIAL_HH 1

#include "params.hh"
#include "result.hh"
#include "graph.hh"

auto sequential_subgraph_isomorphism(const std::pair<Graph, Graph> & graphs, const Params & params) -> Result;
auto no_backjumping_subgraph_isomorphism(const std::pair<Graph, Graph> & graphs, const Params & params) -> Result;
auto no_supplementals_subgraph_isomorphism(const std::pair<Graph, Graph> & graphs, const Params & params) -> Result;
auto no_all_diff_subgraph_isomorphism(const std::pair<Graph, Graph> & graphs, const Params & params) -> Result;
auto full_all_diff_subgraph_isomorphism(const std::pair<Graph, Graph> & graphs, const Params & params) -> Result;

#endif
