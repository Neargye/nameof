// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <doctest/doctest.h>

#include <nameof.hpp>

#include <cstdint>
#include <limits>

enum class Dense { A = -2, B = -1, C = 0, D = 1, E = 2 };
enum class Sparse : long long { FarLow = -1'000'000, InRange = 17, FarHigh = 900'000'000 };
enum class Unordered { C = 30, A = 10, B = 20 };
enum class Aliased { First = 1, AliasFirst = 1, Second = 2, AliasSecond = 2 };
enum class Customized { Declared = 1 };
enum class SyntheticOnly {};
enum class CustomizedFlags : unsigned { A = 1, B = 2, AB = 3 };
enum class SyntheticOnlyFlags : unsigned { Composite = 3 };
enum class CompositeOnlyFlags : unsigned { Composite = 3 };
enum class WideFlags : std::uint64_t { None = 0, A = 1, B = 2, AB = 3, High = std::uint64_t{1} << 63 };
enum class BoolFlags : bool { Disabled = false, Enabled = true };
enum class ByteFlags : std::uint8_t { Low = 1, High = 128 };
enum class SignedByteFlags : std::int8_t { Low = 1, High = -128 };
template <typename T>
struct SignedFlags {
  enum class Type : T { Low = 1, High = (std::numeric_limits<T>::min)() };
};
enum class Extreme : long long {
  Min = (std::numeric_limits<long long>::min)(),
  Max = (std::numeric_limits<long long>::max)(),
};
enum class UnicodeIdentifiers { \u65E5\u672C\u8A9E = 1 };

namespace adl_comparison {

enum class DeletedEquality { First = 1, AliasFirst = 1, Second = 4 };

bool operator==(DeletedEquality, DeletedEquality) = delete;
bool operator!=(DeletedEquality, DeletedEquality) = delete;

enum class NonConstexprEquality { First = 1, Second = 4 };

inline bool operator==(NonConstexprEquality, NonConstexprEquality) noexcept {
  return false;
}

} // namespace adl_comparison

template <>
struct nameof::customize::enum_range<Sparse> {
  static constexpr int min = 17;
  static constexpr int max = 17;
};

template <>
constexpr nameof::string_view nameof::customize::enum_name<Customized>(Customized value) noexcept {
  if (value == Customized::Declared) {
    return "Renamed";
  }
  if (static_cast<int>(value) == 2) {
    return "Synthetic";
  }
  return {};
}

template <>
constexpr nameof::string_view nameof::customize::enum_name<SyntheticOnly>(SyntheticOnly value) noexcept {
  if (value == SyntheticOnly{900'000'000}) {
    return "Far";
  }
  return value == SyntheticOnly{42} ? nameof::string_view{"Only"} : nameof::string_view{};
}

template <>
constexpr nameof::string_view nameof::customize::enum_name<CustomizedFlags>(CustomizedFlags value) noexcept {
  if (value == CustomizedFlags::A) {
    return "RenamedA";
  }
  if (value == static_cast<CustomizedFlags>(4)) {
    return "SyntheticFlag";
  }
  return {};
}

template <>
constexpr nameof::string_view nameof::customize::enum_name<SyntheticOnlyFlags>(SyntheticOnlyFlags value) noexcept {
  if (value == static_cast<SyntheticOnlyFlags>(1)) {
    return "SyntheticA";
  }
  if (value == static_cast<SyntheticOnlyFlags>(2)) {
    return "SyntheticB";
  }
  return {};
}

TEST_CASE("runtime enum backend") {
#if defined(NAMEOF_TEST_STD_REFLECTION)
  static_assert(nameof::detail::reflection::enumerators_v<Dense>.size() == 5);
#else
  static_assert(nameof::detail::values_v<Dense>.size() == 5);
  static_assert(nameof::detail::values_v<Dense>[0] == Dense::A);
  static_assert(nameof::detail::values_v<Dense>[1] == Dense::B);
  static_assert(nameof::detail::values_v<Dense>[2] == Dense::C);
  static_assert(nameof::detail::values_v<Dense>[3] == Dense::D);
  static_assert(nameof::detail::values_v<Dense>[4] == Dense::E);
#endif
  static_assert(nameof::nameof_enum(Unordered::A) == "A");
  static_assert(nameof::nameof_enum(Unordered::B) == "B");
  static_assert(nameof::nameof_enum(Unordered::C) == "C");
  static_assert(nameof::nameof_enum(Aliased::AliasFirst) == "First");
  static_assert(nameof::nameof_enum(Aliased::AliasSecond) == "Second");
  static_assert(nameof::nameof_enum(adl_comparison::DeletedEquality::AliasFirst) == "First");
  static_assert(nameof::nameof_enum<adl_comparison::DeletedEquality::AliasFirst>() == "First");
  static_assert(nameof::nameof_enum(adl_comparison::NonConstexprEquality::Second) == "Second");
  static_assert(nameof::nameof_enum<adl_comparison::NonConstexprEquality::Second>() == "Second");
  static_assert(nameof::nameof_enum(Customized::Declared) == "Renamed");
  static_assert(nameof::nameof_enum(Customized{2}) == "Synthetic");
  static_assert(nameof::nameof_enum<Customized{2}>() == "Synthetic");
  static_assert(nameof::nameof_enum(SyntheticOnly{42}) == "Only");
  static_assert(nameof::nameof_enum<SyntheticOnly{42}>() == "Only");
  static_assert(nameof::nameof_enum(SyntheticOnly{900'000'000}) == "Far");
  static_assert(nameof::nameof_enum<SyntheticOnly{900'000'000}>() == "Far");
  static_assert(nameof::nameof_enum(SyntheticOnly{43}).empty());
  CHECK(nameof::nameof_enum_or(SyntheticOnly{42}, "fallback") == "Only");
  CHECK(nameof::nameof_enum_or(SyntheticOnly{900'000'000}, "fallback") == "Far");
  CHECK(nameof::nameof_enum_or(SyntheticOnly{43}, "fallback") == "fallback");

#if defined(NAMEOF_TEST_STD_REFLECTION)
  static_assert(nameof::detail::reflection::enumerators_v<Sparse>.size() == 3);
  static_assert(nameof::nameof_enum(Sparse::FarLow) == "FarLow");
  static_assert(nameof::nameof_enum(Sparse::FarHigh) == "FarHigh");
  static_assert(nameof::nameof_enum(Extreme::Min) == "Min");
  static_assert(nameof::nameof_enum(Extreme::Max) == "Max");
  constexpr auto unicode_runtime_name = nameof::nameof_enum(UnicodeIdentifiers::\u65E5\u672C\u8A9E);
  constexpr auto& unicode_static_name = nameof::nameof_enum<UnicodeIdentifiers::\u65E5\u672C\u8A9E>();
  static_assert(unicode_runtime_name == "\u65E5\u672C\u8A9E");
  static_assert(unicode_static_name == "\u65E5\u672C\u8A9E");
  static_assert(unicode_runtime_name.data()[unicode_runtime_name.size()] == '\0');
  static_assert(unicode_static_name.data()[unicode_static_name.size()] == '\0');
#else
  static_assert(nameof::detail::values_v<Sparse>.size() == 1);
  static_assert(nameof::detail::values_v<Sparse>[0] == Sparse::InRange);
  static_assert(nameof::nameof_enum(Sparse::FarLow).empty());
  static_assert(nameof::nameof_enum(Sparse::FarHigh).empty());
#endif
}

TEST_CASE("flag backend") {
#if !defined(NAMEOF_TEST_STD_REFLECTION)
  static_assert(nameof::detail::values_v<WideFlags, true>.size() == 3);
  static_assert(nameof::detail::values_v<WideFlags, true>[0] == WideFlags::A);
  static_assert(nameof::detail::values_v<WideFlags, true>[1] == WideFlags::B);
  static_assert(nameof::detail::values_v<WideFlags, true>[2] == WideFlags::High);
#endif
  CHECK(nameof::nameof_enum_flag(WideFlags::AB) == "A|B");
  CHECK(nameof::nameof_enum_flag(WideFlags::High) == "High");
  CHECK(nameof::nameof_enum_flag(static_cast<WideFlags>(static_cast<std::uint64_t>(WideFlags::A) |
                                                       static_cast<std::uint64_t>(WideFlags::High))) == "A|High");
  CHECK(nameof::nameof_enum_flag(static_cast<WideFlags>(std::uint64_t{4})).empty());
  CHECK(nameof::nameof_enum_flag(WideFlags::None).empty());
  CHECK(nameof::nameof_enum_flag(CustomizedFlags::AB) == "RenamedA|B");
  CHECK(nameof::nameof_enum_flag(static_cast<CustomizedFlags>(4)) == "SyntheticFlag");
  CHECK(nameof::nameof_enum_flag(SyntheticOnlyFlags::Composite) == "SyntheticA|SyntheticB");
  CHECK(nameof::nameof_enum_flag(CompositeOnlyFlags::Composite).empty());
  CHECK(nameof::nameof_enum_flag(BoolFlags::Enabled) == "Enabled");
  CHECK(nameof::nameof_enum_flag(BoolFlags::Disabled).empty());
}

TEST_CASE("enum lookup across named values and gaps") {
  for (int raw = -128; raw <= 127; ++raw) {
    CAPTURE(raw);
    nameof::string_view expected;
    switch (raw) {
      case -2: expected = "A"; break;
      case -1: expected = "B"; break;
      case 0: expected = "C"; break;
      case 1: expected = "D"; break;
      case 2: expected = "E"; break;
      default: break;
    }
    CHECK(nameof::nameof_enum(static_cast<Dense>(raw)) == expected);
    CHECK(nameof::nameof_enum_or(static_cast<Dense>(raw), "fallback") == (expected.empty() ? "fallback" : expected));

    nameof::string_view sparse_expected;
    switch (raw) {
      case 10: sparse_expected = "A"; break;
      case 20: sparse_expected = "B"; break;
      case 30: sparse_expected = "C"; break;
      default: break;
    }
    CHECK(nameof::nameof_enum(static_cast<Unordered>(raw)) == sparse_expected);
  }
}

TEST_CASE("all byte flag combinations") {
  for (int raw = 0; raw < 256; ++raw) {
    CAPTURE(raw);
    nameof::string_view expected;
    switch (raw) {
      case 1: expected = "Low"; break;
      case 128: expected = "High"; break;
      case 129: expected = "Low|High"; break;
      default: break;
    }
    CHECK(nameof::nameof_enum_flag(static_cast<ByteFlags>(raw)) == expected);
    const int signed_raw = raw < 128 ? raw : raw - 256;
    CHECK(nameof::nameof_enum_flag(static_cast<SignedByteFlags>(signed_raw)) == expected);
  }
}

TEST_CASE_TEMPLATE("signed flag sign bit", T, std::int8_t, std::int16_t, std::int32_t, std::int64_t) {
  using E = typename SignedFlags<T>::Type;
  using U = std::make_unsigned_t<T>;
  CHECK(nameof::nameof_enum_flag(E::High) == "High");
  CHECK(nameof::nameof_enum_flag(static_cast<E>(static_cast<U>(E::High) | U{1})) == "Low|High");
  CHECK(nameof::nameof_enum_flag(static_cast<E>(static_cast<U>(E::High) | U{2})).empty());
}
