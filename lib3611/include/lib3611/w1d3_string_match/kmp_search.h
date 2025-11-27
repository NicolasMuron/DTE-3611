#ifndef DTE3611_WEEK1_STRING_MATCH_KNUTH_MORRIS_PRATT_SEARCH_H
#define DTE3611_WEEK1_STRING_MATCH_KNUTH_MORRIS_PRATT_SEARCH_H

// stl
#include <iterator>
#include <algorithm>

namespace dte3611::string_match::algorithms
{
  namespace detail
  {

    struct kmp_search_fn {

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
          if (s_first == s_last) return first;

          // Convert sentinels to iterators
          Iterator_T text_end = first;
          while (text_end != last) ++text_end;

          S_Iterator_T pattern_end = s_first;
          while (pattern_end != s_last) ++pattern_end;

          int n = 0;
          for (auto it = first; it != text_end; ++it) ++n;
          int m = 0;
          for (auto it = s_first; it != pattern_end; ++it) ++m;
          if (m == 0) return first;
          if (n < m) return text_end;

          // Build LPS array
          std::vector<int> lps(m, 0);
          for (int i = 1, len = 0; i < m; ) {
              if (std::invoke(pred, std::invoke(s_proj, s_first[i]),
                              std::invoke(s_proj, s_first[len]))) {
                  lps[i++] = ++len;
              } else {
                  if (len != 0) len = lps[len - 1];
                  else lps[i++] = 0;
              }
          }

          // KMP search
          for (int i = 0, j = 0; i < n; ) {
              if (std::invoke(pred, std::invoke(proj, first[i]),
                              std::invoke(s_proj, s_first[j]))) {
                  i++;
                  j++;
              }

              if (j == m) return first + (i - j);
              else if (i < n && !std::invoke(pred, std::invoke(proj, first[i]),
                                             std::invoke(s_proj, s_first[j]))) {
                  if (j != 0) j = lps[j - 1];
                  else i++;
              }
          }

          return text_end;
        //      static_assert( false, "Complete the code" );
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
//        static_assert(false, "Complete the code"
//                             "- find the appropriate call signature in the "
//                             "cpp reference documentation.");
      }

    };

  }   // namespace detail

  // Niebloid API Instantiation
  inline constexpr detail::kmp_search_fn kmp_search{};

}   // namespace dte3611::string_match::algorithms

#endif   // DTE3611_WEEK1_STRING_MATCH_KNUTH_MORRIS_PRATT_SEARCH_H
