#ifndef DTE3611_WEEK1_CUSTOM_AA_SORT_H
#define DTE3611_WEEK1_CUSTOM_AA_SORT_H

// stl
#include <iterator>
#include <algorithm>
#include <functional>

namespace dte3611::sort::algorithms
{

namespace detail
{

struct custom_aa_sort_fn {

    /**************************
       *  Iterator Range Operator
       */

    // Type Generics
    template <std::random_access_iterator   Iterator_T,
             std::sentinel_for<Iterator_T> Sentinel_T,
             typename Compare_T    = std::ranges::less,
             typename Projection_T = std::identity>
    // Algorithm type requirements
        requires std::sortable<Iterator_T, Compare_T, Projection_T>
    // Return value
    constexpr Iterator_T
    // Call-operator signature
    operator()(Iterator_T first, Sentinel_T last,
               Compare_T comp = {}, Projection_T proj = {}) const
    {
        // Convertir le sentinel en iterator si nécessaire
        auto end = std::ranges::next(first, last);

        // Tri par sélection - simple mais éducatif
        for (auto i = first; i != end; ++i) {
            // Trouver l'élément minimum dans la partie non triée
            auto min_it = i;
            for (auto j = std::next(i); j != end; ++j) {
                // Utiliser la comparaison et la projection
                if (std::invoke(comp,
                                std::invoke(proj, *j),
                                std::invoke(proj, *min_it))) {
                    min_it = j;
                }
            }

            // Échanger l'élément minimum avec la position courante
            if (min_it != i) {
                std::iter_swap(i, min_it);
            }
        }

        return end;
    }

    /******************
       *  Ranges Operator
       */

    // Type Generics
    template <std::ranges::random_access_range Range_T,
             typename Compare_T    = std::ranges::less,
             typename Projection_T = std::identity>
    // Algorithm type requirements
        requires std::sortable<std::ranges::iterator_t<Range_T>, Compare_T,
                               Projection_T>
    // Return value
    constexpr std::ranges::borrowed_iterator_t<Range_T>
    // Call-operator signature
    operator()(Range_T&& range, Compare_T comp = {},
               Projection_T proj = {}) const
    {
        return (*this)(std::ranges::begin(range), std::ranges::end(range),
                       std::move(comp), std::move(proj));
    }
};

}   // namespace detail

// Niebloid API Instantiation
inline constexpr detail::custom_aa_sort_fn custom_aa_sort{};

}   // namespace dte3611::sort::algorithms

#endif   // DTE3611_WEEK1_CUSTOM_AA_SORT_H
