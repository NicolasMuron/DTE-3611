#include <benchmark/benchmark.h>
#include <predefined_utils/testing/fixtures/flow_testing_fixtures.h>
#include <lib3611/w3_network_flow/network_flow.h>
#include <vector>
#include <memory>

using namespace dte3611::predef::testing::flow::fixtures;
namespace alg = dte3611::np::algorithms;

// Compteur global pour les itérations
static int g_iteration_count = 0;

// Version instrumentée de cycleCanceling
template<typename Graph_T>
int cycleCancelingWithCounter(Graph_T& graph) {
    g_iteration_count = 0;
    // Vous devrez modifier votre cycleCanceling pour incrémenter g_iteration_count
    // à chaque début de boucle while
    return alg::cycleCanceling(graph);
}

// Benchmark pour les graphes de test existants
static void BM_CycleCanceling_MinCostMaxFlowDAG2(benchmark::State& state) {
    auto gold = std::make_unique<MinCostMaxFlowDAG2>();

    for (auto _ : state) {
        auto graph_copy = gold->graph();
        int result = cycleCancelingWithCounter(graph_copy);
        benchmark::DoNotOptimize(result);
    }

    // Métriques supplémentaires
    state.counters["Iterations"] = g_iteration_count;
    state.counters["Vertices"] = boost::num_vertices(gold->graph());
    state.counters["Edges"] = boost::num_edges(gold->graph());
}
BENCHMARK(BM_CycleCanceling_MinCostMaxFlowDAG2);

// Benchmark pour le graphe biparti sanguin
static void BM_CycleCanceling_BloodDonation(benchmark::State& state) {
    auto gold = std::make_unique<BloodDonationBipartite>();

    for (auto _ : state) {
        auto graph_copy = gold->graph();
        int result = cycleCancelingWithCounter(graph_copy);
        benchmark::DoNotOptimize(result);
    }

    state.counters["Iterations"] = g_iteration_count;
    state.counters["Donors"] = gold->numDonors();
    state.counters["Patients"] = gold->numPatients();
    state.counters["Vertices"] = gold->numDonors() + gold->numPatients();
    state.counters["Edges"] = boost::num_edges(gold->graph());
}
BENCHMARK(BM_CycleCanceling_BloodDonation);

// Benchmark de scalabilité avec différentes tailles
static void BM_CycleCanceling_Scalability(benchmark::State& state) {
    const int donors = state.range(0);
    const int patients = state.range(1);

    // Créer un graphe custom de taille spécifique
    // (Vous devrez adapter BloodDonationBipartite pour accepter des paramètres)
    auto custom_graph = createCustomBloodDonationGraph(donors, patients);

    for (auto _ : state) {
        auto graph_copy = custom_graph->graph();
        int result = cycleCancelingWithCounter(graph_copy);
        benchmark::DoNotOptimize(result);
    }

    state.counters["Iterations"] = g_iteration_count;
    state.counters["Donors"] = donors;
    state.counters["Patients"] = patients;
    state.counters["Edges"] = boost::num_edges(custom_graph->graph());
}

// Configurations de scalabilité
BENCHMARK(BM_CycleCanceling_Scalability)
    ->Args({50, 30})      // Petit
    ->Args({100, 70})     // Moyen
    ->Args({200, 150})    // Grand
    ->Args({500, 300})    // Très grand
    ->Args({1000, 700});  // Énorme

// Benchmark avec différents ratios donneurs/patients
static void BM_CycleCanceling_Ratios(benchmark::State& state) {
    const int total_vertices = state.range(0);
    const double ratio = state.range(1) / 100.0; // Convertir le pourcentage

    const int donors = static_cast<int>(total_vertices * ratio);
    const int patients = total_vertices - donors;

    auto custom_graph = createCustomBloodDonationGraph(donors, patients);

    for (auto _ : state) {
        auto graph_copy = custom_graph->graph();
        int result = cycleCancelingWithCounter(graph_copy);
        benchmark::DoNotOptimize(result);
    }

    state.counters["Ratio"] = ratio;
    state.counters["Donors"] = donors;
    state.counters["Patients"] = patients;
}
BENCHMARK(BM_CycleCanceling_Ratios)
    ->ArgsProduct({
        {200},           // Total vertices
        {25, 50, 75}     // Ratio donneurs (%)
    });

// Benchmark pour mesurer l'impact de la densité du graphe
static void BM_CycleCanceling_Density(benchmark::State& state) {
    const int donors = 100;
    const int patients = 100;
    const double density = state.range(0) / 100.0; // Densité en %

    auto custom_graph = createCustomBloodDonationGraphWithDensity(donors, patients, density);

    for (auto _ : state) {
        auto graph_copy = custom_graph->graph();
        int result = cycleCancelingWithCounter(graph_copy);
        benchmark::DoNotOptimize(result);
    }

    state.counters["Density"] = density;
    state.counters["TheoreticalEdges"] = donors * patients * density;
}
BENCHMARK(BM_CycleCanceling_Density)
    ->DenseRange(10, 90, 20); // Densités de 10% à 90%

// Benchmark avec coûts négatifs (pour tester les cycles)
static void BM_CycleCanceling_NegativeCosts(benchmark::State& state) {
    const int negative_ratio = state.range(0); // % d'arêtes avec coût négatif

    auto custom_graph = createBloodDonationWithNegativeCosts(negative_ratio);

    for (auto _ : state) {
        auto graph_copy = custom_graph->graph();
        int result = cycleCancelingWithCounter(graph_copy);
        benchmark::DoNotOptimize(result);
    }

    state.counters["NegativeCostRatio"] = negative_ratio;
    state.counters["Iterations"] = g_iteration_count;
}
BENCHMARK(BM_CycleCanceling_NegativeCosts)
    ->DenseRange(0, 50, 10); // 0% à 50% d'arêtes négatives

// Benchmark de la mémoire
static void BM_CycleCanceling_Memory(benchmark::State& state) {
    auto gold = std::make_unique<BloodDonationBipartite>();

    for (auto _ : state) {
        auto graph_copy = gold->graph();
        int result = cycleCancelingWithCounter(graph_copy);
        benchmark::DoNotOptimize(result);
    }

    // Estimation mémoire
    size_t vertex_memory = boost::num_vertices(gold->graph()) * sizeof(typename BloodDonationBipartite::VP);
    size_t edge_memory = boost::num_edges(gold->graph()) * sizeof(typename BloodDonationBipartite::EP);

    state.counters["MemoryVerticesKB"] = vertex_memory / 1024.0;
    state.counters["MemoryEdgesKB"] = edge_memory / 1024.0;
    state.counters["MemoryTotalKB"] = (vertex_memory + edge_memory) / 1024.0;
}
BENCHMARK(BM_CycleCanceling_Memory);

// Fonctions helpers à implémenter
std::unique_ptr<BloodDonationBipartite> createCustomBloodDonationGraph(int donors, int patients) {
    // À implémenter - créer un graphe avec les tailles spécifiées
    return std::make_unique<BloodDonationBipartite>();
}

std::unique_ptr<BloodDonationBipartite> createCustomBloodDonationGraphWithDensity(int donors, int patients, double density) {
    // À implémenter - créer un graphe avec densité contrôlée
    return std::make_unique<BloodDonationBipartite>();
}

std::unique_ptr<BloodDonationBipartite> createBloodDonationWithNegativeCosts(int negative_ratio) {
    // À implémenter - créer un graphe avec pourcentage de coûts négatifs
    return std::make_unique<BloodDonationBipartite>();
}

BENCHMARK_MAIN();
