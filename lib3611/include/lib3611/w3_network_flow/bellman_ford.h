#ifndef DTE3611_WEEK2_BELLMAN_FORD_H
#define DTE3611_WEEK2_BELLMAN_FORD_H

#include "../w1d5_graph_path_finding/operators.h"

// concepts
#include "../utils/concepts/graphs.h"
#include "../utils/concepts/operators.h"

// stl
#include <vector>
#include <unordered_map>
#include <limits>
#include <stdexcept>
#include <algorithm>
#include <functional>
#include <type_traits>
// utils
#include "../utils/graph_utils.h"

// Boost
#include <boost/property_map/function_property_map.hpp>
#include <boost/graph/graph_traits.hpp>

namespace dte3611::graph::algorithms
{

template <predef::concepts::graph::BidirectionalGraph            Graph_T,
         predef::concepts::graph::EdgeCapacityOperator<Graph_T> EdOp_T
         = operators::EdgeDistanceOperator<>>
std::vector<std::vector<typename Graph_T::vertex_descriptor>>
bellmanFordShortestPaths(Graph_T const& graph,
                         typename Graph_T::vertex_descriptor const& start,
                         typename Graph_T::vertex_descriptor const& goal,
                         EdOp_T distance_op = EdOp_T())
{
    using VertexDescriptor = typename Graph_T::vertex_descriptor;
    using DistanceType     = double;

    // Initialisation
    std::unordered_map<VertexDescriptor, DistanceType> d;
    std::unordered_map<VertexDescriptor, VertexDescriptor> predecessor;

    // Initialiser toutes les distances à l'infini
    auto [vi, vi_end] = boost::vertices(graph);
    for (; vi != vi_end; ++vi) {
        d[*vi] = std::numeric_limits<DistanceType>::infinity();
    }

    // Distance du start à 0
    d[start] = 0;

    size_t n = boost::num_vertices(graph);

    // Bellman-Ford standard
    for (size_t i = 1; i < n; ++i) {
        bool changed = false;

        // Parcourir toutes les arêtes
        auto [ei, ei_end] = boost::edges(graph);
        for (; ei != ei_end; ++ei) {
            VertexDescriptor u = boost::source(*ei, graph);
            VertexDescriptor v = boost::target(*ei, graph);
            DistanceType weight = distance_op(*ei, graph);

            if (d[u] != std::numeric_limits<DistanceType>::infinity() &&
                d[u] + weight < d[v]) {
                d[v] = d[u] + weight;
                predecessor[v] = u;  // u est le prédécesseur de v
                changed = true;
            }
        }
        if (!changed) break;
    }

    // Reconstruction du chemin
    std::vector<std::vector<VertexDescriptor>> result;

    // Si aucun chemin n'existe
    if (d[goal] == std::numeric_limits<DistanceType>::infinity()) {
        return result;
    }

    // Construire le chemin en remontant les prédécesseurs
    std::vector<VertexDescriptor> path;
    VertexDescriptor current = goal;

    // Remonter de goal à start
    while (current != start) {
        path.push_back(current);
        current = predecessor[current];

        // Éviter les boucles infinies
        if (path.size() > n) {
            break;
        }
    }

    // Inverser le chemin pour avoir start -> goal
    std::reverse(path.begin(), path.end());

    // Resultat
    if (!path.empty() && path.back() == goal) {
        result.push_back(path);
    }

    return result;
}

} // namespace dte3611::graph::algorithms

#endif
