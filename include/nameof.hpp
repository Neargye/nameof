//  _   _                             __    _____
// | \ | |                           / _|  / ____|_     _
// |  \| | __ _ _ __ ___   ___  ___ | |_  | |   _| |_ _| |_
// | . ` |/ _` | '_ ` _ \ / _ \/ _ \|  _| | |  |_   _|_   _|
// | |\  | (_| | | | | | |  __/ (_) | |   | |____|_|   |_|
// |_| \_|\__,_|_| |_| |_|\___|\___/|_|    \_____|
// https://github.com/Neargye/nameof
// version 0.10.6
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
#define NAMEOF_VERSION_MINOR 10
#define NAMEOF_VERSION_PATCH 6

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <iterator>
#include <limits>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#if !defined(NAMEOF_FORCE_COMPILER_SPECIFIC_REFLECTION) && defined(__cpp_impl_reflection) && __cpp_impl_reflection >= 202506L && defined(__cpp_expansion_statements) && __cpp_expansion_statements >= 202506L
#  if __has_include(<meta>)
#    include <meta>
#  endif
#  if defined(__cpp_lib_reflection) && __cpp_lib_reflection >= 202506L && defined(__cpp_lib_define_static) && __cpp_lib_define_static >= 202506L
#    define NAMEOF_DETAIL_USE_STD_REFLECTION 1
#  endif
#endif

#if __has_include(<cxxabi.h>)
#  include <cxxabi.h>
#  include <cstdlib>
#endif

#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(push)
#  pragma warning(disable : 4514)  // Unreferenced inline function has been removed.
#  pragma warning(disable : 6326)  // Code analysis false positive for constant comparisons in consteval helpers.
#  pragma warning(disable : 28020) // Code analysis false positive for bounds-checked enum value arrays.
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
#endif

// Checks nameof_member and nameof_pointer compiler compatibility.
#if defined(__clang__) && __clang_major__ >= 5 || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER) && _MSVC_LANG >= 202002L
#  define NAMEOF_MEMBER_SUPPORTED 1
#  define NAMEOF_POINTER_SUPPORTED 1
#endif

// Checks nameof_enum and enum aliases compiler compatibility.
#if defined(NAMEOF_DETAIL_USE_STD_REFLECTION) || defined(__clang__) && __clang_major__ >= 5 || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER) && _MSC_VER >= 1920
#  define NAMEOF_ENUM_SUPPORTED 1
#  define NAMEOF_ENUM_SUPPORTED_ALIASES 1
#endif

// Enum value must be greater than or equal to NAMEOF_ENUM_RANGE_MIN. By default, NAMEOF_ENUM_RANGE_MIN = -128.
// If you need another default minimum for all enum types, redefine the macro NAMEOF_ENUM_RANGE_MIN.
#if !defined(NAMEOF_ENUM_RANGE_MIN)
#  define NAMEOF_ENUM_RANGE_MIN -128
#endif

// Enum value must be less than or equal to NAMEOF_ENUM_RANGE_MAX. By default, NAMEOF_ENUM_RANGE_MAX = 127.
// If you need another default maximum for all enum types, redefine the macro NAMEOF_ENUM_RANGE_MAX.
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

  constexpr explicit cstring(string_view str) noexcept : cstring{check_size(str), std::make_integer_sequence<std::uint16_t, N>{}} {}

  template <bool Empty = (N == 0), std::enable_if_t<Empty, int> = 0>
  constexpr cstring() noexcept : chars_{} {}

  constexpr cstring(const cstring&) = default;

  constexpr cstring(cstring&&) = default;

  ~cstring() = default;

  cstring& operator=(const cstring&) = default;

  cstring& operator=(cstring&&) = default;

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

  [[nodiscard]] constexpr const_reference operator[](size_type i) const noexcept { return assert(i < size()), data()[i]; }

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
  [[nodiscard]] static constexpr string_view check_size(string_view str) noexcept { return assert(str.size() == N), str; }

  template <std::uint16_t... J>
  constexpr cstring([[maybe_unused]] string_view str, std::integer_sequence<std::uint16_t, J...>) noexcept : chars_{str[J]..., '\0'} {}

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

// Find the start of trailing groups and preceding spaces, or npos if unbalanced: " (a)(b)" in "f<int> (a)(b)" for '(' and ')'.
// One backward pass: suffix_depth counts open/close groups, nested_depth counts other brackets inside them.
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

// The last identifier of an expression or a qualified name: "ns::S<int>::f<int>(x)" -> "f", or "f<int>" without remove_suffix.
// Empty for literals, numbers and other expressions without a name.
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

// Operators have no name, but pretty_name yields "operator" for "S::operator()", "__conv_op" for GCC conversion operators
// and the token after the keyword for "S::operator new", Clang "S::operator int" and "operator\"\"_x".
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

// MSVC spells function pointers as declarations: p<&void __cdecl f(void)>, p<void __cdecl ns::g<int>(void)>.
// The name is the first identifier after ' ' or "::" that is followed by its parameters, possibly after template arguments.
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

template <typename T, typename R>
using enable_if_enum_t = std::enable_if_t<std::is_enum_v<remove_cvref_t<T>>, R>;

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
    static_assert(supported_v<false, E>, "nameof requires a complete enum definition.");
    return std::array<std::meta::info, 0>{};
  }
}

template <typename E>
inline constexpr auto enumerators_v = enumerators<E>();

template <typename E>
constexpr auto enum_name([[maybe_unused]] E value) noexcept {
  template for (constexpr auto enumerator : enumerators_v<E>) {
    constexpr E candidate = std::meta::extract<E>(enumerator);
    if (enum_value_equal(candidate, value)) {
      constexpr auto identifier = std::meta::identifier_of(enumerator);
      constexpr auto persistent_name = std::define_static_string(identifier);
      return string_view{persistent_name, identifier.size()};
    }
  }
  return string_view{""};
}

} // namespace reflection

#endif

// The enumerator is the last identifier, after "::" (V = Color::RED, MSVC nv<Color::RED>), ' ' (V = BLUE),
// or on MSVC '<' (nv<BLUE>) and ',' (enum_name<enum E,BLUE>). Unnamed values are casts: (Color)5, MSVC (enum Color)0x5.
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
    constexpr auto name = reflection::enum_name(V);
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
#if defined(NAMEOF_DETAIL_HAS_BIT_CAST)
  // Clang 16+ rejects out-of-range static_casts to enums without a fixed underlying type in constant expressions:
  // https://reviews.llvm.org/D130058, https://reviews.llvm.org/D131307
  return __builtin_bit_cast(E, value);
#else
  return static_cast<E>(value);
#endif
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
NAMEOF_DETAIL_CONSTEVAL bool cmp_less(L lhs, R rhs) noexcept {
  static_assert(std::is_integral_v<L> && std::is_integral_v<R>, "nameof::detail::cmp_less requires integral types.");

  if constexpr (std::is_same_v<L, bool>) {
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

template <std::size_t N>
struct valid_offsets_t {
  std::uint16_t count = 0;
  std::uint16_t offsets[N] = {};

  constexpr void append(std::size_t i) noexcept {
    offsets[count++] = static_cast<std::uint16_t>(i);
  }
};

template <typename E, bool IsFlags, std::size_t Size, int Min, std::size_t J>
NAMEOF_DETAIL_CONSTEVAL void valid_offsets(valid_offsets_t<Size>& vc) noexcept {
#define NAMEOF_ENUM_V(O)                                                  \
  if constexpr ((J + O) < Size) {                                         \
    if constexpr (nv<enum_cast<E>(value_at<E, Min, IsFlags>(J + O))>()) { \
      vc.append(J + O);                                                   \
    }                                                                     \
  }

  NAMEOF_FOR_EACH_256(NAMEOF_ENUM_V)

  if constexpr ((J + 256) < Size) {
    valid_offsets<E, IsFlags, Size, Min, J + 256>(vc);
  }
#undef NAMEOF_ENUM_V
}

template <typename E, bool IsFlags, std::size_t Size, int Min>
NAMEOF_DETAIL_CONSTEVAL auto valid_offsets() noexcept {
  valid_offsets_t<Size> vc;
  valid_offsets<E, IsFlags, Size, Min, 0>(vc);
  return vc;
}

template <typename E, bool IsFlags, std::size_t Size, int Min>
NAMEOF_DETAIL_CONSTEVAL auto values() noexcept {
  constexpr auto vc = valid_offsets<E, IsFlags, Size, Min>();
  if constexpr (vc.count > 0) {
    std::array<E, vc.count> values = {};
    for (std::size_t i = 0; i < vc.count; ++i) {
      values[i] = static_cast<E>(value_at<E, Min, IsFlags>(vc.offsets[i]));
    }
    return values;
  } else {
    return std::array<E, 0>{};
  }
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
NAMEOF_DETAIL_CONSTEVAL auto values() noexcept {
  using R = reflected_range<E, IsFlags>;
  static_assert(R::valid_order, "nameof::customize::enum_range must contain at least one value.");
  if constexpr (R::empty) {
    return std::array<E, 0>{};
  } else if constexpr (!R::valid_bounds) {
    static_assert(R::valid_bounds, "nameof::customize::enum_range bounds must fit in int.");
    return std::array<E, 0>{};
  } else if constexpr (!R::valid_size) {
    static_assert(R::valid_size, "nameof::customize::enum_range must contain fewer than UINT16_MAX values.");
    return std::array<E, 0>{};
  } else {
    return values<E, IsFlags, R::span + 1, static_cast<int>(R::min)>();
  }
}

template <typename E, bool IsFlags = false>
inline constexpr auto values_v = values<E, IsFlags>();

template <typename E, bool IsFlags = false>
inline constexpr auto count_v = values_v<E, IsFlags>.size();

template <typename E, bool IsFlags = false, typename U = std::underlying_type_t<E>>
inline constexpr auto min_v = (count_v<E, IsFlags> > 0) ? static_cast<U>(values_v<E, IsFlags>.front()) : U{0};

template <typename E, bool IsFlags = false, typename U = std::underlying_type_t<E>>
inline constexpr auto max_v = (count_v<E, IsFlags> > 0) ? static_cast<U>(values_v<E, IsFlags>.back()) : U{0};

template <typename E, bool IsFlags, std::size_t... J>
NAMEOF_DETAIL_CONSTEVAL auto names(std::index_sequence<J...>) noexcept {
  return std::array<string_view, sizeof...(J)>{{enum_name_v<E, values_v<E, IsFlags>[J]>...}};
}

template <typename E, bool IsFlags = false>
inline constexpr auto names_v = names<E, IsFlags>(std::make_index_sequence<count_v<E, IsFlags>>{});

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
  // Invalid ranges already fail in values() with a clearer message.
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

#endif

template <typename E, bool IsFlags = false>
constexpr string_view enum_name(E value) noexcept {
  if (auto custom_name = customize::enum_name<E>(value); !custom_name.empty()) {
    return custom_name;
  }

#if defined(NAMEOF_DETAIL_USE_STD_REFLECTION)
  return reflection::enum_name(value);
#else
  if constexpr (count_v<E, IsFlags> > 0) {
    if constexpr (is_sparse_v<E, IsFlags>) {
      for (std::size_t i = 0; i < count_v<E, IsFlags>; ++i) {
        if (enum_value_equal(values_v<E, IsFlags>[i], value)) {
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
  static_assert(!name.empty(), "Type does not have a short name.");
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
    assert(tn != nullptr);
#if __has_include(<cxxabi.h>)
    int status = 0;
    buffer = tn != nullptr ? abi::__cxa_demangle(tn, nullptr, nullptr, &status) : nullptr;
    if (status == 0 && buffer != nullptr) {
      name = string_view{buffer};
    }
#endif
  }

  demangled_name(const demangled_name&) = delete;
  demangled_name& operator=(const demangled_name&) = delete;

#if __has_include(<cxxabi.h>)
  // Avoid expanding free macros.
  ~demangled_name() { (std::free)(buffer); }
#endif

  string_view name;
#if __has_include(<cxxabi.h>)
  char* buffer = nullptr;
#endif
};

template <typename T>
[[nodiscard]] string nameof_type_rtti(const char* tn) {
  static_assert(supported_v<is_nameof_type_rtti_supported, T>, "NAMEOF_TYPE_RTTI is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  const demangled_name demangled{tn};
  assert(!demangled.name.empty() && "Type does not have a name.");
  return {demangled.name.data(), demangled.name.size()};
}

template <typename T, enable_if_has_short_name_t<T, int> = 0>
[[nodiscard]] string nameof_short_type_rtti(const char* tn) {
  static_assert(supported_v<is_nameof_type_rtti_supported, T>, "NAMEOF_SHORT_TYPE_RTTI is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  using U = remove_cvref_t<T>;
  const demangled_name demangled{tn};
  // The dynamic type of a polymorphic object is a class, so only static arithmetic types keep the whole name.
  const auto name = pretty_type_name(demangled.name, !std::is_arithmetic_v<U>, std::is_null_pointer_v<U>);
  assert(!name.empty() && "Type does not have a short name.");
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
#elif defined(_MSC_VER) && defined(_MSVC_LANG) && _MSVC_LANG >= 202002L
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
#elif defined(_MSC_VER) && defined(_MSVC_LANG) && _MSVC_LANG >= 202002L
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
  while (bits != U{0}) {
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
  }
  return name;
}

// Obtains name of enum value known at compile time.
// This version has a lower compile-time cost and is not restricted by the enum_range limitation.
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
#if __has_include(<format>) && ((defined(_MSVC_LANG) && _MSVC_LANG >= 202002L) || __cplusplus >= 202002L)
#  include <format>
#  if defined(__cpp_lib_format) && __cpp_lib_format >= 201907L
template <std::uint16_t N>
struct std::formatter<nameof::cstring<N>, char> : std::formatter<std::string_view, char> {
  template <typename FormatContext>
  auto format(const nameof::cstring<N>& value, FormatContext& ctx) const {
    return std::formatter<std::string_view, char>::format(std::string_view{value.data(), value.size()}, ctx);
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
  auto format(const nameof::cstring<N>& value, FormatContext& ctx) const {
    return fmt::formatter<fmt::string_view>::format(fmt::string_view{value.data(), value.size()}, ctx);
  }
};
#endif

// Obtains name of variable, function, macro.
#define NAMEOF(...) []() constexpr noexcept {                                             \
  ::std::void_t<decltype(__VA_ARGS__)>();                                                 \
  constexpr auto nameof_detail_name = ::nameof::detail::pretty_name(#__VA_ARGS__);        \
  static_assert(!nameof_detail_name.empty(), "Expression does not have a name.");         \
  return ::nameof::detail::to_cstring<nameof_detail_name.size()>(nameof_detail_name); }()

// Obtains full name of variable, function, macro.
#define NAMEOF_FULL(...) []() constexpr noexcept {                                        \
  ::std::void_t<decltype(__VA_ARGS__)>();                                                 \
  constexpr auto nameof_detail_name = ::nameof::detail::pretty_name(#__VA_ARGS__, false); \
  static_assert(!nameof_detail_name.empty(), "Expression does not have a name.");         \
  return ::nameof::detail::to_cstring<nameof_detail_name.size()>(nameof_detail_name); }()

// Obtains raw expression text.
#define NAMEOF_RAW(...) []() constexpr noexcept {                                         \
  ::std::void_t<decltype(__VA_ARGS__)>();                                                 \
  constexpr auto nameof_detail_name = ::nameof::string_view{#__VA_ARGS__};                \
  static_assert(!nameof_detail_name.empty(), "Expression does not have a name.");         \
  return ::nameof::detail::to_cstring<nameof_detail_name.size()>(nameof_detail_name); }()

// Obtains name of enum value.
#define NAMEOF_ENUM(...) ::nameof::nameof_enum(__VA_ARGS__)

// Obtains name of enum value or default value if no name is available.
#define NAMEOF_ENUM_OR(...) ::nameof::nameof_enum_or(__VA_ARGS__)

// Obtains name of enum value known at compile time.
// This version has a lower compile-time cost and is not restricted by the enum_range limitation.
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
#undef NAMEOF_DETAIL_CDECL
#undef NAMEOF_DETAIL_SIGNATURE
#undef NAMEOF_DETAIL_CONSTEVAL
#undef NAMEOF_DETAIL_USE_STD_REFLECTION
#undef NAMEOF_DETAIL_HAS_BIT_CAST

#if defined(_MSC_VER) && !defined(__clang__)
#  pragma warning(pop)
#endif

#endif // NEARGYE_NAMEOF_HPP
