#ifndef DTE3611_DAY4_BREADTH_FIRST_SEARCH_H
#define DTE3611_DAY4_BREADTH_FIRST_SEARCH_H

// concepts
#include "../utils/concepts/graphs.h"

// boost
#include <boost/graph/adjacency_list.hpp>

namespace dte3611::graph::algorithms
{
  template <predef::concepts::graph::BidirectionalGraph Graph_T>
  std::vector<typename Graph_T::vertex_descriptor>
  breadthFirstSearch([[maybe_unused]] Graph_T const& graph,
                     [[maybe_unused]]
                     typename Graph_T::vertex_descriptor const& start)
  {

    /* Bost Graph-concept type container API
     *
     * For Adjacency list api examples see:
     * https://www.boost.org/doc/libs/1_73_0/libs/graph/doc/adjacency_list.html
     */

    // Given vertice descriptor u
    using VertexDescriptor = typename Graph_T::vertex_descriptor;
    auto u = VertexDescriptor();

    // Access adjacent connected vertices [descriptors]
    [[maybe_unused]]auto const adj_verts = boost::adjacent_vertices(u, graph);

    // In and out edges [descriptors]
    [[maybe_unused]]auto const in_edges  = boost::in_edges(u, graph);
    [[maybe_unused]]auto const out_edges = boost::out_edges(u, graph);

    // Given edge descriptor e
    using EdgeDescriptor = typename Graph_T::edge_descriptor;
    auto const e = EdgeDescriptor();

    // Source and target vertices from edge [descriptors]
    [[maybe_unused]]auto const s = boost::source(e, graph);
    [[maybe_unused]]auto const t = boost::target(e, graph);

    // Given properties [vertexProperty].name and [edgeProperty].distance
    // Access bundled vertex and edge properties from descriptors
    [[maybe_unused]]auto const& vert_name   = graph[u].name;
    [[maybe_unused]]auto const& edge_length = graph[e].distance;

    // Access vertices or edges [iterators]
    auto [v_itr_begin, v_itr_end] = boost::vertices(graph);
    auto [e_itr_begin, e_itr_end] = boost::edges(graph);

    // Iterate over vertices and edges and
    // Access descriptor and properties from said iterators
    for(/**/; v_itr_begin != v_itr_end; ++v_itr_begin) {
      auto const  vi                      = v_itr_begin;
      auto const  v1                      = *vi;
      [[maybe_unused]]auto const& v1_name = graph[v1].name;
    }

    for(/**/; e_itr_begin != e_itr_end; ++e_itr_begin) {
      auto const ei                         = e_itr_begin;
      auto const e1                         = *ei;
      [[maybe_unused]]auto const& e1_length = graph[e1].distance;
    }

    using VertexDescriptor = typename Graph_T::vertex_descriptor;

    std::vector<VertexDescriptor> visited;
    std::vector<VertexDescriptor> nodes;
    std::list<VertexDescriptor> node_queue;

    //Initialize
    visited.push_back(start);
    node_queue.push_back(start);

    //VertexDescriptor u;

    while (not node_queue.empty()) {
        u = node_queue.front();
        if (u != start) {
            nodes.push_back(u);
        }
        node_queue.pop_front();

        auto adj_vertices = boost::adjacent_vertices(u,graph);

        for (auto i = adj_vertices.first; i != adj_vertices.second; ++i) {
            auto const vi = *i;
            //If vi has not been visited, add to node queue
            if (std::find(visited.begin(), visited.end(), vi) == visited.end()) {
                visited.push_back(vi);
                node_queue.push_back(vi);
            }
        }
    }

    return nodes;
  }

}   // namespace dte3611::graph::algorithms


#endif   // DTE3611_DAY4_BREADTH_FIRST_SEARCH_H
