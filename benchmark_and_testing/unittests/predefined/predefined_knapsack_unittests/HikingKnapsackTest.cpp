#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <algorithm>
#include <lib3611/w2_knapsack/knapsack.h>
#include <gtest/gtest.h>

namespace detail {
struct HikingItem {
    std::string name;
    int value;    // Utilité
    int weight;   // Poids en grammes

    // Pour les comparaisons
    bool operator==(const HikingItem& other) const = default;
};
}

class RealHikingKnapsackTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto const Tente          = detail::HikingItem{"Tente",          8,  2500};
        auto const SacCouchage    = detail::HikingItem{"Sac de couchage", 9,  1800};
        auto const Rechaud        = detail::HikingItem{"Réchaud",        7,   800};
        auto const Nourriture     = detail::HikingItem{"Nourriture",     10, 2000};
        auto const Gourde         = detail::HikingItem{"Gourde",         10,  500};
        auto const Lampe          = detail::HikingItem{"Lampe frontale", 8,   200};
        auto const Secours        = detail::HikingItem{"Trousse secours", 9,   300};
        auto const Carte          = detail::HikingItem{"Carte",          6,   100};
        auto const Vetements      = detail::HikingItem{"Vêtements",      7,  1200};
        auto const AppareilPhoto  = detail::HikingItem{"Appareil photo", 5,   600};

        test_sets.push_back({
            {Tente, SacCouchage, Rechaud, Nourriture, Gourde, Lampe, Secours, Carte, Vetements, AppareilPhoto},
            8000, // Capacité
            {false, true, true, true, true, true, true, true, true, true}, // Sélection corrigée
            71    // Valeur totale corrigée
        });
    }

    void TearDown() override {
        test_sets.clear();
    }

    // Types identiques à l'original
    using Items    = std::vector<detail::HikingItem>;
    using Capacity = int;
    using K01Gold  = std::vector<bool>;
    using K01GoldV = int;

    using TestSet  = std::tuple<Items, Capacity, K01Gold, K01GoldV>;
    using TestSets = std::vector<TestSet>;

    TestSets test_sets;
};

TEST_F(RealHikingKnapsackTest, OptimalHikingEquipmentWithDetailedOutput)
{
    for (auto const& [items, capacity, gold, gold_v] : test_sets) {
        std::vector<bool> my_01knapsack;

        auto result = dte3611::np::algorithms::knapsack_01(
            items,
            std::back_inserter(my_01knapsack),
            capacity,
            &detail::HikingItem::value,
            &detail::HikingItem::weight
            );

        std::reverse(my_01knapsack.begin(), my_01knapsack.end());

        // === AFFICHAGE DÉTAILLÉ EN CAS D'ERREUR ===
        if (my_01knapsack != gold || result != gold_v) {
            std::cout << "\n❌ ERREUR DETECTEE ❌" << std::endl;
            std::cout << "==========================================" << std::endl;

            // Afficher la sélection obtenue
            std::cout << "SELECTION OBTENUE:" << std::endl;
            int total_weight = 0;
            int total_value = 0;

            for (size_t i = 0; i < items.size(); ++i) {
                std::string status = my_01knapsack[i] ? "✓ PRIS" : "✗ LAISSE";
                if (my_01knapsack[i] != gold[i]) {
                    status += " ❌ (ERREUR)";
                }
                std::cout << status << " - " << items[i].name
                          << " (" << items[i].weight << "g, "
                          << items[i].value << " util)" << std::endl;

                if (my_01knapsack[i]) {
                    total_weight += items[i].weight;
                    total_value += items[i].value;
                }
            }

            std::cout << "\n📊 STATISTIQUES OBTENUES:" << std::endl;
            std::cout << "Poids total: " << total_weight << "g / " << capacity << "g"
                      << " (" << (total_weight > capacity ? "❌ DEPASSEMENT" : "✓ OK") << ")" << std::endl;
            std::cout << "Valeur totale: " << total_value << " / attendu: " << gold_v
                      << " (" << (total_value == gold_v ? "✓ OK" : "❌ DIFFERENT") << ")" << std::endl;

            // Afficher ce qui était attendu
            std::cout << "\n🎯 SELECTION ATTENDUE:" << std::endl;
            for (size_t i = 0; i < items.size(); ++i) {
                std::string status = gold[i] ? "✓ PRIS" : "✗ LAISSE";
                std::cout << status << " - " << items[i].name << std::endl;
            }

            std::cout << "==========================================" << std::endl;
        }

        // Assertions normales (échouent toujours si faux)
        EXPECT_EQ(result, gold_v);
        EXPECT_EQ(my_01knapsack, gold);

        // Vérification supplémentaire du poids
        int total_weight = 0;
        for (size_t i = 0; i < items.size(); ++i) {
            if (my_01knapsack[i]) {
                total_weight += items[i].weight;
            }
        }
        EXPECT_LE(total_weight, capacity);
    }
}
