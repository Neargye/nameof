// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace ide {

enum class Range { Last = 1023, First = -2, BeforeBlock = 509, AfterBlock = 510, Alias = 510, Renamed = 8, Below = -11, Above = 1032 };
enum Unfixed { Zero, One };
enum class Binary : bool { False, True };
enum class SignedFlags : std::int8_t { None = 0, A = 1, B = 2, AB = 3, High = -128 };
enum class WideFlags : std::uint64_t { None = 0, A = 1, High = std::uint64_t{1} << 63 };

template <typename E>
struct data;

template <>
struct data<Range> {
  inline static constexpr std::array values{Range::Last, Range::First, Range::BeforeBlock, Range::AfterBlock, Range::Alias, Range::Renamed, Range::Below, Range::Above};
  inline static constexpr std::array names{"ide::Range::Last", "ide::Range::First", "ide::Range::BeforeBlock", "ide::Range::FirstAlias", "ide::Range::SecondAlias", "ide::Range::Renamed", "ide::Range::Below", "ide::Range::Above"};
};

template <>
struct data<Unfixed> {
  inline static constexpr std::array values{One, Zero};
  inline static constexpr std::array names{"ide::One", "ide::Zero"};
};

template <>
struct data<Binary> {
  inline static constexpr std::array values{Binary::True, Binary::False};
  inline static constexpr std::array names{"True", "False"};
};

template <>
struct data<SignedFlags> {
  inline static constexpr std::array values{SignedFlags::High, SignedFlags::AB, SignedFlags::B, SignedFlags::A, SignedFlags::None};
  inline static constexpr std::array names{"High", "AB", "B", "A", "None"};
};

template <>
struct data<WideFlags> {
  inline static constexpr std::array values{WideFlags::High, WideFlags::A, WideFlags::None};
  inline static constexpr std::array names{"High", "A", "None"};
};

template <typename E>
constexpr const char* enumerator_name(E value) {
  using U = std::underlying_type_t<E>;
  for (std::size_t i = 0; i < data<E>::values.size(); ++i) {
    if (static_cast<U>(data<E>::values[i]) == static_cast<U>(value)) {
      return data<E>::names[i];
    }
  }
  return nullptr;
}

} // namespace ide

// The stubs test the adapter, not the intrinsic itself.
#define __rscpp_enumerator_name ide::enumerator_name
#define __RESHARPER__ 20250300
#if !defined(NAMEOF_FORCE_COMPILER_SPECIFIC_REFLECTION)
#  define NAMEOF_FORCE_COMPILER_SPECIFIC_REFLECTION
#endif
#include <nameof.hpp>

#undef __RESHARPER__
#undef __rscpp_enumerator_name

// Three scan blocks: 509 is the last value of the first block, 510 the first of the second.
template <>
struct nameof::customize::enum_range<ide::Range> {
  static constexpr int min = -2;
  static constexpr int max = 1023;
};

template <>
constexpr nameof::string_view nameof::customize::enum_name<ide::Range>(ide::Range value) noexcept {
  if (value == ide::Range::Renamed || value == static_cast<ide::Range>(9)) {
    return "custom";
  }
  return {};
}

using nameof::nameof_enum;

static_assert(nameof_enum(ide::Range::First) == "First");
static_assert(nameof_enum(ide::Range::BeforeBlock) == "BeforeBlock");
static_assert(nameof_enum(ide::Range::AfterBlock) == "FirstAlias");
static_assert(nameof_enum(ide::Range::Last) == "Last");
static_assert(nameof_enum<ide::Range::Alias>() == "FirstAlias");
static_assert(nameof_enum(static_cast<ide::Range>(6)).empty());
static_assert(nameof_enum(ide::Range::Renamed) == "custom");
static_assert(nameof_enum(static_cast<ide::Range>(9)) == "custom");
static_assert(nameof_enum(ide::Range::Below).empty());
static_assert(nameof_enum(ide::Range::Above).empty());
static_assert(nameof_enum<ide::Range::Above>() == "Above");
static_assert(nameof_enum(ide::Zero) == "Zero");
static_assert(nameof_enum(ide::One) == "One");
static_assert(nameof_enum(ide::Binary::False) == "False");
static_assert(nameof_enum(ide::Binary::True) == "True");
static_assert(nameof_enum(ide::SignedFlags::High) == "High");
static_assert(nameof_enum(ide::WideFlags::A) == "A");
static_assert(nameof_enum(ide::WideFlags::High).empty());

int main() {
  int failures = 0;
  failures += nameof::nameof_enum_or(static_cast<ide::Range>(6), "none") != "none";
  failures += nameof::nameof_enum_flag(ide::SignedFlags::AB) != "A|B";
  failures += nameof::nameof_enum_flag(static_cast<ide::SignedFlags>(-127)) != "A|High";
  failures += nameof::nameof_enum_flag(static_cast<ide::WideFlags>(0x8000000000000001ULL)) != "A|High";
  failures += !nameof::nameof_enum_flag(static_cast<ide::WideFlags>(2)).empty();
  return failures;
}
