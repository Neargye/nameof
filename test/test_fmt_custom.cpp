// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Daniil Goncharov <neargye@gmail.com>.

// NAMEOF_NO_FMT_FORMAT leaves cstring and name_view to user formatters on fmt 10 and older.
#define NAMEOF_NO_FMT_FORMAT

#if __has_include(<fmt/format.h>)
#  include <fmt/format.h>
#  include <fmt/ranges.h>
#endif

#include <nameof.hpp>

#include <cstdint>
#include <string>
#include <type_traits>
#include <utility>

#include <doctest/doctest.h>

#if defined(NAMEOF_TEST_REQUIRE_FMT) && !defined(FMT_VERSION)
#  error "NAMEOF_TEST_REQUIRE_FMT requires <fmt/format.h>."
#endif

template <typename T, typename = void>
struct has_format_as : std::false_type {};

template <typename T>
struct has_format_as<T, std::void_t<decltype(format_as(std::declval<T>()))>> : std::true_type {};

static_assert(!has_format_as<nameof::name_view>::value);
static_assert(!has_format_as<nameof::cstring<3>>::value);

[[maybe_unused]] static int some_variable = 0;

#if defined(FMT_VERSION)
template <std::uint16_t N>
struct fmt::formatter<nameof::cstring<N>> : fmt::formatter<fmt::string_view> {
  template <typename FormatContext>
  auto format(const nameof::cstring<N>& str, FormatContext& ctx) const {
    return fmt::format_to(ctx.out(), "U:{}", fmt::string_view{str.data(), str.size()});
  }
};

template <>
struct fmt::formatter<nameof::name_view> : fmt::formatter<fmt::string_view> {
  template <typename FormatContext>
  auto format(nameof::name_view str, FormatContext& ctx) const {
    return fmt::format_to(ctx.out(), "U:{}", fmt::string_view{str.data(), str.size()});
  }
};
#endif

TEST_CASE("user fmt formatters") {
#if defined(FMT_VERSION)
  static constexpr nameof::cstring<3> storage{"abc"};
  constexpr nameof::name_view name = storage;
  constexpr auto expression = NAMEOF(some_variable);
#  if FMT_VERSION < 110000
  REQUIRE(fmt::format("{}", name) == "U:abc");
  REQUIRE(fmt::format("{}", expression) == "U:some_variable");
  REQUIRE(fmt::format("{}", NAMEOF(some_variable)) == "U:some_variable");
#  else
  // fmt 11+ formats them as strings itself, found by find_first_of and data(), and ignores the user formatters.
  REQUIRE(fmt::format("{}", name) == "abc");
  REQUIRE(fmt::format("{}", expression) == "some_variable");
#  endif
#endif
}
