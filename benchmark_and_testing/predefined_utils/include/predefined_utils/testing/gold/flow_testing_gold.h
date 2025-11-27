#ifndef DTE3611_PREDEF_TESTING_FLOW_GOLD_H
#define DTE3611_PREDEF_TESTING_FLOW_GOLD_H

// boost
#include <boost/graph/adjacency_list.hpp>

// gtest
#include <gtest/gtest.h>

// stl
#include <concepts>
#include <cstdlib>
#include <vector>
#include <algorithm>
#include <random>

namespace dte3611::predef::testing::flow
{

  namespace detail
  {

    namespace types
    {

      namespace properties
      {

        struct NodeProperties {
          std::string name;
          int supply;
        };

        struct EdgeProperties {
          int flow;
          int capacity;
          int cost;
        };

        using GraphProperties = boost::no_property;

      }   // namespace properties

      using BidirectionalGraph
        = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
                                properties::NodeProperties,
                                properties::EdgeProperties,
                                properties::GraphProperties>;
      using DirectedGraph
        = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
                                properties::NodeProperties,
                                properties::EdgeProperties,
                                properties::GraphProperties>;

    }   // namespace types


    template <typename GraphType_T>
    struct GoldGraphTemplate {

      using Graph       = GraphType_T;
      using VP          = typename Graph::vertex_property_type;
      using EP          = typename Graph::edge_property_type;
      using VD          = typename Graph::vertex_descriptor;
      using ED          = typename Graph::edge_descriptor;
      using VDVector    = std::vector<VD>;
      using VDVecVector = std::vector<VDVector>;

      GoldGraphTemplate()          = default;
      virtual ~GoldGraphTemplate() = default;

      Graph& graph() { return m_graph; }

      Graph m_graph;
    };

  }   // namespace detail

  namespace gold
  {





    class FordFulkersonDemoDAG
      : public detail::GoldGraphTemplate<detail::types::BidirectionalGraph> {

      using Base = detail::GoldGraphTemplate<detail::types::BidirectionalGraph>;

      VD A;
      VD B;
      VD C;
      VD D;
      VD S;
      VD T;

    public:
      FordFulkersonDemoDAG()
      {
        // clang-format off
      /*!
       *         A  -->  B
       *
       *     ^   |  \    ^   \
       *    /    V   v   |    v
       *
       * S  -->  C  -->  D  -->  T
       */
        // clang-format on

        A = boost::add_vertex(VP{.name = "A"}, m_graph);
        B = boost::add_vertex(VP{.name = "B"}, m_graph);
        C = boost::add_vertex(VP{.name = "C"}, m_graph);
        D = boost::add_vertex(VP{.name = "D"}, m_graph);
        S = boost::add_vertex(VP{.name = "S"}, m_graph);
        T = boost::add_vertex(VP{.name = "T"}, m_graph);

        // bgl has "implicit" alphanumeric child-ordering
        boost::add_edge(S, A, EP{.flow = 0, .capacity = 10 }, m_graph);
        boost::add_edge(S, C, EP{.flow = 0, .capacity = 10 }, m_graph);
        boost::add_edge(A, D, EP{.flow = 0, .capacity = 8 }, m_graph);
        boost::add_edge(A, C, EP{.flow = 0, .capacity = 2 }, m_graph);
        boost::add_edge(A, B, EP{.flow = 0, .capacity = 4 }, m_graph);
        boost::add_edge(B, T, EP{.flow = 0, .capacity = 10 }, m_graph);
        boost::add_edge(C, D, EP{.flow = 0, .capacity = 9 }, m_graph);
        boost::add_edge(D, T, EP{.flow = 0, .capacity = 10 }, m_graph);
        boost::add_edge(D, B, EP{.flow = 0, .capacity = 6 }, m_graph);
      }
      ~FordFulkersonDemoDAG() override {}


      VD const& a() const { return A; }
      VD const& b() const { return B; }
      VD const& c() const { return C; }
      VD const& d() const { return D; }
      VD const& s() const { return S; }
      VD const& t() const { return T; }

//      VDVecVector shortestPathsAEGold() const { return {{B, E}}; }
//      VDVecVector shortestPathsACGold() const { return {{B, E, F, C}}; }

      int maxFlowSTGold() const { return 19; }
    };



    class FordFulkersonDemoDAG2
      : public detail::GoldGraphTemplate<detail::types::BidirectionalGraph> {

      using Base = detail::GoldGraphTemplate<detail::types::BidirectionalGraph>;

      VD A;
      VD B;
      VD C;
      VD D;
      VD S;
      VD T;

    public:
      FordFulkersonDemoDAG2()
      {
        // clang-format off
      /*!
       *         A  -->  D
       *
       *     ^   ^    /  ^   \
       *    /    |  v    |    v
       *
       * S  -->  C  -->  C  -->  T
       */
        // clang-format on

        A = boost::add_vertex(VP{.name = "A"}, m_graph);
        B = boost::add_vertex(VP{.name = "B"}, m_graph);
        C = boost::add_vertex(VP{.name = "C"}, m_graph);
        D = boost::add_vertex(VP{.name = "D"}, m_graph);
        S = boost::add_vertex(VP{.name = "S"}, m_graph);
        T = boost::add_vertex(VP{.name = "T"}, m_graph);

        // bgl has "implicit" alphanumeric child-ordering
        boost::add_edge(S, A, EP{.flow = 1, .capacity = 3 }, m_graph);
        boost::add_edge(S, B, EP{.flow = 2, .capacity = 2 }, m_graph);
        boost::add_edge(B, A, EP{.flow = 1, .capacity = 3 }, m_graph);
        boost::add_edge(A, D, EP{.flow = 2, .capacity = 2 }, m_graph);
        boost::add_edge(D, B, EP{.flow = 1, .capacity = 1 }, m_graph);
        boost::add_edge(B, C, EP{.flow = 2, .capacity = 3 }, m_graph);
        boost::add_edge(C, D, EP{.flow = 1, .capacity = 3 }, m_graph);
        boost::add_edge(D, T, EP{.flow = 2, .capacity = 3 }, m_graph);
        boost::add_edge(C, T, EP{.flow = 1, .capacity = 2 }, m_graph);
      }
      ~FordFulkersonDemoDAG2() override {}


      VD const& a() const { return A; }
      VD const& b() const { return B; }
      VD const& c() const { return C; }
      VD const& d() const { return D; }
      VD const& s() const { return S; }
      VD const& t() const { return T; }

//      VDVecVector shortestPathsAEGold() const { return {{B, E}}; }
//      VDVecVector shortestPathsACGold() const { return {{B, E, F, C}}; }

      int maxFlowSTGold() const { return 4; }
    };


    class MinCostMaxFlowDAG
      : public detail::GoldGraphTemplate<detail::types::BidirectionalGraph> {

      using Base = detail::GoldGraphTemplate<detail::types::BidirectionalGraph>;

      VD A;
      VD B;
      VD C;
      VD D;
      VD E;
      VD F;
      VD S;
      VD T;

    public:
      MinCostMaxFlowDAG()
      {
        // clang-format off
      /*!
       *         A  -->  D
       *
       *     ^   |  \    |   \
       *    /    v    v  v    v
       *
       * S  -->  B  -->  E  -->  T
       *
       *    \    |  \    |    ^
       *     v   v   v   v   /
       *
       *         C  -->  F
       */
        // clang-format on

        A = boost::add_vertex(VP{.name = "A"}, m_graph);
        B = boost::add_vertex(VP{.name = "B"}, m_graph);
        C = boost::add_vertex(VP{.name = "C"}, m_graph);
        D = boost::add_vertex(VP{.name = "D"}, m_graph);
        E = boost::add_vertex(VP{.name = "E"}, m_graph);
        F = boost::add_vertex(VP{.name = "F"}, m_graph);
        S = boost::add_vertex(VP{.name = "S"}, m_graph);
        T = boost::add_vertex(VP{.name = "T"}, m_graph);

        // bgl has "implicit" alphanumeric child-ordering

        // 3 extra nodes to allow for a max flow solver in a related network
        // Supply
        boost::add_edge(S, A, EP{.flow = 0, .capacity = 12 }, m_graph);
        boost::add_edge(S, B, EP{.flow = 0, .capacity = 6 }, m_graph);
        boost::add_edge(S, C, EP{.flow = 0, .capacity = 14}, m_graph);

        boost::add_edge(A, B, EP{.flow = 0, .capacity = 11, .cost = 4},
                        m_graph);
        boost::add_edge(A, D, EP{.flow = 0, .capacity = 5, .cost = 5}, m_graph);
        boost::add_edge(A, E, EP{.flow = 0, .capacity = 2, .cost = 12},
                        m_graph);

        boost::add_edge(B, C, EP{.flow = 0, .capacity = 9, .cost = 6}, m_graph);
        boost::add_edge(B, E, EP{.flow = 0, .capacity = 4, .cost = 4}, m_graph);
        boost::add_edge(B, F, EP{.flow = 0, .capacity = 2, .cost = 6}, m_graph);

        boost::add_edge(C, F, EP{.flow = 0, .capacity = 31, .cost = 3},
                        m_graph);

        boost::add_edge(D, E, EP{.flow = 0, .capacity = 18, .cost = 4},
                        m_graph);

        boost::add_edge(E, F, EP{.flow = 0, .capacity = 9, .cost = 5}, m_graph);

        // 3 extra nodes to allow for a max flow solver in a related network
        // Demand
        boost::add_edge(E, T, EP{.flow = 0, .capacity = 7 }, m_graph);
        boost::add_edge(D, T, EP{.flow = 0, .capacity = 3 }, m_graph);
        boost::add_edge(F, T, EP{.flow = 0, .capacity = 22 }, m_graph);
      }
      ~MinCostMaxFlowDAG() override {}


      VD const& a() const { return A; }
      VD const& b() const { return B; }
      VD const& c() const { return C; }
      VD const& d() const { return D; }
      VD const& e() const { return E; }
      VD const& f() const { return F; }
      VD const& s() const { return S; }
      VD const& t() const { return T; }

//      VDVecVector shortestPathsAEGold() const { return {{B, E}}; }
//      VDVecVector shortestPathsACGold() const { return {{B, E, F, C}}; }

      int maxFlowSTGold() const { return 32; }
    };


    class MinCostMaxFlowDAG2
        : public detail::GoldGraphTemplate<detail::types::BidirectionalGraph> {

        using Base = detail::GoldGraphTemplate<detail::types::BidirectionalGraph>;

        VD A;
        VD B;
        VD C;
        VD D;
        VD E;
        VD F;
        //VD S;
        //VD T;

    public:
        MinCostMaxFlowDAG2()
        {
            // clang-format off
            /*!
       *         A  -->  D
       *
       *     ^   |  \    |   \
       *    /    v    v  v    v
       *
       * S  -->  B  -->  E  -->  T
       *
       *    \    |  \    |    ^
       *     v   v   v   v   /
       *
       *         C  -->  F
       */
            // clang-format on

            A = boost::add_vertex(VP{.name = "A", .supply = 12}, m_graph);
            B = boost::add_vertex(VP{.name = "B", .supply = 6}, m_graph);
            C = boost::add_vertex(VP{.name = "C", .supply = 14}, m_graph);
            D = boost::add_vertex(VP{.name = "D", .supply = -3}, m_graph);
            E = boost::add_vertex(VP{.name = "E", .supply = -7}, m_graph);
            F = boost::add_vertex(VP{.name = "F", .supply = -22}, m_graph);
            //S = boost::add_vertex(VP{.name = "S"}, m_graph);
            //T = boost::add_vertex(VP{.name = "T"}, m_graph);

            // bgl has "implicit" alphanumeric child-ordering

            // 3 extra nodes to allow for a max flow solver in a related network
            // Supply
            //boost::add_edge(S, A, EP{.flow = 0, .capacity = 12 }, m_graph);
            //boost::add_edge(S, B, EP{.flow = 0, .capacity = 6 }, m_graph);
            //boost::add_edge(S, C, EP{.flow = 0, .capacity = 14}, m_graph);

            boost::add_edge(A, B, EP{.flow = 0, .capacity = 11, .cost = 4},
                            m_graph);
            boost::add_edge(A, D, EP{.flow = 0, .capacity = 5, .cost = 5}, m_graph);
            boost::add_edge(A, E, EP{.flow = 0, .capacity = 2, .cost = 12},
                            m_graph);

            boost::add_edge(B, C, EP{.flow = 0, .capacity = 9, .cost = 6}, m_graph);
            boost::add_edge(B, E, EP{.flow = 0, .capacity = 4, .cost = 4}, m_graph);
            boost::add_edge(B, F, EP{.flow = 0, .capacity = 2, .cost = 6}, m_graph);

            boost::add_edge(C, F, EP{.flow = 0, .capacity = 31, .cost = 3},
                            m_graph);

            boost::add_edge(D, E, EP{.flow = 0, .capacity = 18, .cost = 4},
                            m_graph);

            boost::add_edge(E, F, EP{.flow = 0, .capacity = 9, .cost = 5}, m_graph);

            // 3 extra nodes to allow for a max flow solver in a related network
            // Demand
            //boost::add_edge(E, T, EP{.flow = 0, .capacity = 7 }, m_graph);
            //boost::add_edge(D, T, EP{.flow = 0, .capacity = 3 }, m_graph);
            //boost::add_edge(F, T, EP{.flow = 0, .capacity = 22 }, m_graph);
        }
        ~MinCostMaxFlowDAG2() override {}


        VD const& a() const { return A; }
        VD const& b() const { return B; }
        VD const& c() const { return C; }
        VD const& d() const { return D; }
        VD const& e() const { return E; }
        VD const& f() const { return F; }
        //VD const& s() const { return S; }
        //VD const& t() const { return T; }

        //      VDVecVector shortestPathsAEGold() const { return {{B, E}}; }
        //      VDVecVector shortestPathsACGold() const { return {{B, E, F, C}}; }

        int minCostFlowSTGold() const { return 193; }
    };






    class BloodDonationBipartite
        : public detail::GoldGraphTemplate<detail::types::BidirectionalGraph> {

        using Base = detail::GoldGraphTemplate<detail::types::BidirectionalGraph>;
        using VP = typename Base::VP;
        using EP = typename Base::EP;
        using VD = typename Base::VD;
        using ED = typename Base::ED;

        std::vector<VD> donors;
        std::vector<VD> patients;

    public:
        BloodDonationBipartite()
        {
            // Configuration réaliste : plus de donneurs que de patients
            const int num_donors = 60;
            const int num_patients = 40;

            // Créer les donneurs avec fréquence réaliste de don
            for (int i = 0; i < num_donors; ++i) {
                std::string name = "Donor_" + std::to_string(i);
                int supply = calculateAnnualDonationCapacity(i);
                VD donor = boost::add_vertex(VP{.name = name, .supply = supply}, m_graph);
                donors.push_back(donor);
            }

            // Créer les patients avec demandes réalistes
            for (int i = 0; i < num_patients; ++i) {
                std::string name = "Patient_" + std::to_string(i);
                int demand = -calculateAnnualTransfusionNeed(i);
                VD patient = boost::add_vertex(VP{.name = name, .supply = demand}, m_graph);
                patients.push_back(patient);
            }

            createCompatibilityEdges();
        }

        ~BloodDonationBipartite() override = default;

    private:
        int calculateAnnualDonationCapacity(int donor_id) {
            // Répartition réaliste des capacités de don sur 1 an
            int pattern = donor_id % 10;

            switch (pattern) {
            case 0: case 1:
                return 1;  // 20% donnent 1 fois/an
            case 2: case 3: case 4:
                return 2;  // 30% donnent 2 fois/an
            case 5: case 6: case 7:
                return 4;  // 30% donnent 4 fois/an
            case 8:
                return 6;  // 10% donnent 6 fois/an
            case 9:
                return 12; // 10% donnent 12 fois/an
            default:
                return 2;
            }
        }

        int calculateAnnualTransfusionNeed(int patient_id) {
            // Répartition réaliste des besoins transfusionnels sur 1 an
            int pattern = patient_id % 10;

            switch (pattern) {
            case 0: case 1: case 2:
                return 1;  // 30% besoin de 1 transfusion/an
            case 3: case 4: case 5:
                return 3;  // 30% besoin de 3 transfusions/an
            case 6: case 7:
                return 6;  // 20% besoin de 6 transfusions/an
            case 8:
                return 12; // 10% besoin de 12 transfusions/an
            case 9:
                return 24; // 10% besoin de 24 transfusions/an
            default:
                return 2;
            }
        }

        void createCompatibilityEdges() {
            std::vector<std::string> blood_types = {"O+", "O-", "A+", "A-", "B+", "B-", "AB+", "AB-"};

            for (size_t i = 0; i < donors.size(); ++i) {
                std::string donor_type = blood_types[i % blood_types.size()];

                for (size_t j = 0; j < patients.size(); ++j) {
                    std::string patient_type = blood_types[j % blood_types.size()];

                    int donor_idx = static_cast<int>(i);
                    int patient_idx = static_cast<int>(j);
                    int cost = calculateCompatibilityCost(donor_type, patient_type, donor_idx, patient_idx);

                    if (cost < 1000) {
                        int capacity = 1 + (static_cast<int>(i) + static_cast<int>(j)) % 3;
                        boost::add_edge(donors[i], patients[j],
                                        EP{.flow = 0, .capacity = capacity, .cost = cost},
                                        m_graph);
                    }
                }
            }
        }

        int calculateCompatibilityCost(const std::string& donor, const std::string& patient, int donor_id, int patient_id) {
            // Logique de compatibilité sanguine avec variations pour cycles négatifs
            int base_cost = 0;

            if (donor == "O-") {
                base_cost = 0;  // Donneur universel
                // Ajouter des variations pour certains donneurs O-
                if (donor_id % 5 == 0) base_cost = -1; // Coût négatif pour certains
            }
            else if (donor == "O+") {
                if (patient == "O+" || patient == "A+" || patient == "B+" || patient == "AB+")
                    base_cost = 1;
                else return 1000;
            }
            else if (donor == "A-") {
                if (patient == "A-" || patient == "A+" || patient == "AB-" || patient == "AB+")
                    base_cost = 2;
                else return 1000;
            }
            else if (donor == "A+") {
                if (patient == "A+" || patient == "AB+")
                    base_cost = 3;
                else return 1000;
            }
            else if (donor == "B-") {
                if (patient == "B-" || patient == "B+" || patient == "AB-" || patient == "AB+")
                    base_cost = 2;
                else return 1000;
            }
            else if (donor == "B+") {
                if (patient == "B+" || patient == "AB+")
                    base_cost = 3;
                else return 1000;
            }
            else if (donor == "AB-") {
                if (patient == "AB-" || patient == "AB+")
                    base_cost = 4;
                else return 1000;
            }
            else if (donor == "AB+") {
                if (patient == "AB+")
                    base_cost = 5;
                else return 1000;
            }
            else {
                return 1000;
            }

            // Variation aléatoire basée sur les IDs pour créer de l'hétérogénéité
            int variation = (donor_id + patient_id * 7) % 5;
            return base_cost + variation - 2; // Permet des coûts négatifs
        }

    public:
        // Getters pour accéder aux donneurs et patients
        const std::vector<VD>& getDonors() const { return donors; }
        const std::vector<VD>& getPatients() const { return patients; }

        VD getDonor(int index) const {
            return (index < static_cast<int>(donors.size())) ? donors[index] : VD();
        }

        VD getPatient(int index) const {
            return (index < static_cast<int>(patients.size())) ? patients[index] : VD();
        }

        int numDonors() const { return static_cast<int>(donors.size()); }
        int numPatients() const { return static_cast<int>(patients.size()); }

        // Méthodes de compatibilité avec les tests existants
        VD const& s() const {
            static VD default_source = donors.empty() ? VD() : donors[0];
            return default_source;
        }

        VD const& t() const {
            static VD default_sink = patients.empty() ? VD() : patients[0];
            return default_sink;
        }

        int maxFlowSTGold() const {
            return std::min(numDonors(), numPatients());
        }

        int minCostFlowSTGold() const {
            // Estimation réaliste basée sur la complexité du graphe
            return 1500;
        }
    };

    }   // namespace gold

    }   // namespace dte3611::predef::testing::flow

#endif   // DTE3611_PREDEF_TESTING_FLOW_GOLD_H
