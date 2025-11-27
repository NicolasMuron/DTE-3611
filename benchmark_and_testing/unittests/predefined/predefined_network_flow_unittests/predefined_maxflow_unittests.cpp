// Unit test utils
#include <predefined_utils/testing/fixtures/flow_testing_fixtures.h>

// Week 3 network flow library
#include <lib3611/w3_network_flow/network_flow.h>

// gtest
#include <gtest/gtest.h>   // googletest header file

// stl
#include <vector>
#include <chrono>

// Qualify predefined fixtures
using namespace dte3611::predef::testing::flow::fixtures;

namespace alg = dte3611::np::algorithms;

TEST_F(FordFulkersonDemoDAGF, FordFulkersonMaxFlowDemo)
{
  auto max_flow = alg::maxFlow(gold->graph(), gold->s(), gold->t());
  auto gold_flow = gold->maxFlowSTGold();
  EXPECT_EQ(max_flow, gold_flow);
}

TEST_F(FordFulkersonDemoDAG2F, FordFulkersonMaxFlowDemo2)
{
  auto max_flow = alg::maxFlow(gold->graph(), gold->s(), gold->t());
  auto gold_flow = gold->maxFlowSTGold();
  EXPECT_EQ(max_flow, gold_flow);
}

TEST_F(MinCostMaxFlowDAG, FordFulkersonMinCostFlowDag)
{
  auto max_flow = alg::maxFlow(gold->graph(), gold->s(), gold->t());
  auto gold_flow = gold->maxFlowSTGold();
  EXPECT_EQ(max_flow, gold_flow);
}

TEST_F(MinCostMaxFlowDAG2, FordFulkersonMinCostFlowDag2)
{
    auto min_flow = alg::cycleCanceling(gold->graph());
    auto gold_flow = gold->minCostFlowSTGold();
    EXPECT_EQ(min_flow, gold_flow);
}

TEST_F(BloodDonationBipartiteF, BasicStructureTest)
{
    // Test de structure basique
    EXPECT_EQ(gold->numDonors(), 60);
    EXPECT_EQ(gold->numPatients(), 40);
    EXPECT_EQ(boost::num_vertices(gold->graph()), 100);
}

TEST_F(BloodDonationBipartiteF, MinCostFlowTest)
{
    auto graph = gold->graph();

    // Utiliser cycleCanceling sans source/sink spécifiques
    // (si votre implémentation le permet)
    auto min_cost = alg::cycleCanceling(graph);

    // Vérifications de base
    EXPECT_GE(min_cost, 0);
    EXPECT_LT(min_cost, 10000); // Doit être réalisable
}

TEST_F(BloodDonationBipartiteF, CustomSizeBenchmark)
{
    std::cout << "\n🚀 BENCHMARK CYCLE CANCELING - Taille Personnalisée\n";
    std::cout << "==================================================\n";

    // ⚡ MODIFIEZ CES NOMBRES POUR TESTER DIFFÉRENTES TAILLES ⚡
    const int NUM_DONORS = 60;    // ← Changez ce nombre
    const int NUM_PATIENTS = 40;  // ← Changez ce nombre

    std::cout << "Configuration: " << NUM_DONORS << " donneurs, " << NUM_PATIENTS << " patients\n";

    // Mesure du temps
    auto start_time = std::chrono::high_resolution_clock::now();

    // Créer un graphe de taille custom (vous devrez adapter votre fixture)
    // Pour l'instant, on utilise le graphe existant mais on affiche les vraies tailles
    auto graph_copy = gold->graph();
    auto min_cost = alg::cycleCanceling(graph_copy);

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    // Résultats
    std::cout << "📊 RÉSULTATS:\n";
    std::cout << "   ⏱️  Temps d'exécution: " << duration.count() << " ms\n";
    std::cout << "   💰 Coût minimum: " << min_cost << "\n";
    std::cout << "   📈 Sommets réels: " << boost::num_vertices(gold->graph()) << "\n";
    std::cout << "   🔗 Arêtes réelles: " << boost::num_edges(gold->graph()) << "\n";
    std::cout << "   🚀 Performance: "
              << (boost::num_edges(gold->graph()) / (duration.count() > 0 ? duration.count() : 1))
              << " arêtes/ms\n";

    // Validation
    EXPECT_GE(min_cost, 0);
    EXPECT_LT(min_cost, 100000);

    std::cout << "✅ BENCHMARK TERMINÉ\n";
    std::cout << "==================================================\n\n";
}

