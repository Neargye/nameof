// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Daniil Goncharov <neargye@gmail.com>.

// nameof.hpp comes before fmt here on purpose, the other suites include fmt first.
#include <nameof.hpp>

#include <string>

#include <doctest/doctest.h>

#if __has_include(<fmt/format.h>)
#  include <fmt/format.h>
#  include <fmt/ranges.h>
#endif

#if defined(NAMEOF_TEST_REQUIRE_FMT) && !defined(FMT_VERSION)
#  error "NAMEOF_TEST_REQUIRE_FMT requires <fmt/format.h>."
#endif

[[maybe_unused]] static int some_variable = 0;

TEST_CASE("fmt included after nameof") {
#if defined(FMT_VERSION) && FMT_VERSION >= 100100
  // name_view and cstring format through format_as, not as ranges.
  static constexpr nameof::cstring<3> storage{"abc"};
  constexpr nameof::name_view name = storage;
  REQUIRE(fmt::format("{}", name) == "abc");
  REQUIRE(fmt::format("{:>5}", name) == "  abc");
  REQUIRE(fmt::format("{:.2}", name) == "ab");
  REQUIRE(fmt::format("{}", nameof::name_view{}).empty());
  constexpr auto expression = NAMEOF(some_variable);
  REQUIRE(fmt::format("{}", expression) == "some_variable");
  REQUIRE(fmt::format("{:>15}", expression) == "  some_variable");
  REQUIRE(fmt::format("{:.4}", expression) == "some");
  REQUIRE(fmt::format("{}", NAMEOF(some_variable)) == "some_variable");
  REQUIRE(fmt::format("{}", nameof::cstring<0>{}).empty());
#endif
}
