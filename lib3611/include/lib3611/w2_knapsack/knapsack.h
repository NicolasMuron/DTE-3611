#ifndef DTE3611_WEEK2_KNAPSACK_H
#define DTE3611_WEEK2_KNAPSACK_H

// stl
#include <vector>
#include <iterator>
#include <ranges>
#include <algorithm>
#include <functional>

namespace dte3611::np::algorithms
{

  namespace detail
  {

    struct knapsack_01_fn {

      /**************************
       *  Iterator Range Operator
       */

      // Type Generics
      template <std::input_iterator           Iterator_T,
                std::sentinel_for<Iterator_T> Sentinel_T,
                std::weakly_incrementable     Output_T,
                typename ValueProjection_T  = std::identity,
                typename WeightProjection_T = std::identity>
      // Return value
      int
      // Call-operator signature
      operator()(Iterator_T  begin , Sentinel_T  end ,
                 Output_T  result , int  capacity ,
                 ValueProjection_T  value_projection ,
                 WeightProjection_T  weight_projection ) const
      {
        const auto n = std::distance(begin, end);

        std::vector<std::vector<int>> M(n+1, std::vector<int>(capacity + 1, 0));

        for (int w = 0; w <= capacity; ++w) {
            M[0][w] = 0;
        }

        int i = 1;
        for (auto it = begin; it != end; ++it, ++i) {
            const int value = std::invoke(value_projection, *it);
            const int weight = std::invoke(weight_projection, *it);
            for (int w = 0; w <= capacity; ++w) {
                if (weight > w) {
                    M[i][w] = M[i-1][w];
                } else {
                    M[i][w] = std::max(M[i-1][w], value + M[i-1][w-weight]);
                }
            }
        }

        std::vector<bool> tmp(n, false);
        int w = capacity;
        for (auto j = n; j > 0; --j) {
            const int weight = std::invoke(weight_projection, *std::next(begin, j-1));

            if (M[j][w] != M[j-1][w]) {
                tmp[j-1] = true;
                w -= weight;
            }
        }

        for (auto it = tmp.rbegin(); it != tmp.rend(); ++it) {
            *result++ = *it;
        }

        return M[n][capacity];
      }

      /******************
       *  Ranges Operator
       */

      // Type Generics
      template <std::ranges::input_range  Range_T,
                std::weakly_incrementable Output_T,
                typename ValueProjection_T  = std::identity,
                typename WeightProjection_T = std::identity>
      // Return value
      auto
      // Call-operator signature
      operator()(Range_T&& range, Output_T result, int capacity,
                 ValueProjection_T  value_projection,
                 WeightProjection_T weight_projection) const
      {
        return (*this)(std::ranges::begin(range), std::ranges::end(range),
                       std::move(result), capacity, std::ref(value_projection),
                       std::ref(weight_projection));
        //      static_assert( false, "Complete the code - find the appropriate call signature in the cpp reference documentation." );
      }

    };

  }   // namespace detail

  inline constexpr detail::knapsack_01_fn knapsack_01{};


}   // namespace dte3611::np::algorithms


#endif   // DTE3611_WEEK2_KNAPSACK_H
