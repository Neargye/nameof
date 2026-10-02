// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <nameof.hpp>

#if defined(NAMEOF_ARRAY_CONSTEXPR) || defined(NAMEOF_FOR_EACH_256) || defined(NAMEOF_DETAIL_CDECL)
#  error "nameof internal implementation macro leaked from the public header."
#endif

enum class DefaultRangeBoundary {
  lower = -128,
  upper = 127,
  outside = 128,
};

#if defined(NAMEOF_ENUM_SUPPORTED)
static_assert(nameof::nameof_enum(DefaultRangeBoundary::lower) == "lower");
static_assert(nameof::nameof_enum(DefaultRangeBoundary::upper) == "upper");
static_assert(nameof::nameof_enum(DefaultRangeBoundary::outside).empty());
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
