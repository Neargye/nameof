// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#define NAMEOF_ENUM_RANGE_MIN -32769
#define NAMEOF_ENUM_RANGE_MAX -32768
#include <nameof.hpp>
#include <limits>

enum class NegativeOnlyRange { First = -32769, Last = -32768 };

static_assert(nameof::nameof_enum(NegativeOnlyRange::First) == "First");
static_assert(nameof::nameof_enum(NegativeOnlyRange::Last) == "Last");

enum class IntMinRange { First = (std::numeric_limits<int>::min)(), Middle, Last };

template <>
struct nameof::customize::enum_range<IntMinRange> {
  static constexpr int min = (std::numeric_limits<int>::min)();
  static constexpr int max = (std::numeric_limits<int>::min)() + 2;
};

static_assert(nameof::nameof_enum(IntMinRange::First) == "First");
static_assert(nameof::nameof_enum(IntMinRange::Middle) == "Middle");
static_assert(nameof::nameof_enum(IntMinRange::Last) == "Last");
static_assert(nameof::nameof_enum(static_cast<IntMinRange>((std::numeric_limits<int>::min)() + 3)).empty());

int main() {}
