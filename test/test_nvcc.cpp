// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <nameof.hpp>

#include <cstdint>
#include <type_traits>

static_assert(nameof::is_nameof_enum_supported);
static_assert(nameof::is_nameof_type_supported);

namespace nvcc {

template <int>
struct TemplateScope {
  enum class Value { One = 1, Four = 4 };
  enum class Collision { nameof_detail_enum_definition = 1 };
};

// Each first lookup uses a fresh enum.
static_assert(nameof::nameof_enum<static_cast<TemplateScope<0>::Value>(4)>() == "Four");
static_assert(nameof::nameof_enum(static_cast<TemplateScope<1>::Value>(1)) == "One");
static_assert(nameof::nameof_enum(static_cast<TemplateScope<2>::Collision>(1)) == "nameof_detail_enum_definition");

// nvcc reuses the spelling of a value's first instantiation, so these enums are first used through an alias.
enum class AliasFirst { Negative = -7, One = 1, Four = 4 };
using Alias = AliasFirst;
static_assert(nameof::nameof_enum<const Alias&>(Alias::Four) == "Four");
static_assert(nameof::nameof_enum(AliasFirst::Negative) == "Negative");
static_assert(nameof::nameof_enum(static_cast<AliasFirst>(2)).empty());

template <typename T>
struct identity {
  using type = T;
};

enum class ConstFirst { One = 1, Four = 4 };
static_assert(nameof::nameof_enum<identity<ConstFirst>::type::Four>() == "Four");
static_assert(NAMEOF_ENUM_CONST(ConstFirst::One) == "One");
static_assert(nameof::nameof_enum(ConstFirst::Four) == "Four");

enum Plain { PlainFirst = 1, PlainLast = 4 };
static_assert(nameof::nameof_enum(PlainLast) == "PlainLast");

enum class Narrow : bool { Off, On };
static_assert(nameof::nameof_enum(Narrow::On) == "On");

enum class Small : std::int8_t { Min = -128, Zero = 0, Max = 127 };
static_assert(nameof::nameof_enum(Small::Min) == "Min");
static_assert(nameof::nameof_enum(Small::Max) == "Max");

struct Holder {
  enum class Member { A, B };
};
static_assert(nameof::nameof_enum(Holder::Member::B) == "B");

enum class Ranged { First = 300, Last = 400 };
enum class Renamed { A, B };
enum class Bits : std::uint64_t { Low = 1, Mid = 1 << 4, High = std::uint64_t{1} << 63 };
enum class SignedBits : std::int8_t { Low = 1, High = -128 };

namespace a_namespace_with_a_long_name_so_that_every_scanned_signature_is_long_enough {
enum class LongScan { First = -128, Middle = 0, Last = 127 };
} // namespace a_namespace_with_a_long_name_so_that_every_scanned_signature_is_long_enough

struct Type {
  int field = 0;
  void method() {}
};

int global = 0;
void function() {}

} // namespace nvcc

template <>
struct nameof::customize::enum_range<nvcc::Ranged> {
  static constexpr int min = 300;
  static constexpr int max = 400;
};

template <>
constexpr nameof::string_view nameof::customize::enum_name<nvcc::Renamed>(nvcc::Renamed value) noexcept {
  return value == nvcc::Renamed::B ? "Bee" : "";
}

using namespace nvcc;

static_assert(nameof::nameof_enum(Ranged::Last) == "Last");
static_assert(nameof::nameof_enum(Renamed::A) == "A");
static_assert(nameof::nameof_enum(Renamed::B) == "Bee");
static_assert(nameof::nameof_enum(a_namespace_with_a_long_name_so_that_every_scanned_signature_is_long_enough::LongScan::Last) == "Last");

#if defined(_MSC_VER)
constexpr nameof::string_view type_name = "struct nvcc::Type";
static_assert(nameof::nameof_type<Holder::Member>() == "enum nvcc::Holder::Member");
#else
constexpr nameof::string_view type_name = "nvcc::Type";
static_assert(nameof::nameof_type<Holder::Member>() == "nvcc::Holder::Member");
#endif
static_assert(nameof::nameof_type<Type>() == type_name);
static_assert(nameof::nameof_short_type<const Type&>() == "Type");
static_assert(nameof::nameof_member<&Type::field>() == "field");
static_assert(nameof::nameof_member<&Type::method>() == "method");
static_assert(nameof::nameof_member<&Type::method>().substr(1, 3) == "eth");
static_assert(nameof::nameof_member<&Type::method>().find("th") == 2 && nameof::nameof_member<&Type::method>().rfind('d') == 5);
static_assert(nameof::nameof_member<&Type::method>().starts_with("me") && nameof::nameof_member<&Type::method>().ends_with("od"));
static_assert(NAMEOF(global).find("ob") == 2 && NAMEOF(global).rfind('l') == 5 && NAMEOF(global).starts_with('g') && NAMEOF(global).ends_with("al"));
static_assert(nameof::nameof_pointer<&global>() == "global");
static_assert(nameof::nameof_pointer<&function>() == "function");
static_assert(NAMEOF(global) == "global");

int main() {
  int failures = 0;
  // Run-time results come from the host compiler under nvcc.
  failures += nameof::nameof_enum(AliasFirst::One) != "One";
  failures += nameof::nameof_enum(Ranged::First) != "First";
  failures += nameof::nameof_enum_or(static_cast<Ranged>(301), "none") != "none";
  failures += nameof::nameof_enum_flag(static_cast<Bits>(17)) != "Low|Mid";
  failures += nameof::nameof_enum_flag(static_cast<Bits>((std::uint64_t{1} << 63) | 1)) != "Low|High";
  failures += nameof::nameof_enum_flag(static_cast<SignedBits>(-127)) != "Low|High";
  failures += nameof::nameof_type<Type>() != type_name;
  failures += nameof::nameof_short_type<Holder::Member>() != "Member";
  failures += nameof::nameof_member<&Type::field>() != "field";
  failures += nameof::nameof_pointer<&function>() != "function";
  return failures;
}
