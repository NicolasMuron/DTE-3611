// Unit test utils
#include <predefined_utils/testing/fixtures/flow_testing_fixtures.h>

// Week 3 network flow library
#include <lib3611/w3_network_flow/network_flow.h>

// gtest
#include <gtest/gtest.h>   // googletest header file

// stl
#include <vector>

// Qualify predefined fixtures
using namespace dte3611::predef::testing::flow::fixtures;

namespace alg = dte3611::np::algorithms;


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
