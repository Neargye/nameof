//  _   _                             __    _____
// | \ | |                           / _|  / ____|_     _
// |  \| | __ _ _ __ ___   ___  ___ | |_  | |   _| |_ _| |_
// | . ` |/ _` | '_ ` _ \ / _ \/ _ \|  _| | |  |_   _|_   _|
// | |\  | (_| | | | | | |  __/ (_) | |   | |____|_|   |_|
// |_| \_|\__,_|_| |_| |_|\___|\___/|_|    \_____|
// https://github.com/Neargye/nameof
// version 0.11.0
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2016 - 2026 Daniil Goncharov <neargye@gmail.com>.
//
// Permission is hereby  granted, free of charge, to any  person obtaining a copy
// of this software and associated  documentation files (the "Software"), to deal
// in the Software  without restriction, including without  limitation the rights
// to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
// copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
// IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
// FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
// AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
// LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef NEARGYE_NAMEOF_HPP
#define NEARGYE_NAMEOF_HPP

#define NAMEOF_VERSION_MAJOR 0
#define NAMEOF_VERSION_MINOR 11
#define NAMEOF_VERSION_PATCH 0

#include <array>
#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <limits>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

// Define NAMEOF_CONFIG_FILE to include a header with nameof options. It is included before any option is read.
#if defined(NAMEOF_CONFIG_FILE)
#  include NAMEOF_CONFIG_FILE
#endif

#if !defined(NAMEOF_FORCE_COMPILER_SPECIFIC_REFLECTION) && defined(__cpp_impl_reflection) && __cpp_impl_reflection >= 202506L
#  if __has_include(<meta>)
#    include <meta>
#  endif
#  if defined(__cpp_lib_reflection) && __cpp_lib_reflection >= 202506L && defined(__cpp_lib_define_static) && __cpp_lib_define_static >= 202506L
#    define NAMEOF_DETAIL_USE_STD_REFLECTION 1
#  endif
#endif

// Define NAMEOF_REQUIRE_STD_REFLECTION to fail compilation when standard reflection is unavailable.
#if defined(NAMEOF_REQUIRE_STD_REFLECTION)
#  if defined(NAMEOF_FORCE_COMPILER_SPECIFIC_REFLECTION)
static_assert(false, "NAMEOF_REQUIRE_STD_REFLECTION and NAMEOF_FORCE_COMPILER_SPECIFIC_REFLECTION cannot be used together.");
#  elif !defined(NAMEOF_DETAIL_USE_STD_REFLECTION)
static_assert(false, "NAMEOF_REQUIRE_STD_REFLECTION requires C++26 standard reflection support.");
#  endif
#endif

// NAMEOF_ASSERT checks preconditions and defaults to assert. Define your own NAMEOF_ASSERT(...) or disable checks with NAMEOF_NO_ASSERT.
#if defined(NAMEOF_NO_ASSERT) && defined(NAMEOF_ASSERT)
static_assert(false, "NAMEOF_NO_ASSERT and NAMEOF_ASSERT cannot be used together.");
#endif

#if (defined(__clang__) || defined(__GNUC__)) && defined(__PRETTY_FUNCTION__)
static_assert(false, "nameof requires the builtin __PRETTY_FUNCTION__, not a macro.");
#endif

// The cast keeps a custom NAMEOF_ASSERT an expression and ignores its result, including an overloaded comma operator.
#if defined(NAMEOF_NO_ASSERT)
#  define NAMEOF_DETAIL_ASSERT(...) static_cast<void>(0)
#elif defined(NAMEOF_ASSERT)
#  define NAMEOF_DETAIL_ASSERT(...) static_cast<void>(NAMEOF_ASSERT(__VA_ARGS__))
#else
#  include <cassert>
#  define NAMEOF_DETAIL_ASSERT(...) assert((__VA_ARGS__))
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(push)
#  pragma warning(disable : 4459 6244) // Internal names may shadow globals declared by the caller.
#  pragma warning(disable : 4514)  // Unreferenced inline function has been removed.
#  pragma warning(disable : 6262)  // Code analysis counts the stack of compile-time helpers that hold a 512-name block.
#  pragma warning(disable : 6326)  // Code analysis false positive for constant comparisons in consteval helpers.
#  pragma warning(disable : 28020) // Code analysis false positive for bounds-checked enum value arrays.
#elif defined(__clang__)
#  pragma clang diagnostic push
#  if __has_warning("-Wnrvo")
#    pragma clang diagnostic ignored "-Wnrvo" // Opt-in warning, NRVO does not matter for compile-time helpers.
#  endif
#  if __has_warning("-Wenum-constexpr-conversion")
#    pragma clang diagnostic ignored "-Wenum-constexpr-conversion" // Clang 16-19: scanning casts candidates outside the value range of unfixed enums.
#  elif __clang_major__ >= 16
#    define NAMEOF_DETAIL_STRICT_ENUM_CAST 1 // Clang 20+ made it a hard error (CWG1766). Apple Clang versions differ, so the warning is probed.
#  endif
#elif defined(__GNUC__) && __GNUC__ >= 14
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wnrvo" // Opt-in warning, NRVO does not matter for compile-time helpers.
#endif

// Checks nameof_type compiler compatibility.
#if defined(__clang__) && __clang_major__ >= 5 || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER) && _MSC_VER >= 1920
#  define NAMEOF_TYPE_SUPPORTED 1
#endif

// Checks nameof_type_rtti compiler compatibility. Not __cpp_rtti: clang-cl defines it even with /GR-.
#if defined(__GXX_RTTI) || defined(_CPPRTTI)
#  define NAMEOF_TYPE_RTTI_SUPPORTED 1
#endif

#if defined(NAMEOF_TYPE_RTTI_SUPPORTED) || defined(NAMEOF_TYPE_NO_CHECK_SUPPORT)
#  include <typeinfo>
#  if __has_include(<cxxabi.h>)
#    include <cxxabi.h>
#    include <cstdlib>
#    define NAMEOF_DETAIL_DEMANGLE 1
#  endif
#endif

// Checks nameof_member and nameof_pointer compiler compatibility. MSVC < 19.44 needs C++20 for its data member fallback.
#if defined(__clang__) && __clang_major__ >= 5 || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER) && (_MSC_VER >= 1944 || _MSVC_LANG >= 202002L)
#  define NAMEOF_MEMBER_SUPPORTED 1
#  define NAMEOF_POINTER_SUPPORTED 1
#endif

// Checks nameof_enum compiler compatibility.
#if defined(NAMEOF_DETAIL_USE_STD_REFLECTION) || defined(__clang__) && __clang_major__ >= 5 || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER) && _MSC_VER >= 1920
#  define NAMEOF_ENUM_SUPPORTED 1
#endif

#if !defined(NAMEOF_DETAIL_USE_STD_REFLECTION) && (defined(__NVCOMPILER) || defined(__NVCC__))
#  define NAMEOF_DETAIL_NVCC 1
#endif

// Enum value must be greater than or equal to NAMEOF_ENUM_RANGE_MIN (-128 by default). Redefine the macro to change it for all enum types.
#if !defined(NAMEOF_ENUM_RANGE_MIN)
#  define NAMEOF_ENUM_RANGE_MIN -128
#endif

// Enum value must be less than or equal to NAMEOF_ENUM_RANGE_MAX (127 by default). Redefine the macro to change it for all enum types.
#if !defined(NAMEOF_ENUM_RANGE_MAX)
#  define NAMEOF_ENUM_RANGE_MAX 127
#endif

namespace nameof {

// If you need another string_view type, define the macro NAMEOF_USING_ALIAS_STRING_VIEW.
#if defined(NAMEOF_USING_ALIAS_STRING_VIEW)
NAMEOF_USING_ALIAS_STRING_VIEW
#else
using std::string_view;
#endif

// If you need another string type, define the macro NAMEOF_USING_ALIAS_STRING.
#if defined(NAMEOF_USING_ALIAS_STRING)
NAMEOF_USING_ALIAS_STRING
#else
using std::string;
#endif

// Checks whether nameof_type is supported by the compiler.
#if defined(NAMEOF_TYPE_SUPPORTED) || defined(NAMEOF_TYPE_NO_CHECK_SUPPORT)
inline constexpr bool is_nameof_type_supported = true;
#else
inline constexpr bool is_nameof_type_supported = false;
#endif

// Checks whether nameof_type_rtti is supported by the compiler.
#if defined(NAMEOF_TYPE_RTTI_SUPPORTED) || defined(NAMEOF_TYPE_NO_CHECK_SUPPORT)
inline constexpr bool is_nameof_type_rtti_supported = true;
#else
inline constexpr bool is_nameof_type_rtti_supported = false;
#endif

// Checks whether nameof_member is supported by the compiler.
#if defined(NAMEOF_MEMBER_SUPPORTED) || defined(NAMEOF_TYPE_NO_CHECK_SUPPORT)
inline constexpr bool is_nameof_member_supported = true;
#else
inline constexpr bool is_nameof_member_supported = false;
#endif

// Checks whether nameof_pointer is supported by the compiler.
#if defined(NAMEOF_POINTER_SUPPORTED) || defined(NAMEOF_TYPE_NO_CHECK_SUPPORT)
inline constexpr bool is_nameof_pointer_supported = true;
#else
inline constexpr bool is_nameof_pointer_supported = false;
#endif

// Checks whether nameof_enum is supported by the compiler.
#if defined(NAMEOF_ENUM_SUPPORTED) || defined(NAMEOF_ENUM_NO_CHECK_SUPPORT)
inline constexpr bool is_nameof_enum_supported = true;
#else
inline constexpr bool is_nameof_enum_supported = false;
#endif

// Keep the calling convention stable for fixed-offset __FUNCSIG__ parsing.
#if defined(_MSC_VER) && !defined(__clang__)
#  define NAMEOF_DETAIL_CDECL __cdecl
#else
#  define NAMEOF_DETAIL_CDECL
#endif

// The signature of the enclosing function without the "]" (GCC, Clang) or ">(void) noexcept" (MSVC) after its template arguments.
#if defined(__clang__) || defined(__GNUC__)
#  define NAMEOF_DETAIL_SIGNATURE ::nameof::string_view{__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - sizeof("]")}
#elif defined(_MSC_VER)
#  define NAMEOF_DETAIL_SIGNATURE ::nameof::string_view{__FUNCSIG__, sizeof(__FUNCSIG__) - sizeof(">(void) noexcept")}
#else
#  define NAMEOF_DETAIL_SIGNATURE ::nameof::string_view{}
#endif

#if defined(__cpp_consteval) && __cpp_consteval >= 201811L
#  define NAMEOF_DETAIL_CONSTEVAL consteval
#else
#  define NAMEOF_DETAIL_CONSTEVAL constexpr
#endif

#if defined(__has_builtin)
#  if __has_builtin(__builtin_bit_cast)
#    define NAMEOF_DETAIL_HAS_BIT_CAST 1
#  endif
#endif

// Clang 16+ rejects out-of-range static_casts to unfixed enums in constant expressions (https://reviews.llvm.org/D130058, D131307).
#if defined(NAMEOF_DETAIL_HAS_BIT_CAST)
#  define NAMEOF_DETAIL_ENUM_CAST(E, V) __builtin_bit_cast(E, V)
#else
#  define NAMEOF_DETAIL_ENUM_CAST(E, V) static_cast<E>(V)
#endif

namespace customize {

// Compiler-specific enum reflection scans [min, max]. Redefine NAMEOF_ENUM_RANGE_MIN/MAX globally or specialize enum_range for a specific enum.
template <typename E>
struct enum_range {
  inline static constexpr int min = NAMEOF_ENUM_RANGE_MIN;
  inline static constexpr int max = NAMEOF_ENUM_RANGE_MAX;
};

// If you need custom enum names, specialize enum_name for that enum type.
template <typename E>
constexpr string_view enum_name(E) noexcept {
  return string_view{""};
}

// If you need a custom type name, specialize type_name for that type.
template <typename T>
constexpr string_view type_name() noexcept {
  return string_view{""};
}

// If you need a custom member name, specialize member_name for that member.
template <auto V>
constexpr string_view member_name() noexcept {
  return string_view{""};
}

// If you need a custom pointer name, specialize pointer_name for that pointer.
template <auto V>
constexpr string_view pointer_name() noexcept {
  return string_view{""};
}

} // namespace nameof::customize

template <std::uint16_t N>
class [[nodiscard]] cstring {
 public:
  using value_type             = char;
  using size_type              = std::size_t;
  using difference_type        = std::ptrdiff_t;
  using pointer                = const char*;
  using const_pointer          = const char*;
  using reference              = const char&;
  using const_reference        = const char&;
  using iterator               = const char*;
  using const_iterator         = const char*;
  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  constexpr explicit cstring(string_view str) noexcept : cstring{check_size(str).data(), std::make_integer_sequence<std::uint16_t, N>{}} {}

  template <bool Empty = (N == 0), std::enable_if_t<Empty, int> = 0>
  constexpr cstring() noexcept : chars_{} {}

  [[nodiscard]] constexpr const_pointer data() const noexcept { return N == 0 ? empty_ : chars_; }

  [[nodiscard]] constexpr size_type size() const noexcept { return N; }

  [[nodiscard]] constexpr const_iterator begin() const noexcept { return data(); }

  [[nodiscard]] constexpr const_iterator end() const noexcept { return data() + size(); }

  [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return begin(); }

  [[nodiscard]] constexpr const_iterator cend() const noexcept { return end(); }

  [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator{end()}; }

  [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator{begin()}; }

  [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }

  [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept { return rend(); }

  [[nodiscard]] constexpr const_reference operator[](size_type i) const noexcept { return NAMEOF_DETAIL_ASSERT(i < size()), data()[i]; }

  [[nodiscard]] constexpr const_reference front() const noexcept { return data()[0]; }

  [[nodiscard]] constexpr const_reference back() const noexcept { return data()[N == 0 ? 0 : N - 1]; }

  [[nodiscard]] constexpr size_type length() const noexcept { return size(); }

  [[nodiscard]] constexpr bool empty() const noexcept { return N == 0; }

  [[nodiscard]] constexpr int compare(string_view str) const noexcept { return string_view{data(), size()}.compare(str); }

  [[nodiscard]] constexpr const char* c_str() const noexcept { return data(); }

  [[nodiscard]] string str() const { return {data(), size()}; }

  [[nodiscard]] constexpr operator string_view() const& noexcept { return {data(), size()}; }

  // A view of a temporary such as NAMEOF(x) would dangle.
  constexpr operator string_view() const&& noexcept = delete;

  [[nodiscard]] constexpr explicit operator const_pointer() const noexcept { return data(); }

  [[nodiscard]] explicit operator string() const { return {data(), size()}; }

  // Without the cstring<M> overloads, comparing two cstrings is ambiguous between the string_view ones.
  template <std::uint16_t M>
  [[nodiscard]] friend constexpr bool operator==(const cstring& lhs, const cstring<M>& rhs) noexcept { return N == M && lhs.compare(rhs) == 0; }
  [[nodiscard]] friend constexpr bool operator==(const cstring& lhs, string_view rhs) noexcept { return lhs.size() == rhs.size() && lhs.compare(rhs) == 0; }
  [[nodiscard]] friend constexpr bool operator==(string_view lhs, const cstring& rhs) noexcept { return lhs.size() == rhs.size() && lhs.compare(rhs) == 0; }

  template <std::uint16_t M>
  [[nodiscard]] friend constexpr bool operator!=(const cstring& lhs, const cstring<M>& rhs) noexcept { return N != M || lhs.compare(rhs) != 0; }
  [[nodiscard]] friend constexpr bool operator!=(const cstring& lhs, string_view rhs) noexcept { return lhs.size() != rhs.size() || lhs.compare(rhs) != 0; }
  [[nodiscard]] friend constexpr bool operator!=(string_view lhs, const cstring& rhs) noexcept { return lhs.size() != rhs.size() || lhs.compare(rhs) != 0; }

  template <std::uint16_t M>
  [[nodiscard]] friend constexpr bool operator<(const cstring& lhs, const cstring<M>& rhs) noexcept { return lhs.compare(rhs) < 0; }
  [[nodiscard]] friend constexpr bool operator<(const cstring& lhs, string_view rhs) noexcept { return lhs.compare(rhs) < 0; }
  [[nodiscard]] friend constexpr bool operator<(string_view lhs, const cstring& rhs) noexcept { return lhs.compare(rhs) < 0; }

  template <std::uint16_t M>
  [[nodiscard]] friend constexpr bool operator<=(const cstring& lhs, const cstring<M>& rhs) noexcept { return lhs.compare(rhs) <= 0; }
  [[nodiscard]] friend constexpr bool operator<=(const cstring& lhs, string_view rhs) noexcept { return lhs.compare(rhs) <= 0; }
  [[nodiscard]] friend constexpr bool operator<=(string_view lhs, const cstring& rhs) noexcept { return lhs.compare(rhs) <= 0; }

  template <std::uint16_t M>
  [[nodiscard]] friend constexpr bool operator>(const cstring& lhs, const cstring<M>& rhs) noexcept { return lhs.compare(rhs) > 0; }
  [[nodiscard]] friend constexpr bool operator>(const cstring& lhs, string_view rhs) noexcept { return lhs.compare(rhs) > 0; }
  [[nodiscard]] friend constexpr bool operator>(string_view lhs, const cstring& rhs) noexcept { return lhs.compare(rhs) > 0; }

  template <std::uint16_t M>
  [[nodiscard]] friend constexpr bool operator>=(const cstring& lhs, const cstring<M>& rhs) noexcept { return lhs.compare(rhs) >= 0; }
  [[nodiscard]] friend constexpr bool operator>=(const cstring& lhs, string_view rhs) noexcept { return lhs.compare(rhs) >= 0; }
  [[nodiscard]] friend constexpr bool operator>=(string_view lhs, const cstring& rhs) noexcept { return lhs.compare(rhs) >= 0; }

 private:
  [[nodiscard]] static constexpr string_view check_size(string_view str) noexcept { return NAMEOF_DETAIL_ASSERT(str.size() == N), str; }

  template <std::uint16_t... J>
  constexpr cstring([[maybe_unused]] const char* str, std::integer_sequence<std::uint16_t, J...>) noexcept : chars_{str[J]..., '\0'} {}

 public:
  // Public on purpose: GCC 13/14 rechecks private access to constexpr view storage in templated lambdas (magic_enum#452).
  // Empty strings point here, so pointers from a cstring<0> temporary stay valid after the full expression.
  static constexpr char empty_[1]{};
  char chars_[static_cast<std::size_t>(N) + 1];
};

template <typename Char, typename Traits, std::uint16_t N>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, const cstring<N>& str) {
  if constexpr (std::is_same_v<Char, char>) {
    return os << std::basic_string_view<Char, Traits>{str.data(), str.size()};
  } else {
    return os << std::basic_string<Char, Traits>{str.begin(), str.end()};
  }
}

namespace detail {

// Keeps support static_asserts dependent, so they fire only when the API is used.
template <bool Supported, typename...>
inline constexpr bool supported_v = Supported;

template <std::size_t N>
constexpr auto to_cstring(string_view str) noexcept {
  static_assert(N <= (std::numeric_limits<std::uint16_t>::max)(), "nameof::cstring supports names of at most UINT16_MAX characters.");
  return cstring<static_cast<std::uint16_t>(N)>{str};
}

constexpr bool is_name_start(char c) noexcept {
  return (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z') ||
         (c == '_') || static_cast<unsigned char>(c) >= 0x80;
}

constexpr bool is_name_char(char c) noexcept {
  return is_name_start(c) || (c >= '0' && c <= '9');
}

// Find the opening quote of the literal whose closing quote is at end, or npos.
constexpr std::size_t find_literal_begin(string_view name, std::size_t end) noexcept {
  for (std::size_t i = end; i > 0; --i) {
    if (name[i - 1] != name[end]) {
      continue;
    }
    // A quote preceded by an odd number of backslashes is escaped.
    std::size_t escapes_begin = i - 1;
    while (escapes_begin > 0 && name[escapes_begin - 1] == '\\') {
      --escapes_begin;
    }
    if ((i - 1 - escapes_begin) % 2 == 0) {
      return i - 1;
    }
    i = escapes_begin + 1;
  }
  return std::string_view::npos;
}

// Find the start of trailing open/close groups and preceding spaces, or npos if unbalanced: " (a)(b)" in "f<int> (a)(b)" for '(' and ')'.
constexpr std::size_t find_suffix_begin(string_view name, char open, char close) noexcept {
  for (std::size_t i = name.size(), suffix_depth = 0, nested_depth = 0; i > 0; --i) {
    const auto c = name[i - 1];
    // Inside a suffix group only brackets and quotes matter.
    if (suffix_depth > 0 && c != '(' && c != ')' && c != '[' && c != ']' && c != '{' && c != '}' && c != '<' && c != '>' && c != '`' && c != '\'' && c != '"') {
      continue;
    }
    const auto next = i < name.size() ? name[i] : '\0';
    // MSVC quotes scopes as in `anonymous namespace'::name.
    const auto closes_scope = c == '\'' && next == ':' && i + 1 < name.size() && name[i + 1] == ':';
    // Ignore digit separators such as 1'000 and 0xFF'FF.
    const auto closes_literal = c == '"' || (c == '\'' && (next == '_' || !is_name_char(next)));
    if (suffix_depth > 0 && c != close && (c == ')' || c == ']' || c == '}' || closes_scope)) {
      ++nested_depth;
    } else if (suffix_depth > 0 && closes_literal) {
      // Skip the whole literal, so brackets and quotes inside it are ignored.
      const auto literal_begin = find_literal_begin(name, i - 1);
      if (literal_begin == std::string_view::npos) {
        return std::string_view::npos;
      }
      i = literal_begin + 1;
    } else if (suffix_depth > 0 && c != open && (c == '(' || c == '[' || c == '{' || c == '`')) {
      // An opening bracket without a matching closing one is unbalanced.
      if (nested_depth == 0) {
        return std::string_view::npos;
      }
      --nested_depth;
    } else if (nested_depth > 0) {
      continue;
    } else if (c == close && !(c == '>' && i > 1 && name[i - 2] == '-')) { // "->" is not a bracket.
      ++suffix_depth;
    } else if (c == open) {
      if (suffix_depth == 0) {
        return std::string_view::npos;
      }
      --suffix_depth;
    } else if (suffix_depth == 0) {
      // The first character outside the groups ends the name.
      while (i > 0 && name[i - 1] == ' ') {
        --i;
      }
      return i;
    }
  }
  // Only groups, no name before them.
  return 0;
}

// The last identifier of an expression, empty if it has no name: "ns::S<int>::f<int>(x)" -> "f", or "f<int>" without remove_suffix.
constexpr string_view pretty_name(string_view name, bool remove_suffix = true) noexcept {
  std::size_t prefix_end = 0;
  while (prefix_end < name.size() && is_name_char(name[prefix_end])) {
    ++prefix_end;
  }
  if (prefix_end < name.size() && (name[prefix_end] == '"' || name[prefix_end] == '\'')) {
    return {}; // Literal, possibly with an encoding or raw prefix.
  } else if (!name.empty() && name[0] >= '0' && name[0] <= '9') {
    return {};
  }
  // Fast path for a plain identifier.
  if (prefix_end != 0 && prefix_end == name.size()) {
    return name;
  }

  // Drop trailing call arguments, then template arguments.
  const auto name_end = find_suffix_begin(name, '(', ')');
  if (name_end == std::string_view::npos) {
    return {};
  }

  const auto suffix_begin = find_suffix_begin({name.data(), name_end}, '<', '>');
  if (suffix_begin == std::string_view::npos) {
    return {};
  }

  // The name is the identifier right before the suffix, so qualifiers and object expressions are dropped.
  std::size_t name_begin = suffix_begin;
  while (name_begin > 0 && is_name_char(name[name_begin - 1])) {
    --name_begin;
  }

  const auto name_size = (remove_suffix ? suffix_begin : name_end) - name_begin;
  if (name_size > 0 && is_name_start(name[name_begin])) {
    return {name.data() + name_begin, name_size};
  }

  return {};
}

constexpr bool is_operator_keyword(string_view str, std::size_t i) noexcept {
  return i + 8 <= str.size() && str[i] == 'o' &&
         string_view{str.data() + i, 8}.compare("operator") == 0 &&
         (i == 0 || !is_name_char(str[i - 1])) &&
         (i + 8 == str.size() || !is_name_char(str[i + 8]));
}

// Operators have no name, but pretty_name yields "operator" ("S::operator()"), GCC "__conv_op" or the token after the keyword ("S::operator new", "operator\"\"_x").
constexpr string_view pretty_member_name(string_view signature) noexcept {
  const auto name = pretty_name(signature);
  if (name.empty() || name.compare("operator") == 0 || name.compare("__conv_op") == 0) {
    return {};
  }
  // Look for the keyword left of the name. Enclosing templates and function scopes stop at '>' or ')'.
  for (auto i = static_cast<std::size_t>(name.data() - signature.data()); i > 0; --i) {
    const auto c = signature[i - 1];
    if (!is_name_char(c) && c != ':' && c != ' ' && c != '"') {
      break;
    }
    if (is_operator_keyword(signature, i - 1)) {
      return {};
    }
  }
  return name;
}

#if defined(_MSC_VER) && !defined(__clang__)
constexpr std::size_t skip_template_arguments(string_view signature, std::size_t i) noexcept {
  if (i >= signature.size() || signature[i] != '<') {
    return i;
  }
  std::size_t depth = 0;
  do {
    const auto c = signature[i];
    // Operator names inside template arguments are not supported.
    if (is_operator_keyword(signature, i)) {
      return std::string_view::npos;
    }
    if (c == '<') {
      ++depth;
    } else if (c == '>') {
      --depth;
    }
    ++i;
  } while (i < signature.size() && depth > 0);

  return depth == 0 ? i : std::string_view::npos;
}

// MSVC spells function pointers as declarations (p<void __cdecl ns::g<int>(void)>): the name is the first identifier after ' ' or "::" followed by its parameters.
constexpr string_view pretty_function_name(string_view signature) noexcept {
  std::size_t template_begin = 0;
  for (; template_begin < signature.size() && signature[template_begin] != '<'; ++template_begin) {}
  if (template_begin == signature.size()) {
    return {};
  }
  for (std::size_t i = template_begin + 1; i + 2 < signature.size(); ++i) {
    if (signature[i] != ' ' && (signature[i] != ':' || signature[i + 1] != ':')) {
      continue;
    }

    const auto name_begin = i + (signature[i] == ' ' ? 1 : 2);
    if (!is_name_start(signature[name_begin])) {
      continue;
    }

    std::size_t name_end = name_begin + 1;
    while (name_end < signature.size() && is_name_char(signature[name_end])) {
      ++name_end;
    }

    const auto name = string_view{signature.data() + name_begin, name_end - name_begin};
    if (name.compare("operator") == 0) {
      return {};
    }

    const auto parameter_begin = skip_template_arguments(signature, name_end);
    if (parameter_begin == std::string_view::npos) {
      return {};
    }
    if (parameter_begin < signature.size() && signature[parameter_begin] == '(') {
      return name;
    }
    // Do not inspect function types or values inside these template arguments.
    i = parameter_begin - 1;
  }

  return {};
}
#endif

// std::make_unsigned_t<bool> is ill-formed.
template <typename T>
using make_unsigned_t = std::make_unsigned_t<std::conditional_t<std::is_same_v<T, bool>, unsigned char, T>>;

template <typename T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

#if defined(__clang__) || defined(__NVCOMPILER) || defined(__NVCC__)
// These compilers can leave a class-template enum's enumerators uninstantiated on a numeric-first lookup. Member lookup forces its definition, both outcomes yield R.
template <typename E, typename R, typename = void>
struct enum_definition { using type = R; };

template <typename E, typename R>
struct enum_definition<E, R, std::void_t<decltype(E::nameof_detail_enum_definition)>> { using type = R; };

template <typename T, typename R>
using enable_if_enum_t = typename enum_definition<remove_cvref_t<T>, std::enable_if_t<std::is_enum_v<remove_cvref_t<T>>, R>>::type;
#else
template <typename T, typename R>
using enable_if_enum_t = std::enable_if_t<std::is_enum_v<remove_cvref_t<T>>, R>;
#endif

// Enums may overload operator==.
template <typename E>
constexpr bool enum_value_equal(E lhs, E rhs) noexcept {
  using U = std::underlying_type_t<E>;
  return static_cast<U>(lhs) == static_cast<U>(rhs);
}

#if defined(NAMEOF_DETAIL_USE_STD_REFLECTION)

namespace reflection {

template <typename E>
consteval auto enumerators() noexcept {
  if constexpr (std::meta::is_enumerable_type(^^E)) {
    return std::define_static_array(std::meta::enumerators_of(^^E));
  } else {
    static_assert(supported_v<false, E>, "nameof::nameof_enum requires a complete enum definition.");
    return std::array<std::meta::info, 0>{};
  }
}

template <typename E>
inline constexpr auto enumerators_v = enumerators<E>();

template <typename U>
struct entry {
  U value;
  string_view name;
};

template <typename E>
consteval auto entries() noexcept {
  using U = std::underlying_type_t<E>;
  // Share array instantiations between enums with the same underlying type and size.
  std::array<entry<U>, enumerators_v<E>.size()> result{};
  std::size_t i = 0;
  for (const auto enumerator : enumerators_v<E>) {
    const auto identifier = std::meta::identifier_of(enumerator);
    // identifier_of's pointer is not a portable pointer NTTP, so keep define_static_string's storage.
    result[i++] = {static_cast<U>(std::meta::extract<E>(enumerator)), {std::define_static_string(identifier), identifier.size()}};
  }
  return result;
}

template <typename E>
inline constexpr auto entries_v = entries<E>();

template <typename E>
consteval auto flag_mask() noexcept {
  // Only a rejection filter: composite enumerators may add extra bits.
  using U = make_unsigned_t<std::underlying_type_t<E>>;
  U mask{};
  // Reads enumerators_v, not entries_v, so tiny enums do not build their name table.
  for (const auto enumerator : enumerators_v<E>) {
    mask = static_cast<U>(mask | static_cast<U>(std::meta::extract<E>(enumerator)));
  }
  return mask;
}

template <typename E>
inline constexpr auto flag_mask_v = flag_mask<E>();

template <typename E>
consteval bool is_dense() noexcept {
  using U = make_unsigned_t<std::underlying_type_t<E>>;
  for (std::size_t i = 1; i < entries_v<E>.size(); ++i) {
    const auto prev = entries_v<E>[i - 1].value;
    const auto next = entries_v<E>[i].value;
    if (next <= prev || static_cast<U>(static_cast<U>(next) - static_cast<U>(prev)) != U{1}) {
      return false;
    }
  }
  return !entries_v<E>.empty();
}

// Dense lookup needs only names at runtime, values remain compile-time metadata.
template <typename E>
consteval auto names() noexcept {
  std::array<string_view, enumerators_v<E>.size()> result{};
  for (std::size_t i = 0; i < result.size(); ++i) {
    result[i] = entries_v<E>[i].name;
  }
  return result;
}

template <typename E>
inline constexpr auto names_v = names<E>();

template <typename E, std::size_t... K>
constexpr auto enum_name([[maybe_unused]] E value, std::index_sequence<K...>) noexcept {
  string_view result{""};
  static_cast<void>(((entries_v<E>[K].value == static_cast<std::underlying_type_t<E>>(value)
                     ? (static_cast<void>(result = entries_v<E>[K].name), true) : false) || ...));
  return result;
}

template <typename E>
constexpr auto enum_name(E value) noexcept {
  if constexpr (enumerators_v<E>.size() <= 4) {
#if defined(__cpp_expansion_statements) && __cpp_expansion_statements >= 202506L
    // Avoid building a tiny table when expansion statements are available.
    template for (constexpr auto enumerator : enumerators_v<E>) {
      constexpr auto candidate = std::meta::extract<E>(enumerator);
      if (enum_value_equal(candidate, value)) {
        constexpr auto identifier = std::meta::identifier_of(enumerator);
        constexpr auto name = std::define_static_string(identifier);
        return string_view{name, identifier.size()};
      }
    }
    return string_view{""};
#else
    return enum_name(value, std::make_index_sequence<enumerators_v<E>.size()>{});
#endif
  } else if constexpr (is_dense<E>()) {
    using U = make_unsigned_t<std::underlying_type_t<E>>;
    constexpr auto first = entries_v<E>.front().value;
    constexpr auto last = entries_v<E>.back().value;
    const auto v = static_cast<std::underlying_type_t<E>>(value);
    if (v >= first && v <= last) {
      // Signed subtraction may overflow. Cast back to U after promotion of small integer types.
      return names_v<E>[static_cast<std::size_t>(static_cast<U>(static_cast<U>(v) - static_cast<U>(first)))];
    }
  } else {
    // Preserve the compiler's switch/table optimisation for sparse enums and first aliases.
    return enum_name(value, std::make_index_sequence<entries_v<E>.size()>{});
  }
  return string_view{""};
}

template <typename E, E V>
consteval auto enum_name() noexcept {
  // A constant-value query creates only its selected name, without the runtime table.
  for (const auto enumerator : enumerators_v<E>) {
    if (enum_value_equal(std::meta::extract<E>(enumerator), V)) {
      const auto identifier = std::meta::identifier_of(enumerator);
      return string_view{std::define_static_string(identifier), identifier.size()};
    }
  }
  return string_view{""};
}

} // namespace reflection

#endif

// The enumerator is the last identifier after "::" (Color::RED), ' ' (V = BLUE), MSVC '<' (nv<BLUE>) or ',' (enum_name<enum E,BLUE>). Unnamed values are casts: (Color)5.
NAMEOF_DETAIL_CONSTEVAL bool enum_name_valid(string_view signature) noexcept {
  std::size_t i = signature.size();
  while (i > 0 && is_name_char(signature[i - 1])) {
    --i;
  }
  if (i == 0 || i == signature.size() || !is_name_start(signature[i])) {
    return false;
  }

  const auto c = signature[i - 1];
  return c == ' ' || c == '<' || c == ',' || (c == ':' && i >= 2 && signature[i - 2] == ':');
}

template <typename E, E V>
NAMEOF_DETAIL_CONSTEVAL auto enum_name() noexcept {
  if constexpr (is_nameof_enum_supported) {
#if defined(NAMEOF_DETAIL_USE_STD_REFLECTION)
    constexpr auto name = reflection::enum_name<E, V>();
#else
    constexpr auto signature = NAMEOF_DETAIL_SIGNATURE;
    constexpr auto name = enum_name_valid(signature) ? pretty_name(signature) : string_view{};
#endif
    return to_cstring<name.size()>(name);
  } else {
    return cstring<0>{};
  }
}

template <typename E, E V>
inline constexpr auto enum_name_v = enum_name<E, V>();

template <typename E, E V>
NAMEOF_DETAIL_CONSTEVAL auto custom_enum_name() noexcept {
  constexpr auto name = customize::enum_name<E>(V);
  return to_cstring<name.size()>(name);
}

template <typename E, E V>
inline constexpr auto custom_enum_name_v = custom_enum_name<E, V>();

#if !defined(NAMEOF_DETAIL_USE_STD_REFLECTION)

template <auto V>
NAMEOF_DETAIL_CONSTEVAL bool nv() noexcept {
  if constexpr (is_nameof_enum_supported) {
    return enum_name_valid(NAMEOF_DETAIL_SIGNATURE);
  } else {
    return false;
  }
}

template <typename E, typename U>
NAMEOF_DETAIL_CONSTEVAL E enum_cast(U value) noexcept {
  return NAMEOF_DETAIL_ENUM_CAST(E, value);
}

// The value at offset i from O. For flags, the range is bit positions, so the value is 1 << (i + O).
template <typename E, int O, bool IsFlags>
NAMEOF_DETAIL_CONSTEVAL auto value_at([[maybe_unused]] std::size_t i) noexcept {
  using U = std::underlying_type_t<E>;
  if constexpr (IsFlags) {
    if constexpr (std::is_same_v<U, bool>) {
      return true;
    } else {
      return static_cast<U>(make_unsigned_t<U>{1} << (static_cast<int>(i) + O));
    }
  } else {
    return static_cast<U>(static_cast<int>(i) + O);
  }
}

// std::cmp_less is C++20 and rejects bool.
template <typename L, typename R>
NAMEOF_DETAIL_CONSTEVAL bool cmp_less([[maybe_unused]] L lhs, [[maybe_unused]] R rhs) noexcept {
  if constexpr (!std::is_integral_v<L> || !std::is_integral_v<R>) {
    return false; // Rejected by reflected_min and reflected_max.
  } else if constexpr (std::is_same_v<L, bool>) {
    return cmp_less(static_cast<int>(lhs), rhs);
  } else if constexpr (std::is_same_v<R, bool>) {
    return cmp_less(lhs, static_cast<int>(rhs));
  } else if constexpr (std::is_signed_v<L> == std::is_signed_v<R>) {
    return lhs < rhs;
  } else if constexpr (std::is_signed_v<L>) {
    return lhs < 0 || static_cast<std::make_unsigned_t<L>>(lhs) < rhs;
  } else {
    return rhs > 0 && lhs < static_cast<std::make_unsigned_t<R>>(rhs);
  }
}

// Preserve bounds until checked for narrowing to int.
template <typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
NAMEOF_DETAIL_CONSTEVAL auto reflected_min() noexcept {
  static_assert(std::is_integral_v<decltype(customize::enum_range<E>::min)>, "nameof::customize::enum_range min must be integral.");
  if constexpr (IsFlags) {
    return 0;
  } else if constexpr (cmp_less((std::numeric_limits<U>::min)(), customize::enum_range<E>::min)) {
    return customize::enum_range<E>::min;
  } else {
    return (std::numeric_limits<U>::min)();
  }
}

template <typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
NAMEOF_DETAIL_CONSTEVAL auto reflected_max() noexcept {
  static_assert(std::is_integral_v<decltype(customize::enum_range<E>::max)>, "nameof::customize::enum_range max must be integral.");
  if constexpr (IsFlags) {
    return std::is_same_v<U, bool> ? 0 : std::numeric_limits<make_unsigned_t<U>>::digits - 1;
  } else if constexpr (cmp_less(customize::enum_range<E>::max, (std::numeric_limits<U>::max)())) {
    return customize::enum_range<E>::max;
  } else {
    return (std::numeric_limits<U>::max)();
  }
}

#define NAMEOF_FOR_EACH_256(T)                                                                                                                                                                     \
  T(  0)T(  1)T(  2)T(  3)T(  4)T(  5)T(  6)T(  7)T(  8)T(  9)T( 10)T( 11)T( 12)T( 13)T( 14)T( 15)T( 16)T( 17)T( 18)T( 19)T( 20)T( 21)T( 22)T( 23)T( 24)T( 25)T( 26)T( 27)T( 28)T( 29)T( 30)T( 31) \
  T( 32)T( 33)T( 34)T( 35)T( 36)T( 37)T( 38)T( 39)T( 40)T( 41)T( 42)T( 43)T( 44)T( 45)T( 46)T( 47)T( 48)T( 49)T( 50)T( 51)T( 52)T( 53)T( 54)T( 55)T( 56)T( 57)T( 58)T( 59)T( 60)T( 61)T( 62)T( 63) \
  T( 64)T( 65)T( 66)T( 67)T( 68)T( 69)T( 70)T( 71)T( 72)T( 73)T( 74)T( 75)T( 76)T( 77)T( 78)T( 79)T( 80)T( 81)T( 82)T( 83)T( 84)T( 85)T( 86)T( 87)T( 88)T( 89)T( 90)T( 91)T( 92)T( 93)T( 94)T( 95) \
  T( 96)T( 97)T( 98)T( 99)T(100)T(101)T(102)T(103)T(104)T(105)T(106)T(107)T(108)T(109)T(110)T(111)T(112)T(113)T(114)T(115)T(116)T(117)T(118)T(119)T(120)T(121)T(122)T(123)T(124)T(125)T(126)T(127) \
  T(128)T(129)T(130)T(131)T(132)T(133)T(134)T(135)T(136)T(137)T(138)T(139)T(140)T(141)T(142)T(143)T(144)T(145)T(146)T(147)T(148)T(149)T(150)T(151)T(152)T(153)T(154)T(155)T(156)T(157)T(158)T(159) \
  T(160)T(161)T(162)T(163)T(164)T(165)T(166)T(167)T(168)T(169)T(170)T(171)T(172)T(173)T(174)T(175)T(176)T(177)T(178)T(179)T(180)T(181)T(182)T(183)T(184)T(185)T(186)T(187)T(188)T(189)T(190)T(191) \
  T(192)T(193)T(194)T(195)T(196)T(197)T(198)T(199)T(200)T(201)T(202)T(203)T(204)T(205)T(206)T(207)T(208)T(209)T(210)T(211)T(212)T(213)T(214)T(215)T(216)T(217)T(218)T(219)T(220)T(221)T(222)T(223) \
  T(224)T(225)T(226)T(227)T(228)T(229)T(230)T(231)T(232)T(233)T(234)T(235)T(236)T(237)T(238)T(239)T(240)T(241)T(242)T(243)T(244)T(245)T(246)T(247)T(248)T(249)T(250)T(251)T(252)T(253)T(254)T(255)

template <auto... V>
NAMEOF_DETAIL_CONSTEVAL auto nb() noexcept {
  return NAMEOF_DETAIL_SIGNATURE;
}

template <std::size_t N>
struct signature_names_t {
  std::array<string_view, N> names{};
  std::size_t count = 0;
  bool valid = false;
};

NAMEOF_DETAIL_CONSTEVAL bool same_prefix(const char* str, std::size_t size, std::size_t a, std::size_t b, std::size_t n) noexcept {
  if (b > size || n > size - b) {
    return false;
  }
#if (defined(__clang__) && __clang_major__ >= 9) || (defined(_MSC_VER) && _MSC_VER >= 1929) || (defined(__GNUC__) && !defined(__clang__) && __GNUC__ >= 13)
  return __builtin_memcmp(str + a, str + b, n) == 0;
#else
  for (std::size_t i = 0; i < n; ++i) {
    if (str[a + i] != str[b + i]) {
      return false;
    }
  }
  return true;
#endif
}

// Split the top-level commas of GCC's `{...}`, Clang's `<...>` or MSVC's unclosed `nb<...`, e.g. `Host<int, void>::E::A, (E)2`. Any mismatch rejects the entire list.
template <std::size_t N>
NAMEOF_DETAIL_CONSTEVAL auto signature_names(string_view signature) noexcept {
  signature_names_t<N> result;
  const char* const str = signature.data();
  // The list follows `V = {`, `V = <` or `nb<`, so a found begin is never zero.
  std::size_t begin = 0, end = signature.size();
  for (std::size_t i = 0; begin == 0 && i < end; ++i) {
    if (end - i >= 5 && str[i] == 'V' && str[i + 1] == ' ' && str[i + 2] == '=' && str[i + 3] == ' ' &&
        (str[i + 4] == '{' || str[i + 4] == '<')) {
      if (str[end - 1] != (str[i + 4] == '{' ? '}' : '>')) {
        return signature_names_t<N>{};
      }
      begin = i + 5;
      --end;
    } else if (end - i >= 3 && str[i] == 'n' && str[i + 1] == 'b' && str[i + 2] == '<') {
      begin = i + 3;
    }
  }
  if (begin == 0) {
    return result;
  }
  std::size_t item = 0, cast = 0, cast_size = 0;
  // Each push overwrites its slot and depth resets per item, so the stack is initialised only once.
  char closes[64]{};
  while (begin < end) {
    while (begin < end && str[begin] == ' ') {
      ++begin;
    }
    if (begin == end || item == N) {
      return signature_names_t<N>{};
    }
    // Unnamed values are numeric casts (`(E)-2`, MSVC `(enum E)0x2`) that repeat their type, so a known prefix up to ')' is compared instead of parsed.
    if (cast_size != 0 && str[begin] == '(' && same_prefix(str, end, cast, begin, cast_size)) {
      auto pos = begin + cast_size;
      if (pos < end && str[pos] == '-') {
        ++pos;
      }
      const bool hex = end - pos >= 2 && str[pos] == '0' && str[pos + 1] == 'x';
      pos += hex ? 2u : 0u;
      const auto digits = pos;
      while (pos < end && ((str[pos] >= '0' && str[pos] <= '9') ||
                           (hex && ((str[pos] >= 'a' && str[pos] <= 'f') || (str[pos] >= 'A' && str[pos] <= 'F'))))) {
        ++pos;
      }
      if (pos > digits) {
        while (pos < end && str[pos] == ' ') {
          ++pos;
        }
        if (pos == end || str[pos] == ',') {
          ++item;
          begin = pos + 1;
          continue;
        }
      }
    }
    // Deeper or unfamiliar compiler spellings use the per-value scanner.
    std::size_t depth = 0, i = begin, name = begin, name_end = begin;
    for (; i < end; ++i) {
      const auto c = str[i];
      if (is_name_char(c)) {
        const auto word = i;
        // Consume an identifier once (backward revalidation exceeds constexpr limits on long names), with inline tests (is_name_char costs MSVC 5% time and 27% memory).
        while (i + 1 < end) {
          const auto next = str[i + 1];
          if (!((next >= 'a' && next <= 'z') || (next >= 'A' && next <= 'Z') ||
                (next >= '0' && next <= '9') || next == '_' || static_cast<unsigned char>(next) >= 0x80)) {
            break;
          }
          ++i;
        }
        if (depth == 0) {
          name = word;
          name_end = i + 1;
        }
        continue;
      }
      if (c == ',' && depth == 0) {
        break;
      }
      if (c == '`') {
        while (++i < end && str[i] != '\'') {}
        if (i == end) {
          return signature_names_t<N>{};
        }
      } else if (c == '"' || c == '\'') {
        // Compilers print values, so a quote always opens a literal (Clang: L'a', u8'a'), never separates digits.
        if (c == '"' && i > begin && str[i - 1] == 'R') {
          return signature_names_t<N>{}; // Raw literals are outside this parser's grammar.
        }
        const auto quote = c;
        bool closed = false;
        while (++i < end) {
          if (str[i] == '\\') {
            if (++i == end) {
              break;
            }
          } else if (str[i] == quote) {
            closed = true;
            break;
          }
        }
        if (!closed) {
          return signature_names_t<N>{};
        }
      } else if (c == '(' || c == '[' || c == '{' || c == '<') {
        if (depth == sizeof(closes)) {
          return signature_names_t<N>{};
        }
        closes[depth++] = c == '(' ? ')' : c == '[' ? ']' : c == '{' ? '}' : '>';
      } else if (c == ')' || c == ']' || c == '}' || (c == '>' && !(i > begin && str[i - 1] == '-'))) {
        if (depth == 0 || closes[--depth] != c) {
          return signature_names_t<N>{};
        }
      }
    }
    auto stop = i;
    while (stop > begin && str[stop - 1] == ' ') {
      --stop;
    }
    if (depth != 0 || stop == begin) {
      return signature_names_t<N>{};
    }
    const auto previous = name > begin ? str[name - 1] : '\0';
    // A top-level '<' or ',' cannot precede the name here, unlike in enum_name_valid.
    if (name_end == stop && name < stop && is_name_start(str[name]) &&
        (name == begin || previous == ' ' || (previous == ':' && name - begin >= 2 && str[name - 2] == ':'))) {
      result.names[item] = {str + name, stop - name};
      ++result.count;
    } else {
      // Anything else must be a cast: remember its prefix for the check above. A prefix already retried is an unrecognised expression.
      auto number = stop;
      while (number > begin && (is_name_char(str[number - 1]) || str[number - 1] == '-')) {
        --number;
      }
      if (cast == begin || str[begin] != '(' || str[number - 1] != ')') {
        return signature_names_t<N>{};
      }
      cast = begin;
      cast_size = number - begin;
      continue;
    }
    ++item;
    begin = i + 1;
  }
  // A trailing comma would be an empty last value.
  result.valid = item == N && str[end - 1] != ',';
  return result.valid ? result : signature_names_t<N>{};
}

template <typename E, typename = void>
inline constexpr bool has_fixed_underlying_v = false;

template <typename E>
inline constexpr bool has_fixed_underlying_v<E, std::void_t<decltype(E{0})>> = true;

template <typename E>
NAMEOF_DETAIL_CONSTEVAL bool batch_supported() noexcept {
#if defined(NAMEOF_DETAIL_TEST_FALLBACK) || defined(NAMEOF_DETAIL_NVCC)
  return false;
#else
  return is_nameof_enum_supported;
#endif
}

#if defined(NAMEOF_DETAIL_STRICT_ENUM_CAST)
template <typename E, auto V, typename = void>
inline constexpr bool enum_castable_v = false;

template <typename E, auto V>
inline constexpr bool enum_castable_v<E, V, std::void_t<std::integral_constant<E, static_cast<E>(V)>>> = true;

// CWG1766: Clang 20+ rejects out-of-domain unfixed values in packs, even after bit_cast.
template <typename E, int K = 1, typename U = std::underlying_type_t<E>>
NAMEOF_DETAIL_CONSTEVAL U unfixed_max() noexcept {
  if constexpr (K == std::numeric_limits<U>::digits) {
    return enum_castable_v<E, (std::numeric_limits<U>::max)()> ? (std::numeric_limits<U>::max)() : static_cast<U>((U{1} << (K - 1)) - 1);
  } else if constexpr (enum_castable_v<E, static_cast<U>((U{1} << K) - 1)>) {
    return unfixed_max<E, K + 1>();
  } else {
    return static_cast<U>((U{1} << (K - 1)) - 1);
  }
}
#endif

template <typename E, typename U = std::underlying_type_t<E>>
NAMEOF_DETAIL_CONSTEVAL bool batch_castable([[maybe_unused]] U value) noexcept {
#if defined(NAMEOF_DETAIL_STRICT_ENUM_CAST)
  if constexpr (!has_fixed_underlying_v<E>) {
    constexpr U max = unfixed_max<E>();
    if constexpr (std::is_signed_v<U> && enum_castable_v<E, static_cast<U>(-1)>) {
      return !cmp_less(value, -max - 1) && !cmp_less(max, value);
    } else {
      return !cmp_less(value, 0) && !cmp_less(max, value);
    }
  }
#endif
  return true;
}

template <typename E, int Min, bool IsFlags>
NAMEOF_DETAIL_CONSTEVAL E batch_value(std::size_t i) noexcept {
  const auto value = value_at<E, Min, IsFlags>(i);
  return batch_castable<E>(value) ? enum_cast<E>(value) : E{};
}

template <typename E, bool IsFlags, int Min, std::size_t J, std::size_t... K>
NAMEOF_DETAIL_CONSTEVAL auto block_names(std::index_sequence<K...>) noexcept {
  if constexpr (!batch_supported<E>()) {
    // MSVC 19.29 calls a non-constexpr array helper for a directly returned empty aggregate in consteval.
    signature_names_t<sizeof...(K)> result;
    return result;
#if !defined(NAMEOF_DETAIL_STRICT_ENUM_CAST) && (defined(NAMEOF_DETAIL_HAS_BIT_CAST) || defined(_MSC_VER))
  } else if constexpr (!IsFlags) {
#else
  // Unfixed enums keep batch_value: Clang 20+ needs its filter, and GCC 9/10 static_cast warns with -Wconversion.
  } else if constexpr (!IsFlags && has_fixed_underlying_v<E>) {
#endif
    // The cast is spelled in the argument itself: a batch_value call per candidate was measured 2-6% slower.
    using U = std::underlying_type_t<E>;
    return signature_names<sizeof...(K)>(nb<NAMEOF_DETAIL_ENUM_CAST(E, static_cast<U>(static_cast<int>(J + K) + Min))...>());
  } else {
    return signature_names<sizeof...(K)>(nb<batch_value<E, Min, IsFlags>(J + K)...>());
  }
}

inline constexpr std::size_t scan_block_size = 512;

// Copy these constants when consuming them: references can make MSVC emit whole signatures and blocks.
template <typename E, bool IsFlags, std::size_t Size, int Min, std::size_t J>
inline constexpr auto block_names_v = block_names<E, IsFlags, Min, J>(std::make_index_sequence<((Size - J) < scan_block_size ? Size - J : scan_block_size)>{});

template <std::size_t N>
struct valid_offsets_t {
  std::uint16_t count = 0;
  std::uint16_t offsets[N] = {};
  string_view names[N] = {};

  constexpr void append(std::size_t i, string_view name) noexcept {
    names[count] = name;
    offsets[count++] = static_cast<std::uint16_t>(i);
  }
};

template <typename E, bool IsFlags, std::size_t Size, int Min, std::size_t J, std::size_t N>
NAMEOF_DETAIL_CONSTEVAL void scan_fallback(valid_offsets_t<N>& vc) noexcept {
#define NAMEOF_ENUM_V(O)                                                                \
  if constexpr ((J + O) < Size) {                                                       \
    if constexpr (nv<enum_cast<E>(value_at<E, Min, IsFlags>(J + O))>()) {               \
      vc.append(J + O, enum_name_v<E, enum_cast<E>(value_at<E, Min, IsFlags>(J + O))>); \
    }                                                                                   \
  }

  NAMEOF_FOR_EACH_256(NAMEOF_ENUM_V)
#undef NAMEOF_ENUM_V
}

template <typename E, bool IsFlags, std::size_t Size, int Min, std::size_t J>
NAMEOF_DETAIL_CONSTEVAL auto block_offsets() noexcept {
  constexpr auto block = block_names_v<E, IsFlags, Size, Min, J>;
  constexpr auto count = block.names.size();
  constexpr auto capacity = block.valid ? (block.count > 0 ? block.count : 1) : count;
  valid_offsets_t<capacity> offsets;
  if constexpr (!block.valid) {
    scan_fallback<E, IsFlags, Size, Min, J>(offsets);
    if constexpr (count > 256) {
      scan_fallback<E, IsFlags, Size, Min, J + 256>(offsets);
    }
  } else if constexpr (block.count > 0) {
    for (std::size_t i = 0; i < count; ++i) {
      // An inadmissible pack argument was replaced with zero, so never count that synthetic position.
      if (!block.names[i].empty() && batch_castable<E>(value_at<E, Min, IsFlags>(J + i))) {
        offsets.append(J + i, block.names[i]);
      }
    }
  }
  return offsets;
}

template <typename E, bool IsFlags, std::size_t Size, int Min, std::size_t J>
inline constexpr auto block_offsets_v = block_offsets<E, IsFlags, Size, Min, J>();

template <typename E, bool IsFlags, std::size_t Size, int Min, std::size_t J = 0>
NAMEOF_DETAIL_CONSTEVAL std::size_t valid_count() noexcept {
  constexpr auto count = block_offsets_v<E, IsFlags, Size, Min, J>.count;
  if constexpr (J + scan_block_size < Size) {
    return count + valid_count<E, IsFlags, Size, Min, J + scan_block_size>();
  } else {
    return count;
  }
}

template <typename E, bool IsFlags, std::size_t Size, int Min, std::size_t J, std::size_t N>
NAMEOF_DETAIL_CONSTEVAL void append_offsets(valid_offsets_t<N>& offsets) noexcept {
  constexpr auto block = block_offsets_v<E, IsFlags, Size, Min, J>;
  if constexpr (block.count > 0) {
    for (std::size_t i = 0; i < block.count; ++i) {
      offsets.append(block.offsets[i], block.names[i]);
    }
  }
  if constexpr (J + scan_block_size < Size) {
    append_offsets<E, IsFlags, Size, Min, J + scan_block_size>(offsets);
  }
}

template <typename E, bool IsFlags, std::size_t Size, int Min>
NAMEOF_DETAIL_CONSTEVAL auto valid_offsets() noexcept {
  constexpr auto count = valid_count<E, IsFlags, Size, Min>();
  valid_offsets_t<(count > 0 ? count : 1)> offsets;
  append_offsets<E, IsFlags, Size, Min, 0>(offsets);
  return offsets;
}

template <typename E, bool IsFlags>
struct reflected_range {
  static constexpr auto min = reflected_min<E, IsFlags>();
  static constexpr auto max = reflected_max<E, IsFlags>();
  static constexpr bool valid_order = IsFlags || !cmp_less(customize::enum_range<E>::max, customize::enum_range<E>::min);
  // A valid range can be empty after clamping to the underlying type.
  static constexpr bool empty = cmp_less(max, min);
  static constexpr bool valid_bounds = !cmp_less(min, (std::numeric_limits<int>::min)()) && !cmp_less((std::numeric_limits<int>::max)(), max);
  // Unsigned subtraction avoids signed overflow.
  static constexpr auto span = static_cast<unsigned int>(max) - static_cast<unsigned int>(min);
  static constexpr bool valid_size = span < (std::numeric_limits<std::uint16_t>::max)() - 1u;
};

template <typename E, bool IsFlags>
NAMEOF_DETAIL_CONSTEVAL auto enum_offsets() noexcept {
  using R = reflected_range<E, IsFlags>;
  static_assert(R::valid_order, "nameof::customize::enum_range min must not be greater than max.");
  if constexpr (R::empty) {
    return valid_offsets_t<1>{};
  } else if constexpr (!R::valid_bounds) {
    static_assert(R::valid_bounds, "nameof::customize::enum_range bounds must fit in int.");
    return valid_offsets_t<1>{};
  } else if constexpr (!R::valid_size) {
    static_assert(R::valid_size, "nameof::customize::enum_range must contain fewer than UINT16_MAX values.");
    return valid_offsets_t<1>{};
  } else {
    return valid_offsets<E, IsFlags, R::span + 1, static_cast<int>(R::min)>();
  }
}

template <typename E, bool IsFlags>
inline constexpr auto offsets_v = enum_offsets<E, IsFlags>();

template <typename E, bool IsFlags>
NAMEOF_DETAIL_CONSTEVAL auto values() noexcept {
  constexpr auto offsets = offsets_v<E, IsFlags>;
  // Underlying values share array instantiations between enums of the same type and size.
  std::array<std::underlying_type_t<E>, offsets.count> result{};
  if constexpr (offsets.count > 0) {
    constexpr auto min = static_cast<int>(reflected_range<E, IsFlags>::min);
    for (std::size_t i = 0; i < offsets.count; ++i) {
      result[i] = value_at<E, min, IsFlags>(offsets.offsets[i]);
    }
  }
  return result;
}

template <typename E, bool IsFlags = false>
inline constexpr auto values_v = values<E, IsFlags>();

template <typename E, bool IsFlags = false>
inline constexpr auto count_v = values_v<E, IsFlags>.size();

template <typename E, bool IsFlags = false, typename U = std::underlying_type_t<E>>
inline constexpr auto min_v = (count_v<E, IsFlags> > 0) ? values_v<E, IsFlags>.front() : U{0};

template <typename E, bool IsFlags = false, typename U = std::underlying_type_t<E>>
inline constexpr auto max_v = (count_v<E, IsFlags> > 0) ? values_v<E, IsFlags>.back() : U{0};

template <typename E, bool IsFlags>
NAMEOF_DETAIL_CONSTEVAL bool valid_name_lengths() noexcept {
  constexpr auto offsets = offsets_v<E, IsFlags>;
  for (std::size_t i = 0; i < offsets.count; ++i) {
    if (offsets.names[i].size() > (std::numeric_limits<std::uint16_t>::max)()) {
      return false;
    }
  }
  return true;
}

template <typename E, bool IsFlags>
NAMEOF_DETAIL_CONSTEVAL std::size_t name_chars_size() noexcept {
  static_assert(valid_name_lengths<E, IsFlags>(), "nameof::cstring supports names of at most UINT16_MAX characters.");
  constexpr auto offsets = offsets_v<E, IsFlags>;
  std::size_t result = 0;
  for (std::size_t i = 0; i < offsets.count; ++i) {
    result += offsets.names[i].size() + 1;
  }
  return result;
}

template <typename E, bool IsFlags>
NAMEOF_DETAIL_CONSTEVAL auto name_chars() noexcept {
  constexpr auto offsets = offsets_v<E, IsFlags>;
  std::array<char, name_chars_size<E, IsFlags>()> result{};
  if constexpr (offsets.count > 0) {
    std::size_t pos = 0;
    auto* const data = result.data();
    for (std::size_t i = 0; i < offsets.count; ++i) {
      const auto name = offsets.names[i];
      const auto* const text = name.data();
      const auto size = name.size();
      for (std::size_t j = 0; j < size; ++j) {
        data[pos++] = text[j];
      }
      ++pos; // The zero-initialised byte terminates this name.
    }
  }
  return result;
}

template <typename E, bool IsFlags>
inline constexpr auto name_chars_v = name_chars<E, IsFlags>();

template <typename E, bool IsFlags>
NAMEOF_DETAIL_CONSTEVAL auto names() noexcept {
  constexpr auto offsets = offsets_v<E, IsFlags>;
  std::array<string_view, offsets.count> result{};
  if constexpr (offsets.count > 0) {
    std::size_t pos = 0;
    for (std::size_t i = 0; i < offsets.count; ++i) {
      result[i] = {name_chars_v<E, IsFlags>.data() + pos, offsets.names[i].size()};
      pos += offsets.names[i].size() + 1;
    }
  }
  return result;
}

template <typename E, bool IsFlags = false>
inline constexpr auto names_v = names<E, IsFlags>();

template <typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
NAMEOF_DETAIL_CONSTEVAL bool is_sparse() noexcept {
  if constexpr (IsFlags) {
    return true; // Flags are searched linearly.
  } else {
    constexpr auto range_size = max_v<E> - min_v<E> + U{1};
    return range_size != count_v<E>;
  }
}

template <typename E, bool IsFlags = false>
inline constexpr bool is_sparse_v = is_sparse<E, IsFlags>();

template <typename E, bool IsFlags>
NAMEOF_DETAIL_CONSTEVAL bool has_names() noexcept {
  using R = reflected_range<E, IsFlags>;
  // Invalid ranges already fail in enum_offsets() with a clearer message.
  if constexpr (count_v<E, IsFlags> > 0 || R::empty || !R::valid_bounds || !R::valid_size) {
    return true;
  } else {
    // No reflected values: accept the enum if customize::enum_name names any value in the range.
    for (std::size_t i = 0; i <= R::span; ++i) {
      if (!customize::enum_name<E>(enum_cast<E>(value_at<E, static_cast<int>(R::min), IsFlags>(i))).empty()) {
        return true;
      }
    }
    return false;
  }
}

template <typename E, bool IsFlags = false>
inline constexpr bool has_names_v = has_names<E, IsFlags>();

// Binary search in sorted values_v, count_v<E> if not found. Inlined into enum_name(E), it slowed GCC dense lookups.
template <typename E>
constexpr std::size_t sparse_index(E value) noexcept {
  using U = std::underlying_type_t<E>;
  const auto v = static_cast<U>(value);
  std::size_t lo = 0;
  std::size_t hi = count_v<E>;
  while (lo < hi) {
    const auto mid = lo + (hi - lo) / 2;
    if (values_v<E>[mid] < v) {
      lo = mid + 1;
    } else {
      hi = mid;
    }
  }
  return lo < count_v<E> && values_v<E>[lo] == v ? lo : count_v<E>;
}

#endif

template <typename E, bool IsFlags = false>
constexpr string_view enum_name(E value) noexcept {
  if (auto custom_name = customize::enum_name<E>(value); !custom_name.empty()) {
    return custom_name;
  }

#if defined(NAMEOF_DETAIL_USE_STD_REFLECTION)
  if constexpr (IsFlags) {
    using U = make_unsigned_t<std::underlying_type_t<E>>;
    // This path receives single bits. Custom names were checked before the automatic mask.
    if ((static_cast<U>(value) & reflection::flag_mask_v<E>) == U{0}) {
      return string_view{""};
    }
  }
  return reflection::enum_name(value);
#else
  if constexpr (count_v<E, IsFlags> > 0) {
    // Binary search pays off above 16 values. Flag tables are sorted by bit, not by signed value.
    if constexpr (is_sparse_v<E, IsFlags> && !IsFlags && count_v<E, IsFlags> > 16) {
      if (const auto i = sparse_index(value); i < count_v<E>) {
        return names_v<E>[i];
      }
    } else if constexpr (is_sparse_v<E, IsFlags>) {
      const auto v = static_cast<std::underlying_type_t<E>>(value);
      for (std::size_t i = 0; i < count_v<E, IsFlags>; ++i) {
        if (values_v<E, IsFlags>[i] == v) {
          return names_v<E, IsFlags>[i];
        }
      }
    } else {
      const auto v = static_cast<std::underlying_type_t<E>>(value);
      if (v >= min_v<E> && v <= max_v<E>) {
        return names_v<E>[static_cast<std::size_t>(v - min_v<E>)];
      }
    }
  }
  return string_view{""};
#endif
}

#if defined(_MSC_VER) && !defined(__clang__)
// MSVC prints a class-key before class and enum types. Wrapping every type keeps the prefix of n() fixed.
template <typename T>
struct identity {
  using type = T;
};

template <typename T>
using remove_identity_t = typename T::type;
#else
template <typename T>
using identity = T;

template <typename T>
using remove_identity_t = T;
#endif

template <typename T, typename R, typename U = remove_cvref_t<T>>
using enable_if_has_short_name_t = std::enable_if_t<!std::is_array_v<U> && !std::is_pointer_v<U> && !std::is_member_pointer_v<U> && !std::is_function_v<U>, R>;

template <typename... T>
NAMEOF_DETAIL_CONSTEVAL auto NAMEOF_DETAIL_CDECL n() noexcept {
  if constexpr (is_nameof_type_supported) {
    constexpr auto signature = NAMEOF_DETAIL_SIGNATURE;
#if defined(__clang__)
    constexpr auto prefix = sizeof("auto nameof::detail::n() [T = <") - 1;
    constexpr auto suffix = sizeof(">") - 1;
#elif defined(__GNUC__)
    // GCC spells "consteval" here in C++20, which has the same length.
    constexpr auto prefix = sizeof("constexpr auto nameof::detail::n() [with T = {") - 1;
    constexpr auto suffix = sizeof("}") - 1;
#elif defined(_MSC_VER)
    constexpr auto prefix = sizeof("auto __cdecl nameof::detail::n<struct nameof::detail::identity<") - 1;
    // MSVC puts a space before the closing '>' of a nested template.
    constexpr auto suffix = signature[signature.size() - 2] == ' ' ? sizeof(" >") - 1 : sizeof(">") - 1;
#else
    constexpr std::size_t prefix = 0;
    constexpr std::size_t suffix = 0;
#endif
    static_assert(signature.size() >= prefix + suffix, "nameof::detail::n requires a valid signature.");
    constexpr auto name = string_view{signature.data() + prefix, signature.size() - prefix - suffix};
    return to_cstring<name.size()>(name);
  } else {
    return cstring<0>{};
  }
}

template <typename... T>
NAMEOF_DETAIL_CONSTEVAL auto type_name() noexcept {
  constexpr auto custom_name = customize::type_name<remove_identity_t<T>...>();
  if constexpr (custom_name.empty()) {
    return n<T...>();
  } else {
    return to_cstring<custom_name.size()>(custom_name);
  }
}

template <typename... T>
inline constexpr auto type_name_v = type_name<T...>();

// Not a template, so short type names do not instantiate type traits per type.
constexpr string_view pretty_type_name(string_view name, bool is_qualified, bool is_null_pointer) noexcept {
  if (is_null_pointer) {
    return "nullptr_t";
  } else if (!is_qualified) {
    return name;
  }
  name = pretty_name(name);
#if defined(_MSC_VER)
  // MSVC spells unnamed types as "struct <unnamed-type-x>", which leaves only the class-key.
  if (name.size() <= 6 && (name.compare("struct") == 0 || name.compare("class") == 0 || name.compare("union") == 0 || name.compare("enum") == 0)) {
    return {};
  }
#endif
  return name;
}

template <typename T>
NAMEOF_DETAIL_CONSTEVAL auto short_type_name() noexcept {
  using U = remove_identity_t<T>;
  constexpr auto custom_name = customize::type_name<U>();
  constexpr auto name = custom_name.empty() ? pretty_type_name(type_name_v<T>, std::is_class_v<U> || std::is_union_v<U> || std::is_enum_v<U>, std::is_null_pointer_v<U>)
                                            : pretty_name(custom_name);
  static_assert(!name.empty(), "nameof::nameof_short_type requires a type with a short name.");
  return to_cstring<name.size()>(name);
}

template <typename T>
inline constexpr auto short_type_name_v = short_type_name<T>();

template <typename T>
string qualified_type_name(string name) {
  using U = std::remove_reference_t<T>;
  if constexpr (std::is_const_v<U> || std::is_volatile_v<U>) {
    string qualified_name;
    if constexpr (std::is_const_v<U>) {
      qualified_name.append("const ", 6);
    }
    if constexpr (std::is_volatile_v<U>) {
      qualified_name.append("volatile ", 9);
    }
    qualified_name.append(name);
    name = std::move(qualified_name);
  }
  if constexpr (std::is_lvalue_reference_v<T>) {
    name.append(1, '&');
  }
  if constexpr (std::is_rvalue_reference_v<T>) {
    name.append("&&", 2);
  }
  return name;
}

// typeid name, demangled if possible. Frees the buffer itself, so the header does not need <memory>.
struct demangled_name {
  explicit demangled_name(const char* tn) noexcept : name{tn != nullptr ? tn : ""} {
    NAMEOF_DETAIL_ASSERT(tn != nullptr);
#if defined(NAMEOF_DETAIL_DEMANGLE)
    int status = 0;
    buffer = tn != nullptr ? abi::__cxa_demangle(tn, nullptr, nullptr, &status) : nullptr;
    if (status == 0 && buffer != nullptr) {
      name = string_view{buffer};
    }
#endif
  }

  demangled_name(const demangled_name&) = delete;
  demangled_name& operator=(const demangled_name&) = delete;

#if defined(NAMEOF_DETAIL_DEMANGLE)
  // Avoid expanding free macros.
  ~demangled_name() { (std::free)(buffer); }
#endif

  string_view name;
#if defined(NAMEOF_DETAIL_DEMANGLE)
  char* buffer = nullptr;
#endif
};

template <typename T>
[[nodiscard]] string nameof_type_rtti(const char* tn) {
  static_assert(supported_v<is_nameof_type_rtti_supported, T>, "NAMEOF_TYPE_RTTI is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  const demangled_name demangled{tn};
  NAMEOF_DETAIL_ASSERT(!demangled.name.empty() && "Type does not have a name.");
  return {demangled.name.data(), demangled.name.size()};
}

template <typename T, enable_if_has_short_name_t<T, int> = 0>
[[nodiscard]] string nameof_short_type_rtti(const char* tn) {
  static_assert(supported_v<is_nameof_type_rtti_supported, T>, "NAMEOF_SHORT_TYPE_RTTI is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  using U = remove_cvref_t<T>;
  const demangled_name demangled{tn};
  // The dynamic type of a polymorphic object is a class, so only static arithmetic types keep the whole name.
  const auto name = pretty_type_name(demangled.name, !std::is_arithmetic_v<U>, std::is_null_pointer_v<U>);
  NAMEOF_DETAIL_ASSERT(!name.empty() && "Type does not have a short name.");
  return {name.data(), name.size()};
}

template <typename T>
[[nodiscard]] string nameof_full_type_rtti([[maybe_unused]] const char* tn) {
  static_assert(supported_v<is_nameof_type_rtti_supported, T>, "NAMEOF_FULL_TYPE_RTTI is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  using U = std::remove_reference_t<T>;
  if constexpr ((std::is_pointer_v<U> || std::is_member_pointer_v<U> || std::is_array_v<U> || std::is_function_v<U>) && is_nameof_type_supported) {
    constexpr auto name = n<identity<T>>();
    return name.str();
  } else {
    return qualified_type_name<T>(nameof_type_rtti<T>(tn));
  }
}

// Weak symbols and GCC -fno-delete-null-pointer-checks can prevent constexpr pointer comparisons.
template <auto P, typename T = std::decay_t<decltype(P)>>
inline constexpr bool is_nullptr_v = std::is_same_v<std::integral_constant<T, P>, std::integral_constant<T, static_cast<T>(nullptr)>>;

template <auto V, auto U = V>
NAMEOF_DETAIL_CONSTEVAL auto n() noexcept {
  constexpr auto custom_name = customize::member_name<V>();
  if constexpr (!custom_name.empty()) {
    return to_cstring<custom_name.size()>(custom_name);
  } else if constexpr (is_nameof_member_supported) {
#if defined(__clang__) || defined(__GNUC__)
    constexpr auto name = pretty_member_name(NAMEOF_DETAIL_SIGNATURE);
#elif defined(NAMEOF_MEMBER_SUPPORTED)
    constexpr auto signature = NAMEOF_DETAIL_SIGNATURE;
    // In the MSVC < 19.44 fallback, U is Store{...} and the name ends before its '}'.
    constexpr auto suffix = std::is_same_v<decltype(V), decltype(U)> ? 0 : sizeof("}") - 1;
    // MSVC null member pointers can parse as names.
    constexpr auto name = V == nullptr ? string_view{"nullptr"}
                        : std::is_member_function_pointer_v<decltype(U)> ? pretty_function_name(signature)
                        : pretty_name({signature.data(), signature.size() - suffix});
#else
    constexpr auto name = string_view{""};
#endif
    if constexpr (!name.empty()) {
      return to_cstring<name.size()>(name);
    } else if constexpr (is_nullptr_v<V>) {
      return cstring<sizeof("nullptr") - 1>{"nullptr"};
    } else {
      return cstring<0>{};
    }
  } else {
    return cstring<0>{};
  }
}

#if defined(__clang__) || defined(__GNUC__) || defined(_MSC_VER) && _MSC_VER >= 1944
template <auto V>
inline constexpr auto member_name_v = n<V>();
#elif defined(_MSC_VER) && defined(_MSVC_LANG) && _MSVC_LANG >= 202002L
// MSVC < 19.44 requires a fake object to name data members.
template <typename From, typename Type>
From get_base_type(Type From::*);

template <typename T>
extern T nonexist_object;

template <typename T>
struct Store {
  T v;
};

template <typename T>
Store(T) -> Store<T>;

template <auto V>
consteval auto get_member_name() noexcept {
  if constexpr (V == nullptr || std::is_member_function_pointer_v<decltype(V)>) {
    return n<V>();
  } else {
    constexpr bool is_defined = sizeof(decltype(get_base_type(V))) != 0;
    static_assert(is_defined, "nameof::nameof_member can only be used if the struct is fully defined. Use the NAMEOF macro, or separate the definition and declaration.");
    if constexpr (is_defined) {
      // __builtin_addressof instead of std::addressof, which needs <memory>.
      return n<V, Store{__builtin_addressof(nonexist_object<decltype(get_base_type(V))>.*V)}>();
    } else {
      return cstring<0>{};
    }
  }
}

template <auto V>
inline constexpr auto member_name_v = get_member_name<V>();
#else
template <auto V>
inline constexpr auto member_name_v = cstring<0>{};
#endif

template <auto V>
NAMEOF_DETAIL_CONSTEVAL auto p() noexcept {
  constexpr auto custom_name = customize::pointer_name<V>();
  if constexpr (!custom_name.empty()) {
    return to_cstring<custom_name.size()>(custom_name);
  } else if constexpr (is_nameof_pointer_supported) {
#if defined(__clang__)
    constexpr auto name = pretty_member_name(NAMEOF_DETAIL_SIGNATURE);
#elif defined(__GNUC__)
    constexpr auto signature = NAMEOF_DETAIL_SIGNATURE;
    // GCC spells pointers to variables as "(& name)".
    constexpr auto suffix = signature[signature.size() - 1] == ')' ? sizeof(")") - 1 : 0;
    constexpr auto name = pretty_member_name({signature.data(), signature.size() - suffix});
#elif defined(NAMEOF_POINTER_SUPPORTED)
    constexpr auto signature = NAMEOF_DETAIL_SIGNATURE;
    constexpr auto name = std::is_function_v<std::remove_pointer_t<decltype(V)>> ? pretty_function_name(signature) : pretty_name(signature);
#else
    constexpr auto name = string_view{""};
#endif
    if constexpr (!name.empty()) {
      return to_cstring<name.size()>(name);
    } else if constexpr (is_nullptr_v<V>) {
      return cstring<sizeof("nullptr") - 1>{"nullptr"};
    } else {
      return cstring<0>{};
    }
  } else {
    return cstring<0>{};
  }
}

template <auto V>
inline constexpr auto pointer_name_v = p<V>();

} // namespace nameof::detail

// Obtains name of enum value.
template <typename E>
[[nodiscard]] constexpr auto nameof_enum(E value) noexcept -> detail::enable_if_enum_t<E, string_view> {
  using D = detail::remove_cvref_t<E>;
  static_assert(detail::supported_v<is_nameof_enum_supported, D>, "nameof::nameof_enum is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
#if !defined(NAMEOF_DETAIL_USE_STD_REFLECTION)
  static_assert(detail::has_names_v<D>, "nameof::nameof_enum requires a defined enum with an enumerator or a custom name in the reflected range.");
#endif
  return detail::enum_name(static_cast<D>(value));
}

// Obtains name of enum value or default value if no name is available.
template <typename E>
[[nodiscard]] auto nameof_enum_or(E value, string_view default_value) -> detail::enable_if_enum_t<E, string> {
  using D = detail::remove_cvref_t<E>;
  static_assert(detail::supported_v<is_nameof_enum_supported, D>, "nameof::nameof_enum_or is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");

  // Unlike nameof_enum, enums without names fall back to the default value.
  if (auto v = detail::enum_name(static_cast<D>(value)); !v.empty()) {
    return string{v.data(), v.size()};
  }
  return string{default_value.data(), default_value.size()};
}

// Obtains name of enum flag value.
template <typename E>
[[nodiscard]] auto nameof_enum_flag(E value, char sep = '|') -> detail::enable_if_enum_t<E, string> {
  using D = detail::remove_cvref_t<E>;
  using U = detail::make_unsigned_t<std::underlying_type_t<D>>;
  static_assert(detail::supported_v<is_nameof_enum_supported, D>, "nameof::nameof_enum_flag is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
#if !defined(NAMEOF_DETAIL_USE_STD_REFLECTION)
  static_assert(detail::has_names_v<D, true>, "nameof::nameof_enum_flag requires a defined enum with a single-bit enumerator or a custom flag name.");
#endif

  string name;
  auto bits = static_cast<U>(value);
#if defined(NAMEOF_DETAIL_USE_STD_REFLECTION)
  // The early zero return helps only reflection: the scanner is faster with its while loop.
  if (bits == U{0}) {
    return name;
  }
  do {
#else
  while (bits != U{0}) {
#endif
    const auto flag = static_cast<U>(bits & static_cast<U>(U{0} - bits));
    const auto flag_name = detail::enum_name<D, true>(static_cast<D>(flag));
    if (flag_name.empty()) {
      return {};
    }
    if (!name.empty()) {
      name.append(1, sep);
    }
    name.append(flag_name.data(), flag_name.size());
    bits = static_cast<U>(bits ^ flag);
#if defined(NAMEOF_DETAIL_USE_STD_REFLECTION)
  } while (bits != U{0});
#else
  }
#endif
  return name;
}

// Obtains name of enum value known at compile time. Lower compile-time cost, not restricted by the enum_range limitation.
template <auto V, detail::enable_if_enum_t<decltype(V), int> = 0>
[[nodiscard]] constexpr const auto& nameof_enum() noexcept {
  using D = decltype(V);
  static_assert(detail::supported_v<is_nameof_enum_supported, D>, "nameof::nameof_enum is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  if constexpr (customize::enum_name<D>(V).empty()) {
    return detail::enum_name_v<D, V>;
  } else {
    return detail::custom_enum_name_v<D, V>;
  }
}

// Obtains type name. Reference and cv-qualifiers are ignored.
template <typename T>
[[nodiscard]] constexpr const auto& nameof_type() noexcept {
  using U = detail::identity<detail::remove_cvref_t<T>>;
  static_assert(detail::supported_v<is_nameof_type_supported, U>, "nameof::nameof_type is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  return detail::type_name_v<U>;
}

// Obtains full type name with reference and cv-qualifiers.
template <typename T>
[[nodiscard]] constexpr const auto& nameof_full_type() noexcept {
  using U = detail::identity<T>;
  static_assert(detail::supported_v<is_nameof_type_supported, U>, "nameof::nameof_full_type is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  return detail::type_name_v<U>;
}

// Obtains short type name.
template <typename T, detail::enable_if_has_short_name_t<T, int> = 0>
[[nodiscard]] constexpr const auto& nameof_short_type() noexcept {
  using U = detail::identity<detail::remove_cvref_t<T>>;
  static_assert(detail::supported_v<is_nameof_type_supported, U>, "nameof::nameof_short_type is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  return detail::short_type_name_v<U>;
}

// Obtains name of member.
template <auto V, std::enable_if_t<std::is_member_pointer_v<decltype(V)>, int> = 0>
[[nodiscard]] constexpr const auto& nameof_member() noexcept {
  static_assert(detail::supported_v<is_nameof_member_supported, decltype(V)>, "nameof::nameof_member is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  return detail::member_name_v<V>;
}

// Obtains name of a function, a global or class static variable.
template <auto V, std::enable_if_t<std::is_pointer_v<std::decay_t<decltype(V)>>, int> = 0>
[[nodiscard]] constexpr const auto& nameof_pointer() noexcept {
  static_assert(detail::supported_v<is_nameof_pointer_supported, decltype(V)>, "nameof::nameof_pointer is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  return detail::pointer_name_v<V>;
}

} // namespace nameof

// Included here, so std::format works regardless of include order. MSVC STL warns when <format> is included before C++20.
// NAMEOF_NO_STD_FORMAT skips <format> and the formatter, which saves its parsing in every translation unit.
#if !defined(NAMEOF_NO_STD_FORMAT) && __has_include(<format>) && ((defined(_MSVC_LANG) && _MSVC_LANG >= 202002L) || __cplusplus >= 202002L)
#  include <format>
#  if defined(__cpp_lib_format) && __cpp_lib_format >= 201907L
template <std::uint16_t N>
struct std::formatter<nameof::cstring<N>, char> : std::formatter<std::string_view, char> {
  template <typename FormatContext>
  auto format(const nameof::cstring<N>& str, FormatContext& ctx) const {
    return std::formatter<std::string_view, char>::format(std::string_view{str.data(), str.size()}, ctx);
  }
};
#  endif
#endif

#if defined(FMT_VERSION)
// fmt/ranges.h treats cstring as a range, which makes its formatter ambiguous (https://github.com/fmtlib/fmt/issues/4202).
FMT_BEGIN_NAMESPACE
template <typename T, typename Char>
struct is_range;
FMT_END_NAMESPACE

template <std::uint16_t N>
struct fmt::is_range<nameof::cstring<N>, char> : std::false_type {};

template <std::uint16_t N>
struct fmt::formatter<nameof::cstring<N>> : fmt::formatter<fmt::string_view> {
  template <typename FormatContext>
  auto format(const nameof::cstring<N>& str, FormatContext& ctx) const {
    return fmt::formatter<fmt::string_view>::format(fmt::string_view{str.data(), str.size()}, ctx);
  }
};
#endif

// Obtains name of variable, function, macro.
#define NAMEOF(...) []() constexpr noexcept {                                             \
  ::std::void_t<decltype(__VA_ARGS__)>();                                                 \
  constexpr auto nameof_detail_name = ::nameof::detail::pretty_name(#__VA_ARGS__);        \
  static_assert(!nameof_detail_name.empty(), "NAMEOF requires a named expression.");      \
  return ::nameof::detail::to_cstring<nameof_detail_name.size()>(nameof_detail_name); }()

// Obtains full name of variable, function, macro.
#define NAMEOF_FULL(...) []() constexpr noexcept {                                        \
  ::std::void_t<decltype(__VA_ARGS__)>();                                                 \
  constexpr auto nameof_detail_name = ::nameof::detail::pretty_name(#__VA_ARGS__, false); \
  static_assert(!nameof_detail_name.empty(), "NAMEOF_FULL requires a named expression."); \
  return ::nameof::detail::to_cstring<nameof_detail_name.size()>(nameof_detail_name); }()

// Obtains raw expression text.
#define NAMEOF_RAW(...) []() constexpr noexcept {                                         \
  ::std::void_t<decltype(__VA_ARGS__)>();                                                 \
  constexpr auto nameof_detail_name = ::nameof::string_view{#__VA_ARGS__};                \
  return ::nameof::detail::to_cstring<nameof_detail_name.size()>(nameof_detail_name); }()

// Obtains name of enum value.
#define NAMEOF_ENUM(...) ::nameof::nameof_enum(__VA_ARGS__)

// Obtains name of enum value or default value if no name is available.
#define NAMEOF_ENUM_OR(...) ::nameof::nameof_enum_or(__VA_ARGS__)

// Obtains name of enum value known at compile time. Lower compile-time cost, not restricted by the enum_range limitation.
#define NAMEOF_ENUM_CONST(...) ::nameof::nameof_enum<__VA_ARGS__>()

// Obtains name of enum flag value.
#define NAMEOF_ENUM_FLAG(...) ::nameof::nameof_enum_flag(__VA_ARGS__)

// Obtains type name. Reference and cv-qualifiers are ignored.
#define NAMEOF_TYPE(...) ::nameof::nameof_type<__VA_ARGS__>()

// Obtains full type name with reference and cv-qualifiers.
#define NAMEOF_FULL_TYPE(...) ::nameof::nameof_full_type<__VA_ARGS__>()

// Obtains short type name.
#define NAMEOF_SHORT_TYPE(...) ::nameof::nameof_short_type<__VA_ARGS__>()

// Obtains type name of expression. Reference and cv-qualifiers are ignored.
#define NAMEOF_TYPE_EXPR(...) ::nameof::nameof_type<decltype(__VA_ARGS__)>()

// Obtains full type name of expression with reference and cv-qualifiers.
#define NAMEOF_FULL_TYPE_EXPR(...) ::nameof::nameof_full_type<decltype(__VA_ARGS__)>()

// Obtains short type name of expression.
#define NAMEOF_SHORT_TYPE_EXPR(...) ::nameof::nameof_short_type<decltype(__VA_ARGS__)>()

// Obtains type name using RTTI.
#define NAMEOF_TYPE_RTTI(...) ::nameof::detail::nameof_type_rtti<::std::void_t<decltype(__VA_ARGS__)>>(typeid(__VA_ARGS__).name())

// Obtains full type name using RTTI.
#define NAMEOF_FULL_TYPE_RTTI(...) ::nameof::detail::nameof_full_type_rtti<decltype(__VA_ARGS__)>(typeid(__VA_ARGS__).name())

// Obtains short type name using RTTI.
#define NAMEOF_SHORT_TYPE_RTTI(...) ::nameof::detail::nameof_short_type_rtti<decltype(__VA_ARGS__)>(typeid(__VA_ARGS__).name())

// Obtains name of member.
#define NAMEOF_MEMBER(...) ::nameof::nameof_member<__VA_ARGS__>()

// Obtains name of a function, a global or class static variable.
#define NAMEOF_POINTER(...) ::nameof::nameof_pointer<__VA_ARGS__>()

#undef NAMEOF_FOR_EACH_256
#undef NAMEOF_DETAIL_ASSERT
#undef NAMEOF_DETAIL_DEMANGLE
#undef NAMEOF_DETAIL_CDECL
#undef NAMEOF_DETAIL_TEST_FALLBACK
#undef NAMEOF_DETAIL_SIGNATURE
#undef NAMEOF_DETAIL_CONSTEVAL
#undef NAMEOF_DETAIL_USE_STD_REFLECTION
#undef NAMEOF_DETAIL_HAS_BIT_CAST
#undef NAMEOF_DETAIL_ENUM_CAST
#undef NAMEOF_DETAIL_STRICT_ENUM_CAST
#undef NAMEOF_DETAIL_NVCC

#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(pop)
#elif defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(__GNUC__) && __GNUC__ >= 14
#  pragma GCC diagnostic pop
#endif

#endif // NEARGYE_NAMEOF_HPP
