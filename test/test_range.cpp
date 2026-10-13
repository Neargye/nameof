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
  static constexpr unsigned long long min = (std::numeric_limits<int>::max)() - 2;
  static constexpr unsigned long long max = (std::numeric_limits<int>::max)();
};

static_assert(nameof::nameof_enum(IntMaxRange::First) == "First");
static_assert(nameof::nameof_enum(IntMaxRange::Middle) == "Middle");
static_assert(nameof::nameof_enum(IntMaxRange::Last) == "Last");
static_assert(nameof::nameof_enum(static_cast<IntMaxRange>((std::numeric_limits<int>::max)() - 3)).empty());

enum class ClampedWideRange : unsigned char { First = 0, Last = (std::numeric_limits<unsigned char>::max)() };

template <>
struct nameof::customize::enum_range<ClampedWideRange> {
  static constexpr long long min = (std::numeric_limits<long long>::min)();
  static constexpr unsigned long long max = (std::numeric_limits<unsigned long long>::max)();
};

static_assert(nameof::nameof_enum(ClampedWideRange::First) == "First");
static_assert(nameof::nameof_enum(ClampedWideRange::Last) == "Last");
static_assert(nameof::nameof_enum(static_cast<ClampedWideRange>(1)).empty());

enum class ClampedBoolRange : bool { Off = false, On = true };

template <>
struct nameof::customize::enum_range<ClampedBoolRange> {
  static constexpr long long min = (std::numeric_limits<long long>::min)();
  static constexpr unsigned long long max = (std::numeric_limits<unsigned long long>::max)();
};

static_assert(nameof::nameof_enum(ClampedBoolRange::Off) == "Off");
static_assert(nameof::nameof_enum(ClampedBoolRange::On) == "On");

int main() {}
