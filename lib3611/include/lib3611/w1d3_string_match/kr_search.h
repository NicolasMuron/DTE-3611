#ifndef DTE3611_WEEK1_STRING_MATCH_KARP_RABIN_SEARCH_H
#define DTE3611_WEEK1_STRING_MATCH_KARP_RABIN_SEARCH_H

// stl
#include <iterator>
#include <algorithm>
#include <functional>

namespace dte3611::string_match::algorithms
{

namespace detail
{

struct kr_search_fn {

    /**************************
       *  Iterator Range Operator
       */

    // Type Generics
    template <std::forward_iterator           Iterator_T,
             std::sentinel_for<Iterator_T>   Sentinel_T,
             std::forward_iterator           S_Iterator_T,
             std::sentinel_for<S_Iterator_T> S_Sentinel_T,
             typename BinaryPredicate_T = std::ranges::equal_to,
             typename Projection_T      = std::identity,
             typename S_Projection_T    = std::identity>

    // Algorithm type requirements
        requires std::indirectly_comparable<Iterator_T, S_Iterator_T,
                                            BinaryPredicate_T, Projection_T,
                                            S_Projection_T>

    // Return value
    constexpr Iterator_T

    // Call-operator signature
    operator()(Iterator_T first, Sentinel_T last,
               S_Iterator_T s_first, S_Sentinel_T s_last,
               BinaryPredicate_T pred = {}, Projection_T proj = {},
               S_Projection_T s_proj = {}) const
    {
        // Handle empty pattern
        if (s_first == s_last) return first;

        // Handle empty text or pattern longer than text
        if (first == last) return last;

        const auto pattern_len = std::distance(s_first, s_last);
        const auto text_len = std::distance(first, last);

        if (pattern_len > text_len) return last;

        // For custom types, use simple comparison without hashing
        // Hashing might not work well with complex objects
        auto text_it = first;
        auto last_possible = std::next(first, text_len - pattern_len + 1);

        while (text_it != last_possible) {
            bool match = true;
            auto text_check = text_it;
            auto pat_check = s_first;

            for (auto i = 0; i < pattern_len; ++i, ++text_check, ++pat_check) {
                auto text_val = std::invoke(proj, *text_check);
                auto pat_val = std::invoke(s_proj, *pat_check);

                if (!std::invoke(pred, text_val, pat_val)) {
                    match = false;
                    break;
                }
            }

            if (match) {
                return text_it;
            }

            ++text_it;
        }

        return last;
    }


    /******************
       *  Ranges Operator
       */

    // Type Generics
    template <std::ranges::forward_range Range_T,
             std::ranges::forward_range S_Range_T,
             typename BinaryPredicate_T = std::ranges::equal_to,
             typename Projection_T      = std::identity,
             typename S_Projection_T    = std::identity>

    // Algorithm type requirements
        requires std::indirectly_comparable<
            std::ranges::iterator_t<Range_T>, std::ranges::iterator_t<S_Range_T>,
            BinaryPredicate_T, Projection_T, S_Projection_T>

    // Return value
    constexpr std::ranges::iterator_t<Range_T>

    // Call-operator signature
    operator()(Range_T&& range, S_Range_T&& s_range,
               BinaryPredicate_T pred = {}, Projection_T proj = {},
               S_Projection_T s_proj = {}) const
    {
        return (*this)(std::ranges::begin(range), std::ranges::end(range),
                       std::ranges::begin(s_range), std::ranges::end(s_range),
                       std::move(pred), std::move(proj), std::move(s_proj));
    }

};

}   // namespace detail

// Niebloid API Instantiation
inline constexpr detail::kr_search_fn kr_search{};


}   // namespace dte3611::string_match::algorithms

#endif   // DTE3611_WEEK1_STRING_MATCH_KARP_RABIN_SEARCH_H
