#ifndef DTE3611_WEEK2_SUBSET_SUM_H
#define DTE3611_WEEK2_SUBSET_SUM_H

// stl
#include <vector>
#include <ranges>
#include <numeric>
#include <functional> // Ajout pour std::function
#include <algorithm>  // Ajout pour std::transform

namespace dte3611::np::algorithms {

namespace detail {

struct subset_sum_fn {

    template <typename Value_T>
    using SubsetSum = std::vector<Value_T>;

    template <typename Value_T>
    using SubsetSum_Output = std::vector<std::vector<Value_T>>;

    /**************************
     *  Iterator Range Operator
     */
    template <std::forward_iterator         Iterator_T,
             std::sentinel_for<Iterator_T> Sentinel_T,
             typename Value_T, typename Projection_T = std::identity>

    SubsetSum_Output<Value_T>
    operator()(Iterator_T begin, Sentinel_T end, Value_T target,
               Projection_T projection = {}) const
    {
        // Appliquer la projection aux éléments
        std::vector<Value_T> arr;
        arr.reserve(std::distance(begin, end));
        std::transform(begin, end, std::back_inserter(arr),
                       [&projection](const auto& val) {
                           return std::invoke(projection, val);
                       });

        size_t n = arr.size();

        // Vérifier si target est négatif (non supporté)
        if (target < 0) return {};

        // Utiliser size_t pour l'indexation
        using Index_T = size_t;
        Index_T target_idx = static_cast<Index_T>(target);

        // DP table
        std::vector<std::vector<bool>> dp(n + 1,
                                          std::vector<bool>(target_idx + 1, false));
        dp[0][0] = true;

        // Remplir la table DP
        for (Index_T i = 1; i <= n; ++i) {
            for (Index_T t = 0; t <= target_idx; ++t) {
                dp[i][t] = dp[i-1][t];
                if (t >= static_cast<Index_T>(arr[i-1]) && arr[i-1] <= target) {
                    dp[i][t] = dp[i][t] || dp[i-1][t - static_cast<Index_T>(arr[i-1])];
                }
            }
        }

        SubsetSum_Output<Value_T> output;

        // Si pas de solution
        if (!dp[n][target_idx]) return output;

        // Backtracking optimisé - meilleure performance
        std::function<void(Index_T, Index_T, SubsetSum<Value_T>&)> backtrack =
            [&](Index_T i, Index_T t, SubsetSum<Value_T>& subset) {
                if (t == 0) {
                    output.push_back(subset);
                    return;
                }
                if (i == 0) return;

                // Exclure l'élément i-1
                if (dp[i-1][t]) {
                    backtrack(i-1, t, subset);
                }

                // Inclure l'élément i-1
                Value_T element = arr[i-1];
                Index_T element_idx = static_cast<Index_T>(element);
                if (t >= element_idx && element <= target && dp[i-1][t - element_idx]) {
                    subset.push_back(element); // on ajoute à la fin
                    backtrack(i-1, t - element_idx, subset);
                    subset.pop_back();
                }
            };

        SubsetSum<Value_T> subset;
        backtrack(n, target_idx, subset);

        for (auto& solution : output) {
            std::vector<Value_T> ordered_solution;

            // Parcourir le tableau original
            for (const auto& original_element : arr) {
                // Chercher si cet élément est dans la solution
                auto it = std::find(solution.begin(), solution.end(), original_element);
                if (it != solution.end()) {
                    ordered_solution.push_back(original_element);
                    // Retirer l'élément pour éviter les doublons
                    solution.erase(it);
                }
            }
            solution = std::move(ordered_solution);
        }

        return output;
    }

    /******************
     *  Ranges Operator
     */
    template <std::ranges::forward_range Range_T,
             typename Value_T, typename Projection_T = std::identity>
    SubsetSum_Output<Value_T>
    operator()(Range_T&& range, Value_T target,
               Projection_T projection = {}) const
    {
        return (*this)(std::ranges::begin(range), std::ranges::end(range),
                       target, projection);
    }
};

} // namespace detail

inline constexpr detail::subset_sum_fn subset_sum{};

} // namespace dte3611::np::algorithms

#endif // DTE3611_WEEK2_SUBSET_SUM_H
