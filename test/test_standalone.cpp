// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <nameof.hpp>

#if defined(NAMEOF_ARRAY_CONSTEXPR) || defined(NAMEOF_FOR_EACH_256) || defined(NAMEOF_DETAIL_CDECL) || defined(NAMEOF_DETAIL_SIGNATURE) || defined(NAMEOF_DETAIL_USE_STD_REFLECTION)
#  error "nameof internal implementation macro leaked from the public header."
#endif

#if defined(NAMEOF_TEST_VERSION_MAJOR)
static_assert(NAMEOF_VERSION_MAJOR == NAMEOF_TEST_VERSION_MAJOR && NAMEOF_VERSION_MINOR == NAMEOF_TEST_VERSION_MINOR && NAMEOF_VERSION_PATCH == NAMEOF_TEST_VERSION_PATCH,
              "nameof.hpp version must match the CMake project version.");
#endif

#if defined(NAMEOF_TEST_NO_RTTI)
static_assert(!nameof::is_nameof_type_rtti_supported);
#endif

enum class DefaultRangeBoundary {
  lower = -128,
  upper = 127,
  outside = 128,
};

#if defined(NAMEOF_ENUM_SUPPORTED)
static_assert(nameof::nameof_enum(DefaultRangeBoundary::lower) == "lower");
static_assert(nameof::nameof_enum(DefaultRangeBoundary::upper) == "upper");
#  if !defined(NAMEOF_TEST_STD_REFLECTION) // Standard reflection ignores the range.
static_assert(nameof::nameof_enum(DefaultRangeBoundary::outside).empty());
#  endif
#endif

#if defined(__GNUC__) && !defined(_MSC_VER)
extern void weak_function() __attribute__((weak));
static_assert(!nameof::detail::is_nullptr_v<&weak_function>);
static_assert(NAMEOF_POINTER(&weak_function) == "weak_function");
#endif

enum UnfixedEnum { unfixed_first = 1, unfixed_second = 2 };

#if defined(NAMEOF_ENUM_SUPPORTED)
static_assert(nameof::nameof_enum(unfixed_second) == "unfixed_second");
static_assert(nameof::nameof_enum(static_cast<UnfixedEnum>(3)).empty());
#endif

#if defined(NAMEOF_TYPE_SUPPORTED)
static_assert(NAMEOF_TYPE(int) == "int");
static_assert(NAMEOF_FULL_TYPE(int) == "int");
static_assert(NAMEOF_SHORT_TYPE(int) == "int");
#endif

struct StandaloneRttiBase {
  virtual ~StandaloneRttiBase() = default;
};

struct StandaloneRttiDerived : StandaloneRttiBase {};

#if defined(_MSC_VER)
int __cdecl main() {
#else
int main() {
#endif
#if defined(NAMEOF_TYPE_RTTI_SUPPORTED)
  StandaloneRttiDerived derived;
  StandaloneRttiBase& base = derived;
  return NAMEOF_SHORT_TYPE_RTTI(base) == "StandaloneRttiDerived" ? 0 : 1;
#else
  return 0;
#endif
}
