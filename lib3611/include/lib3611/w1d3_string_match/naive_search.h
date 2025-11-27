#ifndef DTE3611_WEEK1_STRING_MATCH_NAIVE_SEARCH_H
#define DTE3611_WEEK1_STRING_MATCH_NAIVE_SEARCH_H

// stl
#include <iterator>
#include <algorithm>

namespace dte3611::string_match::algorithms
{

  namespace detail
  {

    struct naive_search_fn {

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

          if (first == last) return last;

          if (std::distance(s_first, s_last) > std::distance(first, last))
              return last;

          Iterator_T it{first};
          Iterator_T s_it{s_first};

          for (; it != last;) {

              for(;;) {

                  if (s_it == s_last) return it;

                  auto const s_offset = std::distance(s_first, s_it);
                  Iterator_T it_curr{it + s_offset};

                  if (it_curr == last) return last;

                  if (not pred(std::invoke(proj, *it_curr), std::invoke(s_proj, *s_it))) {
                      it++;
                      s_it = s_first;
                      break;
                  }
                  else {
                      s_it++;
                  }
              }
          }

        return last;
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
  inline constexpr detail::naive_search_fn naive_search{};

}   // namespace dte3611::string_match::algorithms

#endif   // DTE3611_WEEK1_STRING_MATCH_NAIVE_SEARCH_H
