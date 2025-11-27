#ifndef DTE3611_WEEK1_COUNTING_SORT_H
#define DTE3611_WEEK1_COUNTING_SORT_H

// stl
#include <iterator>
#include <algorithm>

namespace dte3611::sort::algorithms
{
  namespace detail
  {

    struct counting_sort_fn {

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
        if (first == last) return first;

        // Find min and max elements
        auto min_max = std::minmax_element(first, last, comp);
        auto min_val = std::invoke(proj, *min_max.first);
        auto max_val = std::invoke(proj, *min_max.second);

        // Handle case where all elements are equal
        if (min_val == max_val) return last;

        // Create count array
        auto range = max_val - min_val + 1;
        std::vector<size_t> count(range, 0);

        // Count occurrences
        for (auto it = first; it != last; ++it) {
            auto value = std::invoke(proj, *it);
            count[value - min_val]++;
        }

        // Calculate cumulative counts
        for (size_t i = 1; i < count.size(); ++i) {
            count[i] += count[i - 1];
        }

        // Create output array
        using ValueType = typename std::iterator_traits<Iterator_T>::value_type;
        std::vector<ValueType> output(std::distance(first, last));

        // Build output array
        for (auto it = first; it != last; ++it) {
            auto value = std::invoke(proj, *it);
            output[count[value - min_val] - 1] = *it;
            count[value - min_val]--;
        }

        // Copy output back to original range
        std::copy(output.begin(), output.end(), first);

        return last;
        //        static_assert(false, "Complete the code");
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
//        static_assert(false, "Complete the code"
//                             "- find the appropriate call signature in the "
//                             "cpp reference documentation.");
      }
    };

  }   // namespace detail

  // Niebloid API Instantiation
  inline constexpr detail::counting_sort_fn counting_sort{};


}   // namespace dte3611::sort::algorithms

#endif   // DTE3611_WEEK1_COUNTING_SORT_H
