#ifndef DTE3611_WEEK1_RADIX_SORT_H
#define DTE3611_WEEK1_RADIX_SORT_H

// stl
#include <iterator>
#include <ranges>
#include <algorithm>
#include <vector>
#include <cmath>
#include <type_traits>
#include <functional>

namespace dte3611::sort::algorithms
{
namespace detail
{

struct radix_sort_fn {

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

        using ValueType = typename std::iterator_traits<Iterator_T>::value_type;
        using ProjType = std::remove_cvref_t<decltype(std::invoke(proj, *first))>;

        // Check if values are integers (radix sort requires integer-like types)
        if constexpr (!std::is_integral_v<ProjType> &&
                      !std::is_same_v<ProjType, char> &&
                      !std::is_same_v<ProjType, unsigned char> &&
                      !std::is_same_v<ProjType, signed char>) {
            // Fallback to std::sort for non-integer types
            std::sort(first, last, comp);
            return last;
        }
        else {
            const size_t n = std::distance(first, last);
            std::vector<ValueType> output(n);

            // Find maximum absolute value to determine number of digits
            ProjType max_val = 0;
            for (auto it = first; it != last; ++it) {
                ProjType current = std::invoke(proj, *it);
                ProjType abs_current = current;
                if constexpr (std::is_signed_v<ProjType>) {
                    if (current < 0) abs_current = -current;
                }
                if (abs_current > max_val) max_val = abs_current;
            }

            if (max_val == 0) return last; // All zeros

            // Calculate number of digits
            int num_digits = 0;
            ProjType temp = max_val;
            while (temp > 0) {
                temp /= 10;
                num_digits++;
            }
            if (num_digits == 0) num_digits = 1;

            // For signed types, we need an extra "digit" for the sign
            constexpr bool is_signed = std::is_signed_v<ProjType>;
            const int total_passes = is_signed ? num_digits + 1 : num_digits;

            // Perform counting sort for each digit
            for (int pass = 0; pass < total_passes; ++pass) {
                std::vector<int> count(10, 0);
                long exp = (pass == 0 && is_signed) ? 0 : static_cast<long>(std::pow(10, pass - (is_signed ? 1 : 0)));

                // Count occurrences of each digit
                for (auto it = first; it != last; ++it) {
                    ProjType value = std::invoke(proj, *it);
                    int digit_val;

                    if constexpr (is_signed) {
                        if (pass == 0) {
                            // First pass: handle sign (0 for positive, 9 for negative)
                            digit_val = (value >= 0) ? 0 : 9;
                        } else {
                            // Regular digit passes
                            long long_value = static_cast<long>(value);
                            digit_val = (std::abs(long_value) / exp) % 10;
                        }
                    } else {
                        // Unsigned types
                        long long_value = static_cast<long>(value);
                        digit_val = (long_value / exp) % 10;
                    }

                    count[digit_val]++;
                }

                // Calculate cumulative counts - for stable sort we need to build from the end
                for (int i = 1; i < 10; ++i) {
                    count[i] += count[i - 1];
                }

                // Build output array (stable sort) - go backwards
                for (auto it = last; it != first; ) {
                    --it;
                    ProjType value = std::invoke(proj, *it);
                    int digit_val;

                    if constexpr (is_signed) {
                        if (pass == 0) {
                            digit_val = (value >= 0) ? 0 : 9;
                        } else {
                            long long_value = static_cast<long>(value);
                            digit_val = (std::abs(long_value) / exp) % 10;
                        }
                    } else {
                        long long_value = static_cast<long>(value);
                        digit_val = (long_value / exp) % 10;
                    }

                    output[count[digit_val] - 1] = *it;
                    count[digit_val]--;
                }

                // Copy output back to input for next iteration
                std::copy(output.begin(), output.end(), first);
            }

            // Handle the comparison function for ordering
            // Radix sort always produces ascending order for the projected values
            // So if we want descending order, we reverse the result
            bool should_reverse = false;

            // Test the comparator by checking what order it expects
            if (n >= 2) {
                // Create test values that are different
                ProjType test_small = 1;
                ProjType test_large = 2;

                // If comp says large < small, then we want descending order
                if (comp(test_large, test_small)) {
                    should_reverse = true;
                }
            }

            if (should_reverse) {
                std::reverse(first, last);
            }

            return last;
        }
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
inline constexpr detail::radix_sort_fn radix_sort{};

}   // namespace dte3611::sort::algorithms

#endif   // DTE3611_WEEK1_RADIX_SORT_H
