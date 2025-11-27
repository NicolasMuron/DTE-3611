#ifndef DTE3611_DAY5_DIJKSTRA_SHORTEST_PATHS_H
#define DTE3611_DAY5_DIJKSTRA_SHORTEST_PATHS_H

#include "operators.h"

// concepts
#include "../utils/concepts/graphs.h"
#include "../utils/concepts/operators.h"

// stl
#include <vector>
#include <queue>
#include <unordered_map>
#include <limits>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <set>

// Boost includes
#include <boost/graph/graph_traits.hpp>

namespace dte3611::graph::algorithms
{

template <predef::concepts::graph::BidirectionalGraph            Graph_T,
         predef::concepts::graph::EdgeDistanceOperator<Graph_T> EdOp_T
         = operators::EdgeDistanceOperator<>>
std::vector<std::vector<typename Graph_T::vertex_descriptor>>
dijkstraShortestPaths(Graph_T const& graph,
                      typename Graph_T::vertex_descriptor const& start,
                      typename Graph_T::vertex_descriptor const& goal,
                      EdOp_T distance_op = EdOp_T())
{
    using VertexDescriptor = typename Graph_T::vertex_descriptor;
    using EdgeDescriptor = typename Graph_T::edge_descriptor;

    // Déterminer le type de retour de l'opérateur de distance
    using DistanceType = std::decay_t<decltype(distance_op(
        EdgeDescriptor{}, graph))>;

    // Structures de données pour l'algorithme
    std::unordered_map<VertexDescriptor, DistanceType> distances;
    std::unordered_map<VertexDescriptor, std::vector<VertexDescriptor>> predecessors;
    std::set<VertexDescriptor> visited;

    // Priority queue pour sélectionner le prochain nœud à visiter
    using QueueElement = std::pair<DistanceType, VertexDescriptor>;
    std::priority_queue<QueueElement, std::vector<QueueElement>,
                        std::greater<QueueElement>> pq;

    // Initialisation
    auto [vertex_begin, vertex_end] = boost::vertices(graph);
    for (auto it = vertex_begin; it != vertex_end; ++it) {
        distances[*it] = std::numeric_limits<DistanceType>::max();
    }

    distances[start] = DistanceType{};
    pq.push({DistanceType{}, start});

    // Algorithme de Dijkstra
    while (!pq.empty()) {
        VertexDescriptor u = pq.top().second;
        DistanceType current_distance = pq.top().first;
        pq.pop();

        // Si on a déjà visité ce nœud, on skip
        if (visited.find(u) != visited.end()) {
            continue;
        }
        visited.insert(u);

        // Parcourir tous les voisins
        auto [edge_begin, edge_end] = boost::out_edges(u, graph);
        for (auto edge_it = edge_begin; edge_it != edge_end; ++edge_it) {
            EdgeDescriptor edge = *edge_it;
            VertexDescriptor v = boost::target(edge, graph);

            // Calculer la nouvelle distance en utilisant l'arête
            DistanceType edge_weight = distance_op(edge, graph);
            DistanceType new_distance = current_distance + edge_weight;

            // Relaxation de l'arête
            if (new_distance < distances[v]) {
                distances[v] = new_distance;
                predecessors[v].clear();
                predecessors[v].push_back(u);
                pq.push({new_distance, v});
            }
            else if (new_distance == distances[v]) {
                // Si la distance est égale, c'est un autre chemin optimal
                predecessors[v].push_back(u);
            }
        }
    }

    // Si le but est inaccessible, retourner un vecteur vide
    if (distances.find(goal) == distances.end() ||
        distances[goal] == std::numeric_limits<DistanceType>::max()) {
        return {};
    }

    // Reconstruire tous les chemins les plus courts
    std::vector<std::vector<VertexDescriptor>> all_paths;

    // Fonction récursive pour reconstruire les chemins
    std::function<void(VertexDescriptor, std::vector<VertexDescriptor>&)>
        build_paths = [&](VertexDescriptor current, std::vector<VertexDescriptor>& current_path) {

            // Ajouter le nœud courant au chemin (sauf le start)
            if (current != start) {
                current_path.push_back(current);
            }

            // Si on est arrivé au start, on a un chemin complet
            if (current == start) {
                // Inverser pour avoir start->goal
                std::vector<VertexDescriptor> final_path(current_path.rbegin(), current_path.rend());
                all_paths.push_back(final_path);
                if (current != start) {
                    current_path.pop_back();
                }
                return;
            }

            // Pour chaque prédécesseur, continuer la reconstruction
            if (predecessors.find(current) != predecessors.end()) {
                for (VertexDescriptor pred : predecessors.at(current)) {
                    // Vérifier qu'on ne crée pas de cycle
                    if (std::find(current_path.begin(), current_path.end(), pred) == current_path.end()) {
                        build_paths(pred, current_path);
                    }
                }
            }

            if (current != start) {
                current_path.pop_back();
            }
        };

    std::vector<VertexDescriptor> temp_path;
    build_paths(goal, temp_path);

    // Trier les chemins pour avoir un ordre cohérent
    std::sort(all_paths.begin(), all_paths.end());

    return all_paths;
}

}   // namespace dte3611::graph::algorithms

#endif   // DTE3611_DAY5_DIJKSTRA_SHORTEST_PATHS_H
