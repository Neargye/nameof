// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <nameof.hpp>

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

#else

#  error Unknown NAMEOF_COMPILE_FAIL_CASE.

#endif
