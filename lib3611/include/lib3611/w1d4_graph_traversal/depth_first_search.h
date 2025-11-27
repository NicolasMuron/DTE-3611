#ifndef DTE3611_DAY4_DEPTH_FIRST_SEARCH_H
#define DTE3611_DAY4_DEPTH_FIRST_SEARCH_H

// concepts
#include "../utils/concepts/graphs.h"

#include <boost/graph/adjacency_list.hpp>
#include <stack>
#include <vector>
#include <algorithm>

namespace dte3611::graph::algorithms
{

template <predef::concepts::graph::BidirectionalGraph Graph_T>
std::vector<typename Graph_T::vertex_descriptor>
depthFirstSearch(Graph_T const& graph,
                 typename Graph_T::vertex_descriptor const& start)
{
    using VertexDescriptor = typename Graph_T::vertex_descriptor;

    std::vector<VertexDescriptor> visited;
    std::stack<VertexDescriptor> node_stack;

    // Start with neighbors of the start node (don't include start itself)
    auto adj_range_start = boost::adjacent_vertices(start, graph);
    std::vector<VertexDescriptor> neighbors(adj_range_start.first, adj_range_start.second);
    std::reverse(neighbors.begin(), neighbors.end());

    for (VertexDescriptor neighbor : neighbors) {
        node_stack.push(neighbor);
    }

    while (!node_stack.empty()) {
        VertexDescriptor u = node_stack.top();
        node_stack.pop();

        if (std::find(visited.begin(), visited.end(), u) == visited.end()) {
            visited.push_back(u);

            // Get adjacent vertices for current node u
            auto adj_range = boost::adjacent_vertices(u, graph);
            std::vector<VertexDescriptor> next_neighbors(adj_range.first, adj_range.second);
            std::reverse(next_neighbors.begin(), next_neighbors.end());

            for (VertexDescriptor neighbor : next_neighbors) {
                if (std::find(visited.begin(), visited.end(), neighbor) == visited.end()) {
                    node_stack.push(neighbor);
                }
            }
        }
    }

    return visited;
}
}   // namespace dte3611::graph::algorithms

#endif   // DTE3611_DAY4_DEPTH_FIRST_SEARCH_H
