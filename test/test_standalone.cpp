// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.

// User globals with common names must not trigger -Wshadow (GCC/Clang) or C4459 (MSVC) inside the header.
int count = 0;
int result = 0;
int size = 0;
int first = 0;
int last = 0;
int prefix = 0;

// Strict builds are Release: keep the default assert for -Wcomma.
#undef NDEBUG
#include <nameof.hpp>

#if defined(NAMEOF_FOR_EACH_256) || defined(NAMEOF_DETAIL_ASSERT) || defined(NAMEOF_DETAIL_LIFETIMEBOUND) || defined(NAMEOF_DETAIL_DEMANGLE) || defined(NAMEOF_DETAIL_CDECL) || defined(NAMEOF_DETAIL_SIGNATURE) || defined(NAMEOF_DETAIL_CONSTEVAL) || defined(NAMEOF_DETAIL_USE_STD_REFLECTION) || defined(NAMEOF_DETAIL_HAS_BIT_CAST) || defined(NAMEOF_DETAIL_ENUM_CAST) || defined(NAMEOF_DETAIL_STRICT_ENUM_CAST) || defined(NAMEOF_DETAIL_TEST_FALLBACK) || defined(NAMEOF_DETAIL_NVCC) || defined(NAMEOF_DETAIL_NVCC_NAME) || defined(NAMEOF_DETAIL_RESHARPER)
#  error "nameof internal implementation macro leaked from the public header."
#endif

#if defined(NAMEOF_TEST_VERSION_MAJOR)
static_assert(NAMEOF_VERSION_MAJOR == NAMEOF_TEST_VERSION_MAJOR && NAMEOF_VERSION_MINOR == NAMEOF_TEST_VERSION_MINOR && NAMEOF_VERSION_PATCH == NAMEOF_TEST_VERSION_PATCH,
              "nameof.hpp version must match the CMake project version.");
#endif

#if defined(NAMEOF_TEST_NO_RTTI)
static_assert(!nameof::is_nameof_type_rtti_supported);
#endif

#if defined(NAMEOF_CONFIG_FILE) && !defined(NAMEOF_TEST_CONFIG_FILE)
#  error "NAMEOF_CONFIG_FILE was not included."
#endif

#if defined(NAMEOF_NO_STD_FORMAT) && __has_include(<format>) && ((defined(_MSVC_LANG) && _MSVC_LANG >= 202002L) || __cplusplus >= 202002L)
#  include <format>
// C++23 range formatting accepts cstring as a sequence of characters.
#  if defined(__cpp_lib_format) && __cpp_lib_format >= 201907L && !defined(__cpp_lib_format_ranges)
static_assert(!std::is_default_constructible_v<std::formatter<nameof::cstring<1>, char>>, "NAMEOF_NO_STD_FORMAT must not define the formatter.");
#  endif
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

enum class ZeroFlags : unsigned { zero_none = 0, zero_first = 1 };

#if defined(NAMEOF_ENUM_SUPPORTED)
static_assert(nameof::nameof_enum(unfixed_second) == "unfixed_second");
static_assert(nameof::nameof_enum(static_cast<UnfixedEnum>(3)).empty());
static_assert(nameof::nameof_enum<unfixed_first>() == "unfixed_first");
#endif

struct StandaloneMembers {
  int field;
  void method() {}
};

void standalone_function() {}

// With GCC -fno-delete-null-pointer-checks, `&T::method == nullptr` is not a constant expression.
#if defined(NAMEOF_MEMBER_SUPPORTED)
static_assert(NAMEOF_MEMBER(&StandaloneMembers::field) == "field");
static_assert(NAMEOF_MEMBER(&StandaloneMembers::method) == "method");
static_assert(NAMEOF_MEMBER(static_cast<int StandaloneMembers::*>(nullptr)) == "nullptr");
static_assert(NAMEOF_MEMBER(static_cast<void (StandaloneMembers::*)()>(nullptr)) == "nullptr");
#endif

#if defined(NAMEOF_POINTER_SUPPORTED)
static_assert(NAMEOF_POINTER(&standalone_function) == "standalone_function");
static_assert(NAMEOF_POINTER(static_cast<void (*)()>(nullptr)) == "nullptr");
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
#if defined(NAMEOF_TEST_CONFIG_FILE) || defined(NAMEOF_NO_ASSERT)
  // The index of the terminator is inside the buffer but fails the bounds check, which must not abort here.
  auto name = NAMEOF(standalone_function);
  if (name[name.size()] != '\0') {
    return 3;
  }
  // A substr position past the end fails its check too, and then gives the empty end of the name.
  const nameof::name_view view = name;
  const auto tail = view.substr(view.size() + 1);
  if (tail.data() != view.data() + view.size() || tail.size() != 0) {
    return 6;
  }
#  if defined(NAMEOF_TEST_CONFIG_FILE)
  if (nameof_test_assert_failures != 2) {
    return 4;
  }
#  endif
#endif
#if defined(NAMEOF_ENUM_SUPPORTED)
  if (nameof::nameof_enum_flag(static_cast<UnfixedEnum>(3)) != "unfixed_first|unfixed_second") {
    return 2;
  }
#  if defined(NAMEOF_NO_NAMED_ZERO_IN_FLAGS)
  if (!nameof::nameof_enum_flag(ZeroFlags::zero_none).empty() || nameof::nameof_enum_flag(ZeroFlags::zero_first) != "zero_first") {
    return 7;
  }
#  else
  if (nameof::nameof_enum_flag(ZeroFlags::zero_none) != "zero_none" || nameof::nameof_enum_flag(ZeroFlags::zero_first) != "zero_first") {
    return 7;
  }
#  endif
#endif
#if defined(NAMEOF_NO_STD_FORMAT) && defined(__cpp_lib_format_ranges) && defined(NAMEOF_TYPE_SUPPORTED)
  if (std::format("{}", NAMEOF_TYPE(int)) != "['i', 'n', 't']") {
    return 5;
  }
#endif
#if defined(NAMEOF_NO_STD_FORMAT) && defined(__cpp_lib_format_ranges)
  if (std::format("{}", NAMEOF(DefaultRangeBoundary::lower)) != "['l', 'o', 'w', 'e', 'r']") {
    return 6;
  }
#endif
#if defined(NAMEOF_TYPE_RTTI_SUPPORTED)
  StandaloneRttiDerived derived;
  StandaloneRttiBase& base = derived;
  return NAMEOF_SHORT_TYPE_RTTI(base) == "StandaloneRttiDerived" ? 0 : 1;
#else
  return 0;
#endif
}
