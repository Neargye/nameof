// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#define NAMEOF_ENUM_RANGE_MIN 32767
#define NAMEOF_ENUM_RANGE_MAX 33023
#include <nameof.hpp>
#include <limits>

enum class PositiveOnlyRange { First = 32767, Last = 33023 };

static_assert(nameof::nameof_enum(PositiveOnlyRange::First) == "First");
static_assert(nameof::nameof_enum(PositiveOnlyRange::Last) == "Last");

enum class IntMaxRange { First = (std::numeric_limits<int>::max)() - 2, Middle, Last };

template <>
struct nameof::customize::enum_range<IntMaxRange> {
  static constexpr int min = (std::numeric_limits<int>::max)() - 2;
  static constexpr int max = (std::numeric_limits<int>::max)();
};

static_assert(nameof::nameof_enum(IntMaxRange::First) == "First");
static_assert(nameof::nameof_enum(IntMaxRange::Middle) == "Middle");
static_assert(nameof::nameof_enum(IntMaxRange::Last) == "Last");
static_assert(nameof::nameof_enum(static_cast<IntMaxRange>((std::numeric_limits<int>::max)() - 3)).empty());

int main() {}
