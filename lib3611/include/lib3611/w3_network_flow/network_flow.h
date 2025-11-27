#ifndef DTE3611_WEEK3_NETWORK_FLOW_H
#define DTE3611_WEEK3_NETWORK_FLOW_H

#include "../w3_network_flow/bellman_ford.h"
#include "../w1d5_graph_path_finding/operators.h"

// concepts
#include "../utils/concepts/graphs.h"
#include "../utils/concepts/operators.h"

// stl
#include <vector>
#include <unordered_map>
#include <queue>

// utils
#include "../utils/graph_utils.h"

// boost
#include <boost/graph/edge_list.hpp>
#include <boost/property_map/function_property_map.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>


#include "operators.h"

namespace dte3611::np::algorithms
{


template <predef::concepts::graph::BidirectionalGraph Graph_T,
         predef::concepts::graph::EdgeCapacityOperator<Graph_T> EdCOp_T =
         graph::operators::DefaultEdgeCapacityOperator<>,
         predef::concepts::graph::EdgeCapacityOperator<Graph_T> EdFOp_T =
         graph::operators::DefaultEdgeFlowOperator<>>


bool find_augmenting_path(Graph_T const& gf,
                          typename Graph_T::vertex_descriptor const& source,
                          typename Graph_T::vertex_descriptor const& sink,
                          std::vector<typename Graph_T::edge_descriptor>& path) {

    using VertexDescriptor = typename Graph_T::vertex_descriptor;
    using EdgeDescriptor = typename Graph_T::edge_descriptor;

    std::map<VertexDescriptor, EdgeDescriptor> predecessor;
    std::queue<VertexDescriptor> q;

    q.push(source);
    predecessor[source] = EdgeDescriptor();

    while (!q.empty()) {
        VertexDescriptor u = q.front();
        q.pop();

        if (u==sink) {
            path.clear();
            VertexDescriptor current = sink;

            while (current != source) {
                EdgeDescriptor e = predecessor[current];
                path.push_back(e);
                current = boost::source(e,gf);
            }

            std::reverse(path.begin(), path.end());
            return true;
        }

        auto [out_begin, out_end] = boost::out_edges(u,gf);
        for (auto e = out_begin; e != out_end; ++e) {
            VertexDescriptor v = boost::target(*e, gf);
            if (predecessor.find(v) == predecessor.end()) {
                predecessor[v] = *e;
                q.push(v);
            }
        }
    }
    return false;

}

  template <predef::concepts::graph::BidirectionalGraph Graph_T,
            predef::concepts::graph::EdgeCapacityOperator<Graph_T> EdCOp_T =
                graph::operators::DefaultEdgeCapacityOperator<>,
            predef::concepts::graph::EdgeCapacityOperator<Graph_T> EdFOp_T =
                graph::operators::DefaultEdgeFlowOperator<>>

  int maxFlow([[maybe_unused]] Graph_T& graph,
              [[maybe_unused]] typename Graph_T::vertex_descriptor const& source,
              [[maybe_unused]] typename Graph_T::vertex_descriptor const& sink,
              [[maybe_unused]] EdCOp_T capacity_op = EdCOp_T(),
              [[maybe_unused]] EdFOp_T flow_op = EdFOp_T()) {
    // Implement Ford-Fulkerson (method) for computing the maximum flow in a flow
    // network by augmenting path.
    // Note: Edmonds-Karp is a fully defined implementation of the Ford-Fulkerson
    // method.

    using VertexDescriptor = typename Graph_T::vertex_descriptor;
    using EdgeDescriptor = typename Graph_T::edge_descriptor;

    std::map<typename Graph_T::edge_descriptor, int> flow;
    std::map<EdgeDescriptor, int> capacite_residuelle;

    //Initialisation
    auto [edge_begin, edge_end] = boost::edges(graph);
    for (auto edge_it = edge_begin; edge_it != edge_end; ++edge_it) {
        flow[*edge_it] = flow_op(*edge_it, graph);
    }

    Graph_T gf;

    auto [ei, ei_end] = boost::edges(graph);
    for (auto e = ei; e != ei_end; ++e) {
        auto u = boost::source(*e, graph);
        auto v = boost::target(*e, graph);

        // Arête forward : u → v
        auto capacite_residuelle_actuelle = capacity_op(*e, graph) - flow_op(*e, graph);
        if (capacite_residuelle_actuelle > 0) {
            auto [new_edge, added] = boost::add_edge(u, v, gf);
            capacite_residuelle[new_edge] = capacite_residuelle_actuelle;
        }

        // Arête backward : v → u (inverse)
        auto flux_actuel = flow_op(*e, graph);
        if (flux_actuel > 0) {
            auto [new_edge, added] = boost::add_edge(v, u, gf);
            capacite_residuelle[new_edge] = flux_actuel;
        }
    }

    std::vector<EdgeDescriptor> path;
    while (find_augmenting_path(gf, source, sink, path)) {
        //AUGMENT
        int delta = capacite_residuelle[path[0]];
        for (size_t i = 1; i<path.size(); ++i) {
            if (capacite_residuelle[path[i]] < delta) {
                delta = capacite_residuelle[path[i]];
            }
        }

        for (EdgeDescriptor e_gf : path) {
            VertexDescriptor u = boost::source(e_gf, gf);
            VertexDescriptor v = boost::target(e_gf, graph);
            auto [edge_directe, exists_directe] = boost::edge(u,v,graph);
            if (exists_directe) {
                flow[edge_directe] = flow[edge_directe] + delta;
            } else {
                auto [edge_indirecte, exists_indirecte] = boost::edge(v,u,graph);
                if (exists_indirecte) {
                    flow[edge_indirecte] = flow[edge_indirecte] - delta;
                }
            }
        }

        //Update Gf
        gf.clear();
        capacite_residuelle.clear();
        auto [ei2, ei_end2] = boost::edges(graph);
        for (auto e = ei2; e != ei_end2; ++e) {
            auto u = boost::source(*e, graph);
            auto v = boost::target(*e, graph);

            // Arête forward : u → v
            auto capacite_residuelle_actuelle = capacity_op(*e, graph) - flow[*e];
            if (capacite_residuelle_actuelle > 0) {
                auto [new_edge, added] = boost::add_edge(u, v, gf);
                capacite_residuelle[new_edge] = capacite_residuelle_actuelle;
            }

            // Arête backward : v → u (inverse)
            auto flux_actuel = flow[*e];
            if (flux_actuel > 0) {
                auto [new_edge, added] = boost::add_edge(v, u, gf);
                capacite_residuelle[new_edge] = flux_actuel;
            }
        }
    }
    //Calcul flow
    int total_flow = 0;
    auto [out_begin, out_end] = boost::out_edges(source, graph);
    for (auto e = out_begin; e!= out_end; ++e) {
        total_flow += flow[*e];
    }
    return total_flow;
  }

  template <predef::concepts::graph::BidirectionalGraph Graph_T,
           predef::concepts::graph::EdgeCapacityOperator<Graph_T> EdCOp_T =
           graph::operators::DefaultEdgeCapacityOperator<>,
           predef::concepts::graph::EdgeCapacityOperator<Graph_T> EdFOp_T =
           graph::operators::DefaultEdgeFlowOperator<>>
  std::map<typename Graph_T::edge_descriptor,int>
  maxFlow2([[maybe_unused]] Graph_T& graph,
           [[maybe_unused]] typename Graph_T::vertex_descriptor const& source,
           [[maybe_unused]] typename Graph_T::vertex_descriptor const& sink,
           [[maybe_unused]] EdCOp_T capacity_op = EdCOp_T(),
           [[maybe_unused]] EdFOp_T flow_op = EdFOp_T()) {
      // Implement Ford-Fulkerson (method) for computing the maximum flow in a flow
      // network by augmenting path.
      // Note: Edmonds-Karp is a fully defined implementation of the Ford-Fulkerson
      // method.

      using VertexDescriptor = typename Graph_T::vertex_descriptor;
      using EdgeDescriptor = typename Graph_T::edge_descriptor;

      std::map<typename Graph_T::edge_descriptor, int> flow;
      std::map<EdgeDescriptor, int> capacite_residuelle;

      //Initialisation
      auto [edge_begin, edge_end] = boost::edges(graph);
      for (auto edge_it = edge_begin; edge_it != edge_end; ++edge_it) {
          flow[*edge_it] = flow_op(*edge_it, graph);
      }

      Graph_T gf;

      auto [ei, ei_end] = boost::edges(graph);
      for (auto e = ei; e != ei_end; ++e) {
          auto u = boost::source(*e, graph);
          auto v = boost::target(*e, graph);

          // Arête forward : u → v
          auto capacite_residuelle_actuelle = capacity_op(*e, graph) - flow_op(*e, graph);
          if (capacite_residuelle_actuelle > 0) {
              auto [new_edge, added] = boost::add_edge(u, v, gf);
              capacite_residuelle[new_edge] = capacite_residuelle_actuelle;
          }

          // Arête backward : v → u (inverse)
          auto flux_actuel = flow_op(*e, graph);
          if (flux_actuel > 0) {
              auto [new_edge, added] = boost::add_edge(v, u, gf);
              capacite_residuelle[new_edge] = flux_actuel;
          }
      }

      std::vector<EdgeDescriptor> path;
      while (find_augmenting_path(gf, source, sink, path)) {
          //AUGMENT
          int delta = capacite_residuelle[path[0]];
          for (size_t i = 1; i<path.size(); ++i) {
              if (capacite_residuelle[path[i]] < delta) {
                  delta = capacite_residuelle[path[i]];
              }
          }

          for (EdgeDescriptor e_gf : path) {
              VertexDescriptor u = boost::source(e_gf, gf);
              VertexDescriptor v = boost::target(e_gf, graph);
              auto [edge_directe, exists_directe] = boost::edge(u,v,graph);
              if (exists_directe) {
                  flow[edge_directe] = flow[edge_directe] + delta;
              } else {
                  auto [edge_indirecte, exists_indirecte] = boost::edge(v,u,graph);
                  if (exists_indirecte) {
                      flow[edge_indirecte] = flow[edge_indirecte] - delta;
                  }
              }
          }

          //Update Gf
          gf.clear();
          capacite_residuelle.clear();
          auto [ei2, ei_end2] = boost::edges(graph);
          for (auto e = ei2; e != ei_end2; ++e) {
              auto u = boost::source(*e, graph);
              auto v = boost::target(*e, graph);

              // Arête forward : u → v
              auto capacite_residuelle_actuelle = capacity_op(*e, graph) - flow[*e];
              if (capacite_residuelle_actuelle > 0) {
                  auto [new_edge, added] = boost::add_edge(u, v, gf);
                  capacite_residuelle[new_edge] = capacite_residuelle_actuelle;
              }

              // Arête backward : v → u (inverse)
              auto flux_actuel = flow[*e];
              if (flux_actuel > 0) {
                  auto [new_edge, added] = boost::add_edge(v, u, gf);
                  capacite_residuelle[new_edge] = flux_actuel;
              }
          }
      }
      return flow;
  }

  template <predef::concepts::graph::BidirectionalGraph Graph_T,
           predef::concepts::graph::EdgeCapacityOperator<Graph_T> EdCoOp_T =
           graph::operators::DefaultEdgeCostOperator<>>
  std::vector<typename Graph_T::edge_descriptor>
  cycle_negatif(Graph_T const& graph,
                std::map<typename Graph_T::edge_descriptor, int> cout) {

      using VertexDescriptor = typename Graph_T::vertex_descriptor;
      using EdgeDescriptor = typename Graph_T::edge_descriptor;

      size_t n = boost::num_vertices(graph);
      std::vector<VertexDescriptor> vertices_list;
      auto [vi, vi_end] = boost::vertices(graph);
      for (; vi != vi_end; ++vi) {
          vertices_list.push_back(*vi);
      }

      // Initialisation
      std::map<VertexDescriptor, long long> d;
      std::map<VertexDescriptor, VertexDescriptor> predecessor;
      std::map<VertexDescriptor, EdgeDescriptor> edge_predecessor;

      for (auto v : vertices_list) {
          d[v] = 0;
          predecessor[v] = v;
      }

      VertexDescriptor cycle_vertex = vertices_list[0];
      bool has_cycle = false;

      // Bellman-Ford
      for (size_t i = 0; i < n; ++i) {
          bool changed = false;

          auto [ei, ei_end] = boost::edges(graph);
          for (; ei != ei_end; ++ei) {
              VertexDescriptor u = boost::source(*ei, graph);
              VertexDescriptor v = boost::target(*ei, graph);

              auto it = cout.find(*ei);
              if (it == cout.end()) {
                  continue;
              }

              long long weight = it->second;

              if (d[u] + weight < d[v]) {
                  d[v] = d[u] + weight;
                  predecessor[v] = u;
                  edge_predecessor[v] = *ei;
                  changed = true;

                  if (i == n - 1) {
                      has_cycle = true;
                      cycle_vertex = v;
                  }
              }
          }

          if (!changed) break;
      }

      if (has_cycle) {
          std::vector<EdgeDescriptor> cycle_edges;
          std::vector<VertexDescriptor> cycle_vertices;

          // Trouver un sommet dans le cycle
          VertexDescriptor in_cycle = cycle_vertex;
          for (size_t i = 0; i < n; ++i) {
              in_cycle = predecessor[in_cycle];
          }

          // Reconstruire le cycle
          VertexDescriptor current = in_cycle;
          std::set<VertexDescriptor> visited;

          do {
              if (visited.count(current)) break;
              visited.insert(current);
              cycle_vertices.push_back(current);

              VertexDescriptor next = predecessor[current];

              auto edge_it = edge_predecessor.find(current);
              if (edge_it != edge_predecessor.end()) {
                  cycle_edges.push_back(edge_it->second);
              } else {
                  auto [edge, exists] = boost::edge(predecessor[current], current, graph);
                  if (exists) {
                      cycle_edges.push_back(edge);
                  }
              }

              current = next;

          } while (current != in_cycle && visited.size() <= n);

          // INVERSER avant d'afficher
          std::reverse(cycle_edges.begin(), cycle_edges.end());
          std::reverse(cycle_vertices.begin(), cycle_vertices.end());

          return cycle_edges;
      }
      return {};
  }

  template <typename Graph_T,
           predef::concepts::graph::EdgeCapacityOperator<Graph_T> EdCOp_T =
           graph::operators::DefaultEdgeCapacityOperator<>,
           predef::concepts::graph::EdgeCapacityOperator<Graph_T> EdFOp_T =
           graph::operators::DefaultEdgeFlowOperator<>,
           predef::concepts::graph::EdgeCapacityOperator<Graph_T> EdCoOp_T =
           graph::operators::DefaultEdgeCostOperator<>>

  int cycleCanceling([[maybe_unused]] Graph_T& graph,
                     //[[maybe_unused]] typename Graph_T::vertex_descriptor const& source,
                     //[[maybe_unused]] typename Graph_T::vertex_descriptor const& sink,
                     [[maybe_unused]] EdCOp_T capacity_op = EdCOp_T(),
                     [[maybe_unused]] EdFOp_T flow_op = EdFOp_T(),
                     [[maybe_unused]] EdCoOp_T cost_op = EdCoOp_T())
  {

      using VertexDescriptor = typename Graph_T::vertex_descriptor;
      using EdgeDescriptor = typename Graph_T::edge_descriptor;
      Graph_T graph_usuel = graph;

      VertexDescriptor S = boost::add_vertex(graph_usuel);
      VertexDescriptor T = boost::add_vertex(graph_usuel);
      auto vertices = boost::vertices(graph);
      for (auto it = vertices.first; it != vertices.second; ++it) {
          VertexDescriptor v = *it;
          int supply_value = graph[v].supply;
          if (supply_value > 0) {
              EdgeDescriptor e_usuel1 = boost::add_edge(S, v, graph_usuel).first;
              graph_usuel[e_usuel1].capacity = supply_value;
              graph_usuel[e_usuel1].flow = 0;
              graph_usuel[e_usuel1].cost = 0;
          } else if (supply_value < 0) {
              EdgeDescriptor e_usuel2 = boost::add_edge(v, T, graph_usuel).first;
              graph_usuel[e_usuel2].capacity = -supply_value;
              graph_usuel[e_usuel2].flow = 0;
              graph_usuel[e_usuel2].cost = 0;
          }
      }

      std::map<typename Graph_T::edge_descriptor,int> feasible_flow = maxFlow2(graph_usuel, S, T, capacity_op, flow_op);

      //std::cout << "=== FLOW FEASIBLE (AVANT BOUCLE WHILE) ===" << std::endl;
      //int total_flow_before = 0;
      //for (const auto& [edge, flow] : feasible_flow) {
          //auto u = boost::source(edge, graph_usuel);
          //auto v = boost::target(edge, graph_usuel);
          //std::cout << "ARÊTE " << u << " -> " << v << " : flow=" << flow << std::endl;
          //total_flow_before += flow;
      //}
      //std::cout << "FLOW TOTAL AVANT BOUCLE: " << total_flow_before << std::endl;
      //std::cout << "==========================================" << std::endl;


      Graph_T gf;
      std::map<EdgeDescriptor, int> capacite_residuelle;
      std::map<EdgeDescriptor, int> cout_residuel;
      std::map<typename Graph_T::edge_descriptor, int> flow_gf;

      auto [ei, ei_end] = boost::edges(graph_usuel);
      for (auto e = ei; e != ei_end; ++e) {
          auto u = boost::source(*e, graph_usuel);
          auto v = boost::target(*e, graph_usuel);
          auto flux_actuel = feasible_flow[*e];
          flow_gf[*e] = flux_actuel;

          // Arête forward : u → v
          auto capacite_residuelle_actuelle = capacity_op(*e, graph_usuel) - flux_actuel;
          if (capacite_residuelle_actuelle > 0 ) {
              auto [new_edge, added] = boost::add_edge(u, v, gf);
              capacite_residuelle[new_edge] = capacite_residuelle_actuelle;
              cout_residuel[new_edge] = cost_op(*e, graph_usuel);
              flow_gf[new_edge] = feasible_flow[*e];
          }

          // Arête backward : v → u (inverse)
          if (flux_actuel > 0) {
              auto [new_edge, added] = boost::add_edge(v, u, gf);
              capacite_residuelle[new_edge] = flux_actuel;
              cout_residuel[new_edge] = -cost_op(*e, graph_usuel);
              flow_gf[new_edge] = 0;
          }
      }



      bool cycle_trouve = true;
      while (cycle_trouve) {
          auto cycle = cycle_negatif(gf, cout_residuel);

          int cout_cycle = 0;
          for (EdgeDescriptor e : cycle) {
              cout_cycle += cout_residuel[e];
          }

          if (cycle.empty()) {
              cycle_trouve = false;
              std::cout << "Aucun cycle négatif trouvé"<< std::endl;
          } else {
              std::cout << "Cycle négatif trouvé, coût: " << cout_cycle << std::endl;
              std::cout << "Taille du cycle: " << cycle.size() << " arêtes" << std::endl;

              // Trouver le delta (capacité résiduelle minimale)
              int delta = capacite_residuelle[cycle[0]];
              for (size_t i = 1; i < cycle.size(); ++i) {
                  if (capacite_residuelle[cycle[i]] < delta) {
                      delta = capacite_residuelle[cycle[i]];
                  }
              }

              // Mettre à jour le flot feasible_flow du graphe ORIGINAL
              for (EdgeDescriptor e_gf : cycle) {
                  VertexDescriptor u_gf = boost::source(e_gf, gf);
                  VertexDescriptor v_gf = boost::target(e_gf, gf);

                  // Chercher l'arête correspondante dans le graphe original
                  auto [ei_orig, ei_end_orig] = boost::edges(graph_usuel);
                  for (auto e_orig = ei_orig; e_orig != ei_end_orig; ++e_orig) {
                      VertexDescriptor u_orig = boost::source(*e_orig, graph_usuel);
                      VertexDescriptor v_orig = boost::target(*e_orig, graph_usuel);

                      // Si c'est une forward edge dans gf
                      if (u_gf == u_orig && v_gf == v_orig) {
                          flow_gf[*e_orig] += delta;  // Augmenter le flux
                      }
                      // Si c'est une backward edge dans gf (correspond à diminuer le flux original)
                      else if (u_gf == v_orig && v_gf == u_orig) {
                          flow_gf[*e_orig] -= delta;  // Diminuer le flux
                      }
                  }
              }


              // Mettre à jour le graphe résiduel gf AVEC LES NOUVELLES CAPACITÉS
              Graph_T new_gf;
              std::map<EdgeDescriptor, int> new_capacite;
              std::map<EdgeDescriptor, int> new_cout;

              auto [ei_orig, ei_end_orig] = boost::edges(graph_usuel);
              for (auto e_orig = ei_orig; e_orig != ei_end_orig; ++e_orig) {
                  auto u = boost::source(*e_orig, graph_usuel);
                  auto v = boost::target(*e_orig, graph_usuel);
                  auto flux_actuel = flow_gf[*e_orig];  // NOUVEAU flux après modification

                  // Forward edge
                  int cap_forward = capacity_op(*e_orig, graph_usuel) - flux_actuel;
                  if (cap_forward > 0) {
                      auto [new_edge, added] = boost::add_edge(u, v, new_gf);
                      new_capacite[new_edge] = cap_forward;
                      new_cout[new_edge] = cost_op(*e_orig, graph_usuel);
                  }

                  // Backward edge
                  if (flux_actuel > 0) {
                      auto [new_edge, added] = boost::add_edge(v, u, new_gf);
                      new_capacite[new_edge] = flux_actuel;
                      new_cout[new_edge] = -cost_op(*e_orig, graph_usuel);
                  }
              }

              // Mettre à jour les structures
              gf.clear();
              gf = std::move(new_gf);
              capacite_residuelle = std::move(new_capacite);
              cout_residuel = std::move(new_cout);
          }
      }
      //std::cout << "=== FLOW FINAL (APRÈS BOUCLE WHILE) ===" << std::endl;
      std::map<typename Graph_T::edge_descriptor, int> final_flow;
      auto [ei_final, ei_end_final] = boost::edges(graph_usuel);
      for (auto e = ei_final; e != ei_end_final; ++e) {
          final_flow[*e] = feasible_flow[*e];
          if (feasible_flow[*e] != flow_gf[*e]) {
              final_flow[*e] = flow_gf[*e];
          }
          //auto u = boost::source(*e, graph_usuel);
          //auto v = boost::target(*e, graph_usuel);
          //std::cout << "ARÊTE " << u << " -> " << v << " : flow=" << final_flow[*e]
                    //<< " (était: " << feasible_flow[*e] << ")" << std::endl;
      }

      int total_cost = 0;
      auto [out_begin, out_end] = boost::edges(graph_usuel);
      for (auto e = out_begin; e != out_end; ++e) {
          total_cost += final_flow[*e]*cost_op(*e, graph_usuel);
      }

      return total_cost;

  }


  }   // namespace dte3611::graph::algorithms


#endif   // DTE3611_WEEK3_NETWORK_FLOW_H
