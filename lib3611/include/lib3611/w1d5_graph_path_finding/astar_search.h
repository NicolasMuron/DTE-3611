#ifndef DTE3611_DAY5_ASTAR_SEARCH_H
#define DTE3611_DAY5_ASTAR_SEARCH_H

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
#include <set>

// Boost includes
#include <boost/graph/graph_traits.hpp>

namespace dte3611::graph::algorithms
{

template <
    predef::concepts::graph::BidirectionalGraph Graph_T,
    predef::concepts::graph::ShortestPathHeuristicOperator<Graph_T> HOp_T,
    predef::concepts::graph::EdgeDistanceOperator<Graph_T> EdOp_T =
    operators::EdgeDistanceOperator<>>
std::vector<std::vector<typename Graph_T::vertex_descriptor>> aStarSearch(
    Graph_T const& graph,
    typename Graph_T::vertex_descriptor const& start,
    typename Graph_T::vertex_descriptor const& goal,
    HOp_T h_op,
    EdOp_T distance_op = EdOp_T()) {

    using VertexDescriptor = typename Graph_T::vertex_descriptor;
    using EdgeDescriptor = typename Graph_T::edge_descriptor;
    using DistanceType = std::decay_t<decltype(distance_op(EdgeDescriptor{}, graph))>;

    // Structures de données
    std::unordered_map<VertexDescriptor, DistanceType> g_score;
    std::unordered_map<VertexDescriptor, std::vector<VertexDescriptor>> predecessors;

    // Initialisation
    auto [vertex_begin, vertex_end] = boost::vertices(graph);
    for (auto it = vertex_begin; it != vertex_end; ++it) {
        g_score[*it] = std::numeric_limits<DistanceType>::max();
    }

    g_score[start] = DistanceType{0};
    predecessors[start] = {};

    // File de priorité: (f_score, vertex)
    using QueueElement = std::pair<DistanceType, VertexDescriptor>;
    std::priority_queue<QueueElement, std::vector<QueueElement>,
                        std::greater<QueueElement>> open_set;

    DistanceType start_f_score = g_score[start] + h_op(start, goal, graph);
    open_set.push({start_f_score, start});

    DistanceType best_goal_distance = std::numeric_limits<DistanceType>::max();

    while (!open_set.empty()) {
        auto [current_f, current] = open_set.top();
        open_set.pop();

        // Si on a déjà un meilleur chemin vers le but, ignorer
        if (current_f > best_goal_distance) {
            continue;
        }

        // Si c'est le but, enregistrer la distance
        if (current == goal) {
            best_goal_distance = g_score[current];
            continue; // Continuer pour trouver tous les chemins
        }

        // Explorer les voisins
        auto [edge_begin, edge_end] = boost::out_edges(current, graph);
        for (auto edge_it = edge_begin; edge_it != edge_end; ++edge_it) {
            EdgeDescriptor edge = *edge_it;
            VertexDescriptor neighbor = boost::target(edge, graph);

            DistanceType edge_weight = distance_op(edge, graph);
            DistanceType tentative_g_score = g_score[current] + edge_weight;

            if (tentative_g_score < g_score[neighbor]) {
                g_score[neighbor] = tentative_g_score;
                predecessors[neighbor].clear();
                predecessors[neighbor].push_back(current);

                DistanceType f_score = tentative_g_score + h_op(neighbor, goal, graph);
                open_set.push({f_score, neighbor});
            }
            else if (tentative_g_score == g_score[neighbor]) {
                predecessors[neighbor].push_back(current);
            }
        }
    }

    // Si le but est inaccessible
    if (best_goal_distance == std::numeric_limits<DistanceType>::max()) {
        return {};
    }

    // Reconstruction de tous les chemins optimaux - CORRIGÉ
    std::vector<std::vector<VertexDescriptor>> all_paths;

    // Utiliser une stack pour reconstruire les chemins dans le bon ordre
    std::vector<std::pair<VertexDescriptor, std::vector<VertexDescriptor>>> stack;
    stack.push_back({goal, {}});

    while (!stack.empty()) {
        auto [current, path_so_far] = stack.back();
        stack.pop_back();

        // Si on est au start, on a un chemin complet
        if (current == start) {
            // Le chemin est dans l'ordre start→goal, donc on l'ajoute directement
            all_paths.push_back(path_so_far);
            continue;
        }

        // Ajouter tous les prédécesseurs
        if (predecessors.find(current) != predecessors.end()) {
            for (VertexDescriptor pred : predecessors.at(current)) {
                // Vérifier qu'on ne crée pas de cycle
                if (std::find(path_so_far.begin(), path_so_far.end(), pred) == path_so_far.end()) {
                    std::vector<VertexDescriptor> new_path = path_so_far;
                    // Ajouter le nœud courant au DÉBUT du chemin
                    new_path.insert(new_path.begin(), current);
                    stack.push_back({pred, new_path});
                }
            }
        }
    }

    // Trier pour ordre cohérent
    std::sort(all_paths.begin(), all_paths.end());

    return all_paths;
}

}   // namespace dte3611::graph::algorithms

#endif   // DTE3611_DAY5_ASTAR_SEARCH_H
