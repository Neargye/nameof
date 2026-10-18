// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <nameof.hpp>

#include <limits>

#ifndef NAMEOF_COMPILE_FAIL_CASE
#  error NAMEOF_COMPILE_FAIL_CASE must select a compile-fail scenario.
#endif

#if NAMEOF_COMPILE_FAIL_CASE == 1

constexpr auto invalid_name = NAMEOF(42);

int main() {}

#elif NAMEOF_COMPILE_FAIL_CASE == 2

enum class InvertedRange { value = 0 };

template <>
struct nameof::customize::enum_range<InvertedRange> {
  static constexpr int min = 1;
  static constexpr int max = 0;
};

constexpr auto inverted_range_name = nameof::nameof_enum(InvertedRange::value);

int main() {}

#elif NAMEOF_COMPILE_FAIL_CASE == 3

enum class OversizedRange { value = 0 };

template <>
struct nameof::customize::enum_range<OversizedRange> {
  static constexpr int min = 0;
  static constexpr int max = 65534;
};

constexpr auto oversized_range_name = nameof::nameof_enum(OversizedRange::value);

int main() {}

#elif NAMEOF_COMPILE_FAIL_CASE >= 4 && NAMEOF_COMPILE_FAIL_CASE <= 6

enum class WideRange : long long { value = 0 };

template <>
struct nameof::customize::enum_range<WideRange> {
#if NAMEOF_COMPILE_FAIL_CASE == 5
  static constexpr long long min = (std::numeric_limits<int>::min)() - 1LL;
  static constexpr long long max = 0;
#elif NAMEOF_COMPILE_FAIL_CASE == 6
  static constexpr unsigned long long min = 0;
  static constexpr unsigned long long max = (std::numeric_limits<unsigned long long>::max)();
#else
  static constexpr long long min = 0;
  static constexpr long long max = (std::numeric_limits<int>::max)() + 1LL;
#endif
};

constexpr auto wide_range_name = nameof::nameof_enum(WideRange::value);

int main() {}

#elif NAMEOF_COMPILE_FAIL_CASE == 7

enum class FullIntRange { value = 0 };

template <>
struct nameof::customize::enum_range<FullIntRange> {
  static constexpr int min = (std::numeric_limits<int>::min)();
  static constexpr int max = (std::numeric_limits<int>::max)();
};

constexpr auto full_int_range_name = nameof::nameof_enum(FullIntRange::value);

int main() {}

#elif NAMEOF_COMPILE_FAIL_CASE == 8

struct TooLongName {};

inline constexpr char too_long_name[70000] = {'n'};

template <>
constexpr nameof::string_view nameof::customize::type_name<TooLongName>() noexcept {
  return {too_long_name, sizeof(too_long_name)};
}

constexpr auto& too_long_type_name = nameof::nameof_type<TooLongName>();

int main() {}

#else

#  error Unknown NAMEOF_COMPILE_FAIL_CASE.

#endif
