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
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <utility>

#if !defined(NAMEOF_FORCE_COMPILER_SPECIFIC_REFLECTION) && defined(__cpp_impl_reflection) && __cpp_impl_reflection >= 202506L && defined(__cpp_expansion_statements) && __cpp_expansion_statements >= 202506L
#  if defined(__has_include)
#    if __has_include(<meta>)
#      include <meta>
#    endif
#  endif
#  if defined(__cpp_lib_reflection) && __cpp_lib_reflection >= 202506L && defined(__cpp_lib_define_static) && __cpp_lib_define_static >= 202506L
#    define NAMEOF_DETAIL_USE_STD_REFLECTION 1
#  endif
#endif

#if !defined(NAMEOF_USING_ALIAS_STRING)
#  include <string>
#endif
#if !defined(NAMEOF_USING_ALIAS_STRING_VIEW)
#  include <string_view>
#endif

#if __has_include(<cxxabi.h>)
#  include <cxxabi.h>
#  include <cstdlib>
#endif

#if defined(__clang__)
#  pragma clang diagnostic push
#  if __has_warning("-Wenum-constexpr-conversion")
#    pragma clang diagnostic ignored "-Wenum-constexpr-conversion"
#  endif
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 28020) // Code analysis false positive for bounds-checked enum value arrays.
#  pragma warning(disable : 4514) // Unreferenced inline function has been removed.
#endif

// Checks nameof_type compiler compatibility.
#if defined(__clang__) && __clang_major__ >= 5 || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER) && _MSC_VER >= 1910
#  undef  NAMEOF_TYPE_SUPPORTED
#  define NAMEOF_TYPE_SUPPORTED 1
#endif

// Checks nameof_type_rtti compiler compatibility.
#if defined(__clang__)
#  if __has_feature(cxx_rtti)
#    undef  NAMEOF_TYPE_RTTI_SUPPORTED
#    define NAMEOF_TYPE_RTTI_SUPPORTED 1
#  endif
#elif defined(__GNUC__) && __GNUC__ >= 9
#  if defined(__GXX_RTTI)
#    undef  NAMEOF_TYPE_RTTI_SUPPORTED
#    define NAMEOF_TYPE_RTTI_SUPPORTED 1
#  endif
#elif defined(_MSC_VER)
#  if defined(_CPPRTTI)
#    undef  NAMEOF_TYPE_RTTI_SUPPORTED
#    define NAMEOF_TYPE_RTTI_SUPPORTED 1
#  endif
#endif

// Checks nameof_member compiler compatibility.
#if defined(__clang__) && __clang_major__ >= 5 || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER) && defined(_MSVC_LANG) && _MSVC_LANG >= 202002L
#  undef  NAMEOF_MEMBER_SUPPORTED
#  define NAMEOF_MEMBER_SUPPORTED 1
#endif

// Checks nameof_pointer compiler compatibility.
#if defined(__clang__) && __clang_major__ >= 5 || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER) && defined(_MSVC_LANG) && _MSVC_LANG >= 202002L
#  undef  NAMEOF_POINTER_SUPPORTED
#  define NAMEOF_POINTER_SUPPORTED 1
#endif

// Checks nameof_enum compiler compatibility.
#if defined(NAMEOF_DETAIL_USE_STD_REFLECTION) || defined(__clang__) && __clang_major__ >= 5 || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER) && _MSC_VER >= 1910
#  undef  NAMEOF_ENUM_SUPPORTED
#  define NAMEOF_ENUM_SUPPORTED 1
#endif

// Checks nameof_enum compiler aliases compatibility.
#if defined(NAMEOF_DETAIL_USE_STD_REFLECTION) || defined(__clang__) && __clang_major__ >= 5 || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER) && _MSC_VER >= 1920
#  undef  NAMEOF_ENUM_SUPPORTED_ALIASES
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

namespace customize {

// Compiler-specific enum reflection scans [min, max]. Redefine NAMEOF_ENUM_RANGE_MIN/MAX globally or specialize enum_range for a specific enum.
template <typename E>
struct enum_range {
  static_assert(std::is_enum_v<E>, "nameof::customize::enum_range requires an enum type.");
  inline static constexpr int min = NAMEOF_ENUM_RANGE_MIN;
  inline static constexpr int max = NAMEOF_ENUM_RANGE_MAX;
};

// If you need custom enum names, specialize enum_name for that enum type.
template <typename E>
constexpr string_view enum_name(E) noexcept {
  static_assert(std::is_enum_v<E>, "nameof::customize::enum_name requires an enum type.");
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
  using value_type      = const char;
  using size_type       = std::uint16_t;
  using difference_type = std::ptrdiff_t;
  using pointer         = const char*;
  using const_pointer   = const char*;
  using reference       = const char&;
  using const_reference = const char&;

  using iterator       = const char*;
  using const_iterator = const char*;

  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  constexpr explicit cstring(string_view str) noexcept : cstring{check_size(str), std::make_integer_sequence<std::uint16_t, N>{}} {}

  constexpr cstring() = delete;

  constexpr cstring(const cstring&) = default;

  constexpr cstring(cstring&&) = default;

  ~cstring() = default;

  cstring& operator=(const cstring&) = default;

  cstring& operator=(cstring&&) = default;

  [[nodiscard]] constexpr const_pointer data() const noexcept { return chars_; }

  [[nodiscard]] constexpr size_type size() const noexcept { return N; }

  [[nodiscard]] constexpr const_iterator begin() const noexcept { return data(); }

  [[nodiscard]] constexpr const_iterator end() const noexcept { return data() + size(); }

  [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return begin(); }

  [[nodiscard]] constexpr const_iterator cend() const noexcept { return end(); }

  [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator{end()}; }

  [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator{begin()}; }

  [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }

  [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept { return rend(); }

  [[nodiscard]] constexpr const_reference operator[](size_type i) const noexcept { return assert(i < size()), chars_[i]; }

  [[nodiscard]] constexpr const_reference front() const noexcept { return chars_[0]; }

  [[nodiscard]] constexpr const_reference back() const noexcept { return chars_[N - 1]; }

  [[nodiscard]] constexpr size_type length() const noexcept { return size(); }

  [[nodiscard]] constexpr bool empty() const noexcept { return false; }

  [[nodiscard]] constexpr int compare(string_view str) const noexcept { return string_view{data(), size()}.compare(str); }

  [[nodiscard]] constexpr const char* c_str() const noexcept { return data(); }

  [[nodiscard]] string str() const { return {data(), size()}; }

  [[nodiscard]] constexpr operator string_view() const& noexcept { return {data(), size()}; }

  [[nodiscard]] constexpr operator string_view() const&& noexcept = delete;

  [[nodiscard]] constexpr explicit operator const_pointer() const noexcept { return data(); }

  [[nodiscard]] explicit operator string() const { return {data(), size()}; }

 private:
  [[nodiscard]] static constexpr string_view check_size(string_view str) noexcept { return assert(str.size() == N), str; }

  template <std::uint16_t... J>
  constexpr cstring(string_view str, std::integer_sequence<std::uint16_t, J...>) noexcept : chars_{str[J]..., '\0'} {}

  char chars_[static_cast<std::size_t>(N) + 1];
};

template <>
class [[nodiscard]] cstring<0> {
 public:
  using value_type      = const char;
  using size_type       = std::uint16_t;
  using difference_type = std::ptrdiff_t;
  using pointer         = const char*;
  using const_pointer   = const char*;
  using reference       = const char&;
  using const_reference = const char&;

  using iterator       = const char*;
  using const_iterator = const char*;

  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  constexpr explicit cstring([[maybe_unused]] string_view str) noexcept { assert(str.empty()); }

  constexpr cstring() = default;

  constexpr cstring(const cstring&) = default;

  constexpr cstring(cstring&&) = default;

  ~cstring() = default;

  cstring& operator=(const cstring&) = default;

  cstring& operator=(cstring&&) = default;

  [[nodiscard]] constexpr const_pointer data() const noexcept { return chars_; }

  [[nodiscard]] constexpr size_type size() const noexcept { return 0; }

  [[nodiscard]] constexpr const_iterator begin() const noexcept { return data(); }

  [[nodiscard]] constexpr const_iterator end() const noexcept { return data() + size(); }

  [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return begin(); }

  [[nodiscard]] constexpr const_iterator cend() const noexcept { return end(); }

  [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator{end()}; }

  [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator{begin()}; }

  [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }

  [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept { return rend(); }

  [[nodiscard]] constexpr size_type length() const noexcept { return 0; }

  [[nodiscard]] constexpr bool empty() const noexcept { return true; }

  [[nodiscard]] constexpr int compare(string_view str) const noexcept { return string_view{}.compare(str); }

  [[nodiscard]] constexpr const char* c_str() const noexcept { return chars_; }

  [[nodiscard]] string str() const { return {data(), size()}; }

  [[nodiscard]] constexpr operator string_view() const& noexcept { return {data(), size()}; }

  [[nodiscard]] constexpr operator string_view() const&& noexcept = delete;

  [[nodiscard]] constexpr explicit operator const_pointer() const noexcept { return chars_; }

  [[nodiscard]] explicit operator string() const { return {data(), size()}; }

 private:
  static constexpr char chars_[1] = {};
};

template <std::uint16_t N>
[[nodiscard]] constexpr bool operator==(const cstring<N>& lhs, string_view rhs) noexcept {
  return lhs.compare(rhs) == 0;
}

template <std::uint16_t N, std::uint16_t M>
[[nodiscard]] constexpr bool operator==(const cstring<N>& lhs, const cstring<M>& rhs) noexcept {
  if constexpr (N != M) {
    return false;
  } else {
    return lhs.compare(string_view{rhs.data(), rhs.size()}) == 0;
  }
}

template <std::uint16_t N>
[[nodiscard]] constexpr bool operator==(string_view lhs, const cstring<N>& rhs) noexcept {
  return lhs.compare(rhs) == 0;
}

template <std::uint16_t N>
[[nodiscard]] constexpr bool operator!=(const cstring<N>& lhs, string_view rhs) noexcept {
  return lhs.compare(rhs) != 0;
}

template <std::uint16_t N, std::uint16_t M>
[[nodiscard]] constexpr bool operator!=(const cstring<N>& lhs, const cstring<M>& rhs) noexcept {
  return !(lhs == rhs);
}

template <std::uint16_t N>
[[nodiscard]] constexpr bool operator!=(string_view lhs, const cstring<N>& rhs) noexcept {
  return lhs.compare(rhs) != 0;
}

template <std::uint16_t N>
[[nodiscard]] constexpr bool operator>(const cstring<N>& lhs, string_view rhs) noexcept {
  return lhs.compare(rhs) > 0;
}

template <std::uint16_t N, std::uint16_t M>
[[nodiscard]] constexpr bool operator>(const cstring<N>& lhs, const cstring<M>& rhs) noexcept {
  return lhs.compare(string_view{rhs.data(), rhs.size()}) > 0;
}

template <std::uint16_t N>
[[nodiscard]] constexpr bool operator>(string_view lhs, const cstring<N>& rhs) noexcept {
  return lhs.compare(rhs) > 0;
}

template <std::uint16_t N>
[[nodiscard]] constexpr bool operator>=(const cstring<N>& lhs, string_view rhs) noexcept {
  return lhs.compare(rhs) >= 0;
}

template <std::uint16_t N, std::uint16_t M>
[[nodiscard]] constexpr bool operator>=(const cstring<N>& lhs, const cstring<M>& rhs) noexcept {
  return lhs.compare(string_view{rhs.data(), rhs.size()}) >= 0;
}

template <std::uint16_t N>
[[nodiscard]] constexpr bool operator>=(string_view lhs, const cstring<N>& rhs) noexcept {
  return lhs.compare(rhs) >= 0;
}

template <std::uint16_t N>
[[nodiscard]] constexpr bool operator<(const cstring<N>& lhs, string_view rhs) noexcept {
  return lhs.compare(rhs) < 0;
}

template <std::uint16_t N, std::uint16_t M>
[[nodiscard]] constexpr bool operator<(const cstring<N>& lhs, const cstring<M>& rhs) noexcept {
  return lhs.compare(string_view{rhs.data(), rhs.size()}) < 0;
}

template <std::uint16_t N>
[[nodiscard]] constexpr bool operator<(string_view lhs, const cstring<N>& rhs) noexcept {
  return lhs.compare(rhs) < 0;
}

template <std::uint16_t N>
[[nodiscard]] constexpr bool operator<=(const cstring<N>& lhs, string_view rhs) noexcept {
  return lhs.compare(rhs) <= 0;
}

template <std::uint16_t N, std::uint16_t M>
[[nodiscard]] constexpr bool operator<=(const cstring<N>& lhs, const cstring<M>& rhs) noexcept {
  return lhs.compare(string_view{rhs.data(), rhs.size()}) <= 0;
}

template <std::uint16_t N>
[[nodiscard]] constexpr bool operator<=(string_view lhs, const cstring<N>& rhs) noexcept {
  return lhs.compare(rhs) <= 0;
}

template <typename Char, typename Traits, std::uint16_t N>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, const cstring<N>& str) {
  for (const auto c : str) {
    os.put(c);
  }
  return os;
}

namespace detail {

constexpr bool is_name_char(char c) noexcept {
  return (c >= '0' && c <= '9') ||
         (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z') ||
         (c == '_') || static_cast<unsigned char>(c) >= 0x80;
}

constexpr bool is_name_start(char c) noexcept {
  return (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z') ||
         (c == '_') || static_cast<unsigned char>(c) >= 0x80;
}

constexpr string_view pretty_name(string_view name, bool remove_suffix = true) noexcept {
  if (name.size() >= 1 && (name[0] == '"' || name[0] == '\'')) {
    return {}; // Narrow multibyte string literal.
  } else if (name.size() >= 2 && name[0] == 'R' && (name[1] == '"' || name[1] == '\'')) {
    return {}; // Raw string literal.
  } else if (name.size() >= 2 && name[0] == 'L' && (name[1] == '"' || name[1] == '\'')) {
    return {}; // Wide string literal.
  } else if (name.size() >= 2 && name[0] == 'U' && (name[1] == '"' || name[1] == '\'')) {
    return {}; // UTF-32 encoded string literal.
  } else if (name.size() >= 2 && name[0] == 'u' && (name[1] == '"' || name[1] == '\'')) {
    return {}; // UTF-16 encoded string literal.
  } else if (name.size() >= 3 && name[0] == 'u' && name[1] == '8' && (name[2] == '"' || name[2] == '\'')) {
    return {}; // UTF-8 encoded string literal.
  } else if (name.size() >= 1 && (name[0] >= '0' && name[0] <= '9')) {
    return {}; // Invalid name.
  }

  for (std::size_t i = name.size(), h = 0, s = 0; i > 0; --i) {
    if (name[i - 1] == ')') {
      ++h;
      ++s;
      continue;
    } else if (name[i - 1] == '(') {
      if (h == 0) {
        return {};
      }
      --h;
      ++s;
      continue;
    }

    if (h == 0) {
      name.remove_suffix(s);
      break;
    } else {
      ++s;
      continue;
    }
  }

  while (!name.empty() && name[name.size() - 1] == ' ') {
    name.remove_suffix(1);
  }

  std::size_t s = 0;
  for (std::size_t i = name.size(), h = 0; i > 0; --i) {
    if (name[i - 1] == '>') {
      ++h;
      ++s;
      continue;
    } else if (name[i - 1] == '<') {
      if (h == 0) {
        return {};
      }
      --h;
      ++s;
      continue;
    }

    if (h == 0) {
      break;
    } else {
      ++s;
      continue;
    }
  }

  while (s < name.size() && name[name.size() - s - 1] == ' ') {
    ++s;
  }

  for (std::size_t i = name.size() - s; i > 0; --i) {
    if (!is_name_char(name[i - 1])) {
      name.remove_prefix(i);
      break;
    }
  }
  if (remove_suffix) {
    name.remove_suffix(s);
  }

  if (!name.empty() && is_name_start(name[0])) {
    return name;
  }

  return {}; // Invalid name.
}

#if defined(_MSC_VER) && !defined(__clang__)
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
    auto name_end = name_begin + 1;
    while (name_end < signature.size() && is_name_char(signature[name_end])) {
      ++name_end;
    }
    const auto name = string_view{signature.data() + name_begin, name_end - name_begin};
    if (name.compare("operator") == 0) {
      return {};
    }

    auto parameter_begin = name_end;
    if (parameter_begin < signature.size() && signature[parameter_begin] == '<') {
      std::size_t depth = 0;
      do {
        if (signature[parameter_begin] == '<') {
          ++depth;
        } else if (signature[parameter_begin] == '>') {
          --depth;
        }
        ++parameter_begin;
      } while (parameter_begin < signature.size() && depth > 0);

      if (depth > 0) {
        return {};
      }
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

#if !defined(NAMEOF_DETAIL_USE_STD_REFLECTION)
constexpr bool enum_name_valid(string_view name) noexcept {
#if defined(__clang__)
  constexpr auto anonymous_namespace_size = sizeof("(anonymous namespace)::") - 1;
  while (name.size() > anonymous_namespace_size &&
         name[0] == '(' &&
         name[1] == 'a' &&
         name[10] == ' ' &&
         name[20] == ')' &&
         name[21] == ':' &&
         name[22] == ':') {
    name.remove_prefix(anonymous_namespace_size);
  }
#elif defined(__GNUC__)
  constexpr auto gcc_anonymous_namespace_size = sizeof("{anonymous}::") - 1;
  while (name.size() > gcc_anonymous_namespace_size &&
         name[0] == '{' &&
         name[1] == 'a' &&
         name[10] == '}' &&
         name[11] == ':' &&
         name[12] == ':') {
    name.remove_prefix(gcc_anonymous_namespace_size);
  }
  constexpr auto unnamed_namespace_size = sizeof("<unnamed>::") - 1;
  while (name.size() > unnamed_namespace_size &&
         name[0] == '<' &&
         name[1] == 'u' &&
         name[8] == '>' &&
         name[9] == ':' &&
         name[10] == ':') {
    name.remove_prefix(unnamed_namespace_size);
  }
#elif defined(_MSC_VER)
  constexpr auto msvc_anonymous_namespace_size = sizeof("`anonymous-namespace'::") - 1;
  while (name.size() > msvc_anonymous_namespace_size &&
         name[0] == '`' &&
         name[1] == 'a' &&
         name[10] == '-' &&
         name[20] == '\'' &&
         name[21] == ':' &&
         name[22] == ':') {
    name.remove_prefix(msvc_anonymous_namespace_size);
  }
#endif

  return !name.empty() && is_name_start(name[0]);
}
#endif

template <typename T>
using make_unsigned_t = std::make_unsigned_t<std::conditional_t<std::is_same_v<T, bool>, unsigned char, T>>;

#if !defined(NAMEOF_DETAIL_USE_STD_REFLECTION)

#  if defined(__cpp_lib_array_constexpr) && __cpp_lib_array_constexpr >= 201603L
#    define NAMEOF_ARRAY_CONSTEXPR 1
#  else
template <typename T, std::size_t N, std::size_t... J>
constexpr std::array<std::remove_cv_t<T>, N> to_array(T (&a)[N], std::index_sequence<J...>) noexcept {
  return {{a[J]...}};
}
#  endif

template <typename L, typename R>
constexpr bool cmp_less(L lhs, R rhs) noexcept {
  static_assert(std::is_integral_v<L> && std::is_integral_v<R>, "nameof::detail::cmp_less requires integral types.");

  if constexpr (std::is_same_v<L, bool> && std::is_same_v<R, bool>) {
    return static_cast<unsigned char>(lhs) < static_cast<unsigned char>(rhs);
  } else if constexpr (std::is_same_v<L, bool>) {
    return static_cast<R>(lhs) < rhs;
  } else if constexpr (std::is_same_v<R, bool>) {
    return lhs < static_cast<L>(rhs);
  } else if constexpr (std::is_signed_v<L> == std::is_signed_v<R>) {
    return lhs < rhs;
  } else if constexpr (std::is_signed_v<R>) {
    using C = std::common_type_t<std::make_unsigned_t<L>, std::make_unsigned_t<R>>;
    return rhs > 0 && static_cast<C>(lhs) < static_cast<C>(rhs);
  } else {
    using C = std::common_type_t<std::make_unsigned_t<L>, std::make_unsigned_t<R>>;
    return lhs < 0 || static_cast<C>(lhs) < static_cast<C>(rhs);
  }
}

#endif

template <typename T>
struct nameof_enum_supported
#if defined(NAMEOF_ENUM_SUPPORTED) && NAMEOF_ENUM_SUPPORTED || defined(NAMEOF_ENUM_NO_CHECK_SUPPORT)
    : std::true_type {};
#else
    : std::false_type {};
#endif

template <typename T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <typename T, typename R>
using enable_if_enum_t = std::enable_if_t<std::is_enum_v<remove_cvref_t<T>>, R>;

template <typename T>
inline constexpr bool is_enum_v = std::is_enum_v<T> && std::is_same_v<T, remove_cvref_t<T>>;

template <typename E>
constexpr bool enum_value_equal(E lhs, E rhs) noexcept {
  using U = std::underlying_type_t<E>;
  return static_cast<U>(lhs) == static_cast<U>(rhs);
}

#if defined(NAMEOF_DETAIL_USE_STD_REFLECTION)

namespace reflection {

template <typename...>
inline constexpr bool always_false_v = false;

template <typename E>
consteval auto enumerators() noexcept {
  if constexpr (std::meta::is_enumerable_type(^^E)) {
    return std::define_static_array(std::meta::enumerators_of(^^E));
  } else {
    static_assert(always_false_v<E>, "nameof requires a complete enum definition.");
    return std::array<std::meta::info, 0>{};
  }
}

template <typename E>
inline constexpr auto enumerators_v = enumerators<E>();

template <auto V, typename E = remove_cvref_t<decltype(V)>>
consteval auto enum_name() noexcept {
  static_assert(std::is_enum_v<E>, "nameof::detail::reflection::enum_name requires an enum value.");

  template for (constexpr auto enumerator : enumerators_v<E>) {
    if constexpr (enum_value_equal([:enumerator:], V)) {
      constexpr auto identifier = std::meta::identifier_of(enumerator);
      return string_view{identifier.data(), identifier.size()};
    }
  }
  return string_view{};
}

template <typename E>
constexpr auto enum_name([[maybe_unused]] E value) noexcept {
  template for (constexpr auto enumerator : enumerators_v<E>) {
    constexpr E candidate = [:enumerator:];
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

template <typename E, E V>
constexpr auto n() noexcept {
  static_assert(is_enum_v<E>, "nameof::detail::n requires an enum type.");

  if constexpr (nameof_enum_supported<E>::value) {
#if defined(NAMEOF_DETAIL_USE_STD_REFLECTION)
    constexpr auto name = reflection::enum_name<V>();
#elif defined(__clang__) || defined(__GNUC__)
    constexpr auto name = pretty_name({__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2});
#elif defined(_MSC_VER)
    constexpr auto name = pretty_name({__FUNCSIG__, sizeof(__FUNCSIG__) - 17});
#else
    constexpr auto name = string_view{""};
#endif
    return name;
  } else {
    return string_view{""};
  }
}

#if !defined(NAMEOF_DETAIL_USE_STD_REFLECTION)
template <auto V>
constexpr bool nv() noexcept {
  using E = decltype(V);
  static_assert(is_enum_v<E>, "nameof::detail::nv requires an enum type.");

  if constexpr (nameof_enum_supported<E>::value) {
#if defined(__GNUC__) && !defined(__clang__)
    constexpr auto prefix = sizeof("constexpr bool nameof::detail::nv() [with auto V = ") - 1;
    static_assert(sizeof(__PRETTY_FUNCTION__) > prefix + 2, "nameof::detail::nv requires a valid __PRETTY_FUNCTION__.");
    return enum_name_valid({__PRETTY_FUNCTION__ + prefix, sizeof(__PRETTY_FUNCTION__) - prefix - 2});
#elif defined(__clang__)
    constexpr auto prefix = sizeof("bool nameof::detail::nv() [V = ") - 1;
    static_assert(sizeof(__PRETTY_FUNCTION__) > prefix + 2, "nameof::detail::nv requires a valid __PRETTY_FUNCTION__.");
    return enum_name_valid({__PRETTY_FUNCTION__ + prefix, sizeof(__PRETTY_FUNCTION__) - prefix - 2});
#elif defined(_MSC_VER)
    constexpr auto prefix = __FUNCSIG__[5] == 'c' ? sizeof("bool const __cdecl nameof::detail::nv<") - 1 : sizeof("bool __cdecl nameof::detail::nv<") - 1;
    constexpr auto suffix = sizeof(">(void) noexcept") - 1;
    static_assert(sizeof(__FUNCSIG__) > prefix + suffix + 1, "nameof::detail::nv requires a valid __FUNCSIG__.");
    return enum_name_valid({__FUNCSIG__ + prefix, sizeof(__FUNCSIG__) - prefix - suffix - 1});
#else
    return false;
#endif
  } else {
    return false;
  }
}
#endif

template <typename E, E V>
constexpr auto enum_name() noexcept {
  constexpr auto name = n<E, V>();
  return cstring<name.size()>{name};
}

template <typename E, E V>
inline constexpr auto enum_name_v = enum_name<E, V>();

template <typename E, E V>
constexpr auto custom_enum_name() noexcept {
  constexpr auto name = customize::enum_name<E>(V);
  return cstring<name.size()>{name};
}

template <typename E, E V>
inline constexpr auto custom_enum_name_v = custom_enum_name<E, V>();

#if !defined(NAMEOF_DETAIL_USE_STD_REFLECTION)
template <typename E, auto V>
constexpr bool is_valid() noexcept {
#if defined(__clang__) && __clang_major__ >= 16
  // https://reviews.llvm.org/D130058, https://reviews.llvm.org/D131307
  constexpr E v = __builtin_bit_cast(E, V);
#else
  constexpr E v = static_cast<E>(V);
#endif
  return nv<v>();
}

template <typename E, int O, bool IsFlags, typename U = std::underlying_type_t<E>>
constexpr U ualue([[maybe_unused]] std::size_t i) noexcept {
  if constexpr (IsFlags) {
    if constexpr (std::is_same_v<U, bool>) {
      return true;
    } else {
      return static_cast<U>(U{1} << static_cast<U>(static_cast<int>(i) + O));
    }
  } else {
    return static_cast<U>(static_cast<int>(i) + O);
  }
}

template <typename E, int O, bool IsFlags, typename U = std::underlying_type_t<E>>
constexpr E value(std::size_t i) noexcept {
  return static_cast<E>(ualue<E, O, IsFlags>(i));
}

template <typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
constexpr int reflected_min() noexcept {
  if constexpr (IsFlags) {
    return 0;
  } else {
    constexpr auto lhs = customize::enum_range<E>::min;
    constexpr auto rhs = (std::numeric_limits<U>::min)();

    if constexpr (cmp_less(rhs, lhs)) {
      return lhs;
    } else {
      return rhs;
    }
  }
}

template <typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
constexpr int reflected_max() noexcept {
  if constexpr (IsFlags) {
    return std::numeric_limits<U>::digits - 1;
  } else {
    constexpr auto lhs = customize::enum_range<E>::max;
    constexpr auto rhs = (std::numeric_limits<U>::max)();

    if constexpr (cmp_less(lhs, rhs)) {
      return lhs;
    } else {
      return rhs;
    }
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
struct valid_count_t {
  std::uint16_t count = 0;
  std::uint16_t offsets[N] = {};

  constexpr void set(std::size_t i) noexcept {
    offsets[count++] = static_cast<std::uint16_t>(i);
  }
};

template <typename E, bool IsFlags, std::size_t Size, int Min, std::size_t J>
constexpr void valid_count(valid_count_t<Size>& vc) noexcept {
#define NAMEOF_ENUM_V(O)                                          \
  if constexpr ((J + O) < Size) {                                 \
    if constexpr (is_valid<E, ualue<E, Min, IsFlags>(J + O)>()) { \
      vc.set(J + O);                                              \
    }                                                             \
  }

  NAMEOF_FOR_EACH_256(NAMEOF_ENUM_V)

  if constexpr ((J + 256) < Size) {
    valid_count<E, IsFlags, Size, Min, J + 256>(vc);
  }
#undef NAMEOF_ENUM_V
}

template <typename E, bool IsFlags, std::size_t Size, int Min>
constexpr auto valid_count() noexcept {
  valid_count_t<Size> vc;
  valid_count<E, IsFlags, Size, Min, 0>(vc);
  return vc;
}

template <typename E, bool IsFlags, std::size_t Size, int Min>
constexpr auto values() noexcept {
  constexpr auto vc = valid_count<E, IsFlags, Size, Min>();

  if constexpr (vc.count > 0) {
#if defined(NAMEOF_ARRAY_CONSTEXPR)
    std::array<E, vc.count> values = {};
#else
    E values[vc.count] = {};
#endif
    if constexpr (vc.count == Size) {
      for (std::size_t i = 0; i < vc.count; ++i) {
        values[i] = value<E, Min, IsFlags>(i);
      }
    } else {
      for (std::size_t i = 0; i < vc.count; ++i) {
        values[i] = value<E, Min, IsFlags>(vc.offsets[i]);
      }
    }
#if defined(NAMEOF_ARRAY_CONSTEXPR)
    return values;
#else
    return to_array(values, std::make_index_sequence<vc.count>{});
#endif
  } else {
    return std::array<E, 0>{};
  }
}

template <typename E, bool IsFlags>
constexpr auto values() noexcept {
  constexpr auto min = reflected_min<E, IsFlags>();
  constexpr auto max = reflected_max<E, IsFlags>();
  constexpr bool valid_range = min <= max;
  constexpr auto range_span = static_cast<std::uintmax_t>(max) - static_cast<std::uintmax_t>(min);
  constexpr auto max_range_size = static_cast<std::uintmax_t>((std::numeric_limits<std::uint16_t>::max)());
  constexpr bool valid_size = range_span < max_range_size - 1;
  static_assert(valid_range, "nameof::customize::enum_range must contain at least one value.");
  static_assert(!valid_range || valid_size, "nameof::customize::enum_range must contain fewer than UINT16_MAX values.");

  if constexpr (valid_range && valid_size) {
    constexpr auto range_size = static_cast<std::size_t>(range_span + 1);
    return values<E, IsFlags, range_size, min>();
  } else {
    return std::array<E, 0>{};
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
constexpr auto names(std::index_sequence<J...>) noexcept {
  constexpr auto names = std::array<string_view, sizeof...(J)>{{enum_name_v<E, values_v<E, IsFlags>[J]>...}};
  return names;
}

template <typename E, bool IsFlags = false>
inline constexpr auto names_v = names<E, IsFlags>(std::make_index_sequence<count_v<E, IsFlags>>{});

template <typename E, typename U = std::underlying_type_t<E>>
constexpr bool is_sparse() noexcept {
  if constexpr (count_v<E> == 0) {
    return false;
  } else if constexpr (std::is_same_v<U, bool>) { // bool special case
    return false;
  } else {
    constexpr auto range_size = max_v<E> - min_v<E> + U{1};
    return range_size != count_v<E>;
  }
}

template <typename E>
inline constexpr bool is_sparse_v = is_sparse<E>();

template <typename E, typename U = make_unsigned_t<std::underlying_type_t<E>>>
constexpr U flags_mask() noexcept {
  U mask = 0;
  for (const auto value : values_v<E, true>) {
    mask |= static_cast<U>(value);
  }
  return mask;
}

template <typename E>
inline constexpr auto flags_mask_v = flags_mask<E>();

#endif

template <typename E>
constexpr string_view enum_name(E value) noexcept {
  if (auto custom_name = customize::enum_name<E>(value); !custom_name.empty()) {
    return custom_name;
  }

#if defined(NAMEOF_DETAIL_USE_STD_REFLECTION)
  return reflection::enum_name(value);
#else
  using U = std::underlying_type_t<E>;
  if constexpr (count_v<E> > 0) {
    if constexpr (is_sparse_v<E>) {
      for (std::size_t i = 0; i < count_v<E>; ++i) {
        if (enum_value_equal(values_v<E>[i], value)) {
          return names_v<E>[i];
        }
      }
    } else {
      const auto v = static_cast<U>(value);
      if (v >= min_v<E> && v <= max_v<E>) {
        return names_v<E>[static_cast<std::size_t>(v - min_v<E>)];
      }
    }
  }
  return string_view{""};
#endif
}

template <typename E>
constexpr string_view enum_flag_name(E value) noexcept {
  if (auto custom_name = customize::enum_name<E>(value); !custom_name.empty()) {
    return custom_name;
  }

#if defined(NAMEOF_DETAIL_USE_STD_REFLECTION)
  return reflection::enum_name(value);
#else
  using U = make_unsigned_t<std::underlying_type_t<E>>;
  const auto flag = static_cast<U>(value);
  if ((flags_mask_v<E> & flag) == U{0}) {
    return {};
  }
  for (std::size_t i = 0; i < count_v<E, true>; ++i) {
    if (static_cast<U>(values_v<E, true>[i]) == flag) {
      return names_v<E, true>[i];
    }
  }
  return {};
#endif
}

template <typename... T>
struct nameof_type_supported
#if defined(NAMEOF_TYPE_SUPPORTED) && NAMEOF_TYPE_SUPPORTED || defined(NAMEOF_TYPE_NO_CHECK_SUPPORT)
    : std::true_type {};
#else
    : std::false_type {};
#endif

template <typename... T>
struct nameof_type_rtti_supported
#if defined(NAMEOF_TYPE_RTTI_SUPPORTED) && NAMEOF_TYPE_RTTI_SUPPORTED || defined(NAMEOF_TYPE_NO_CHECK_SUPPORT)
    : std::true_type {};
#else
    : std::false_type {};
#endif

template <typename... T>
struct nameof_member_supported
#if defined(NAMEOF_MEMBER_SUPPORTED) && NAMEOF_MEMBER_SUPPORTED || defined(NAMEOF_TYPE_NO_CHECK_SUPPORT)
    : std::true_type {};
#else
    : std::false_type {};
#endif

template <typename... T>
struct nameof_pointer_supported
#if defined(NAMEOF_POINTER_SUPPORTED) && NAMEOF_POINTER_SUPPORTED || defined(NAMEOF_TYPE_NO_CHECK_SUPPORT)
    : std::true_type {};
#else
    : std::false_type {};
#endif

#if defined(_MSC_VER) && !defined(__clang__)
template <typename T>
struct identity {
  using type = T;
};
#else
template <typename T>
using identity = T;
#endif

template <typename T, typename R>
using enable_if_has_short_name_t = std::enable_if_t<!std::is_array_v<remove_cvref_t<T>> && !std::is_pointer_v<remove_cvref_t<T>>, R>;

template <typename... T>
constexpr auto n() noexcept {
#if defined(_MSC_VER) && !defined(__clang__)
  [[maybe_unused]] constexpr auto custom_name = customize::type_name<typename T::type...>();
#else
  [[maybe_unused]] constexpr auto custom_name = customize::type_name<T...>();
#endif

  if constexpr (custom_name.empty() && nameof_type_supported<T...>::value) {
#if defined(__clang__)
    constexpr string_view name{__PRETTY_FUNCTION__ + 31, sizeof(__PRETTY_FUNCTION__) - 34};
#elif defined(__GNUC__)
    constexpr string_view name{__PRETTY_FUNCTION__ + 46, sizeof(__PRETTY_FUNCTION__) - 49};
#elif defined(_MSC_VER)
    constexpr string_view name{__FUNCSIG__ + 63, sizeof(__FUNCSIG__) - 81 - (__FUNCSIG__[sizeof(__FUNCSIG__) - 19] == ' ' ? 1 : 0)};
#else
    constexpr auto name = string_view{""};
#endif
    return cstring<name.size()>{name};
  } else {
    return cstring<custom_name.size()>{custom_name};
  }
}

template <typename... T>
inline constexpr auto type_name_v = n<T...>();

template <typename T>
constexpr auto short_type_name() noexcept {
  constexpr auto name = pretty_name(type_name_v<T>);
  static_assert(!name.empty(), "Type does not have a short name.");
  return cstring<name.size()>{name};
}

template <typename T>
inline constexpr auto short_type_name_v = short_type_name<T>();

template <typename T>
string full_type_name(string name) {
  using U = std::remove_reference_t<T>;
  if constexpr (std::is_const_v<U> || std::is_volatile_v<U>) {
    string qualified_name;
    if constexpr (std::is_volatile_v<U>) {
      qualified_name.append("volatile ", 9);
    }
    if constexpr (std::is_const_v<U>) {
      qualified_name.append("const ", 6);
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

#if __has_include(<cxxabi.h>)
using demangle_ptr = std::unique_ptr<char, decltype(&std::free)>;

struct demangled_name {
  demangle_ptr p;
  string_view v;

  [[nodiscard]] string_view view() const noexcept { return v; }
  [[nodiscard]] bool empty() const noexcept { return v.empty(); }
  [[nodiscard]] string str() const { return {v.data(), v.size()}; }
};

inline demangled_name demangle(const char* tn) {
  assert(tn != nullptr);
  if (tn == nullptr) {
    return {demangle_ptr{nullptr, &std::free}, string_view{""}};
  }
  int status = 0;
  demangle_ptr p{abi::__cxa_demangle(tn, nullptr, nullptr, &status), &std::free};
  if (status != 0) {
    p.reset();
  }
  const auto v = p ? string_view{p.get()} : string_view{tn};
  return {std::move(p), v};
}

template <typename T>
string nameof_type_rtti(const char* tn) {
  static_assert(nameof_type_rtti_supported<T>::value, "NAMEOF_TYPE_RTTI is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  const auto name = demangle(tn);
  assert(!name.empty() && "Type does not have a name.");
  return name.str();
}

template <typename T>
string nameof_full_type_rtti(const char* tn) {
  static_assert(nameof_type_rtti_supported<T>::value, "NAMEOF_FULL_TYPE_RTTI is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  const auto name = demangle(tn);
  assert(!name.empty() && "Type does not have a name.");
  return full_type_name<T>(name.str());
}

template <typename T, enable_if_has_short_name_t<T, int> = 0>
string nameof_short_type_rtti(const char* tn) {
  static_assert(nameof_type_rtti_supported<T>::value, "NAMEOF_SHORT_TYPE_RTTI is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  const auto full_name = demangle(tn);
  const auto name = pretty_name(full_name.view());
  assert(!name.empty() && "Type does not have a short name.");
  return {name.data(), name.size()};
}
#else
template <typename T>
string nameof_type_rtti(const char* tn) {
  static_assert(nameof_type_rtti_supported<T>::value, "NAMEOF_TYPE_RTTI is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  const auto name = string_view{tn != nullptr ? tn : ""};
  assert(!name.empty() && "Type does not have a name.");
  return {name.data(), name.size()};
}

template <typename T>
string nameof_full_type_rtti(const char* tn) {
  static_assert(nameof_type_rtti_supported<T>::value, "NAMEOF_FULL_TYPE_RTTI is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  const auto name = string_view{tn != nullptr ? tn : ""};
  assert(!name.empty() && "Type does not have a name.");
  return full_type_name<T>({name.data(), name.size()});
}

template <typename T, enable_if_has_short_name_t<T, int> = 0>
string nameof_short_type_rtti(const char* tn) {
  static_assert(nameof_type_rtti_supported<T>::value, "NAMEOF_SHORT_TYPE_RTTI is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  const auto name = pretty_name(tn != nullptr ? tn : "");
  assert(!name.empty() && "Type does not have a short name.");
  return {name.data(), name.size()};
}
#endif

template <auto V, auto U = V>
constexpr auto n() noexcept {
  [[maybe_unused]] constexpr auto custom_name = customize::member_name<V>();

  if constexpr (custom_name.empty() && nameof_member_supported<decltype(V)>::value) {
#if defined(__clang__) || defined(__GNUC__)
    constexpr auto name = pretty_name({__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2});
#elif defined(_MSC_VER) && defined(_MSVC_LANG) && _MSVC_LANG >= 202002L
    constexpr auto name = std::is_member_function_pointer_v<decltype(U)>
                            ? pretty_function_name({__FUNCSIG__, sizeof(__FUNCSIG__) - 1})
                            : pretty_name({__FUNCSIG__, sizeof(__FUNCSIG__) - 18});
#else
    constexpr auto name = string_view{""};
#endif
    return cstring<name.size()>{name};
  } else {
    return cstring<custom_name.size()>{custom_name};
  }
}

#if defined(__clang__) || defined(__GNUC__)
template <auto V>
inline constexpr auto member_name_v = n<V>();
#elif defined(_MSC_VER) && defined(_MSVC_LANG) && _MSVC_LANG >= 202002L
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
      return n<V, Store{std::addressof(nonexist_object<decltype(get_base_type(V))>.*V)}>();
    } else {
      return "";
    }
  }
}

template <auto V>
inline constexpr auto member_name_v = get_member_name<V>();
#else
template <auto V>
inline constexpr auto member_name_v = cstring<0>{};
#endif

template <auto U, auto V>
struct is_same : std::false_type {};

template <auto U>
struct is_same<U, U> : std::true_type {};

template <auto P>
constexpr bool is_nullptr_v = is_same<P, static_cast<std::remove_reference_t<decltype(P)>>(nullptr)>::value;

template <auto V>
constexpr auto p() noexcept {
  [[maybe_unused]] constexpr auto custom_name = customize::pointer_name<V>().empty() && is_nullptr_v<V> ? "nullptr" : customize::pointer_name<V>();

  if constexpr (custom_name.empty() && nameof_pointer_supported<decltype(V)>::value) {
#if defined(__clang__)
    constexpr auto name = pretty_name({__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2});
#elif defined(__GNUC__)
    constexpr bool has_parenthesis = __PRETTY_FUNCTION__[sizeof(__PRETTY_FUNCTION__) - 3] == ')';
    constexpr auto name = pretty_name({__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2 - has_parenthesis});
#elif defined(_MSC_VER) && defined(_MSVC_LANG) && _MSVC_LANG >= 202002L
    constexpr auto name = std::is_function_v<std::remove_pointer_t<decltype(V)>>
                            ? pretty_function_name({__FUNCSIG__, sizeof(__FUNCSIG__) - 1})
                            : pretty_name({__FUNCSIG__, sizeof(__FUNCSIG__) - 17});
#else
    constexpr auto name = string_view{""};
#endif
    return cstring<name.size()>{name};
  } else {
    return cstring<custom_name.size()>{custom_name};
  }
}

template <auto V>
inline constexpr auto pointer_name_v = p<V>();

} // namespace nameof::detail

// Checks whether nameof_type is supported by the compiler.
inline constexpr bool is_nameof_type_supported = detail::nameof_type_supported<void>::value;

// Checks whether nameof_type_rtti is supported by the compiler.
inline constexpr bool is_nameof_type_rtti_supported = detail::nameof_type_rtti_supported<void>::value;

// Checks whether nameof_member is supported by the compiler.
inline constexpr bool is_nameof_member_supported = detail::nameof_member_supported<void>::value;

// Checks whether nameof_pointer is supported by the compiler.
inline constexpr bool is_nameof_pointer_supported = detail::nameof_pointer_supported<void>::value;

// Checks whether nameof_enum is supported by the compiler.
inline constexpr bool is_nameof_enum_supported = detail::nameof_enum_supported<void>::value;

// Obtains name of enum value.
template <typename E>
[[nodiscard]] constexpr auto nameof_enum(E value) noexcept -> detail::enable_if_enum_t<E, string_view> {
  using D = detail::remove_cvref_t<E>;
  static_assert(detail::nameof_enum_supported<D>::value, "nameof::nameof_enum is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  return detail::enum_name(static_cast<D>(value));
}

// Obtains name of enum value or default value if no name is available.
template <typename E>
[[nodiscard]] auto nameof_enum_or(E value, string_view default_value) -> detail::enable_if_enum_t<E, string> {
  using D = detail::remove_cvref_t<E>;
  static_assert(detail::nameof_enum_supported<D>::value, "nameof::nameof_enum_or is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");

  if (auto v = nameof_enum<D>(value); !v.empty()) {
    return string{v.data(), v.size()};
  }
  return string{default_value.data(), default_value.size()};
}

// Obtains name of enum flag value.
template <typename E>
[[nodiscard]] auto nameof_enum_flag(E value, char sep = '|') -> detail::enable_if_enum_t<E, string> {
  using D = detail::remove_cvref_t<E>;
  using U = detail::make_unsigned_t<std::underlying_type_t<D>>;
  static_assert(detail::nameof_enum_supported<D>::value, "nameof::nameof_enum_flag is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");

  const auto flag_value = static_cast<U>(value);
  if (flag_value == U{0}) {
    return {}; // Invalid value.
  }

  string name;
  auto remaining = flag_value;
  while (remaining != U{0}) {
    const auto flag = static_cast<U>(remaining & static_cast<U>(U{0} - remaining));
    const auto flag_name = detail::enum_flag_name(static_cast<D>(flag));
    if (flag_name.empty()) {
      return {}; // Unnamed flag.
    }
    if (!name.empty()) {
      name.append(1, sep);
    }
    name.append(flag_name.data(), flag_name.size());
    remaining = static_cast<U>(remaining ^ flag);
  }

  return name;
}

// Obtains name of enum value known at compile time.
// This version has a lower compile-time cost and is not restricted by the enum_range limitation.
template <auto V, detail::enable_if_enum_t<decltype(V), int> = 0>
[[nodiscard]] constexpr const auto& nameof_enum() noexcept {
  using D = decltype(V);
  static_assert(detail::nameof_enum_supported<D>::value, "nameof::nameof_enum is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  constexpr auto custom_name = customize::enum_name<D>(V);
  if constexpr (custom_name.empty()) {
    return detail::enum_name_v<D, V>;
  } else {
    return detail::custom_enum_name_v<D, V>;
  }
}

// Obtains type name; reference and cv-qualifiers are ignored.
template <typename T>
[[nodiscard]] constexpr const auto& nameof_type() noexcept {
  using U = detail::identity<detail::remove_cvref_t<T>>;
  static_assert(detail::nameof_type_supported<U>::value, "nameof::nameof_type is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  return detail::type_name_v<U>;
}

// Obtains full type name with reference and cv-qualifiers.
template <typename T>
[[nodiscard]] constexpr const auto& nameof_full_type() noexcept {
  using U = detail::identity<T>;
  static_assert(detail::nameof_type_supported<U>::value, "nameof::nameof_full_type is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  return detail::type_name_v<U>;
}

// Obtains short type name.
template <typename T, detail::enable_if_has_short_name_t<T, int> = 0>
[[nodiscard]] constexpr const auto& nameof_short_type() noexcept {
  using U = detail::identity<detail::remove_cvref_t<T>>;
  static_assert(detail::nameof_type_supported<U>::value, "nameof::nameof_short_type is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  return detail::short_type_name_v<U>;
}

// Obtains name of member.
template <auto V, std::enable_if_t<std::is_member_pointer_v<decltype(V)>, int> = 0>
[[nodiscard]] constexpr const auto& nameof_member() noexcept {
  using U = decltype(V);
  static_assert(detail::nameof_member_supported<U>::value, "nameof::nameof_member is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  return detail::member_name_v<V>;
}

// Obtains name of a function, a global or class static variable.
template <auto V, std::enable_if_t<std::is_pointer_v<decltype(V)>, int> = 0>
[[nodiscard]] constexpr const auto& nameof_pointer() noexcept {
  using U = decltype(V);
  static_assert(detail::nameof_pointer_supported<U>::value, "nameof::nameof_pointer is not supported by this compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  return detail::pointer_name_v<V>;
}

} // namespace nameof

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
template <std::uint16_t N>
struct fmt::formatter<nameof::cstring<N>> : fmt::formatter<fmt::string_view> {
  template <typename FormatContext>
  auto format(const nameof::cstring<N>& value, FormatContext& ctx) const {
    return fmt::formatter<fmt::string_view>::format(fmt::string_view{value.data(), value.size()}, ctx);
  }
};
#endif

// Obtains name of variable, function, macro.
#define NAMEOF(...) []() constexpr noexcept {                         \
  ::std::void_t<decltype(__VA_ARGS__)>();                             \
  constexpr auto _name = ::nameof::detail::pretty_name(#__VA_ARGS__); \
  static_assert(!_name.empty(), "Expression does not have a name.");  \
  constexpr auto _size = _name.size();                                \
  constexpr auto _nameof = ::nameof::cstring<_size>{_name};           \
  return _nameof; }()

// Obtains full name of variable, function, macro.
#define NAMEOF_FULL(...) []() constexpr noexcept {                           \
  ::std::void_t<decltype(__VA_ARGS__)>();                                    \
  constexpr auto _name = ::nameof::detail::pretty_name(#__VA_ARGS__, false); \
  static_assert(!_name.empty(), "Expression does not have a name.");         \
  constexpr auto _size = _name.size();                                       \
  constexpr auto _nameof_full = ::nameof::cstring<_size>{_name};             \
  return _nameof_full; }()

// Obtains raw expression text.
#define NAMEOF_RAW(...) []() constexpr noexcept {                    \
  ::std::void_t<decltype(__VA_ARGS__)>();                            \
  constexpr auto _name = ::nameof::string_view{#__VA_ARGS__};        \
  static_assert(!_name.empty(), "Expression does not have a name."); \
  constexpr auto _size = _name.size();                               \
  constexpr auto _nameof_raw = ::nameof::cstring<_size>{_name};      \
  return _nameof_raw; }()

// Obtains name of enum value.
#define NAMEOF_ENUM(...) ::nameof::nameof_enum(__VA_ARGS__)

// Obtains name of enum value or default value if no name is available.
#define NAMEOF_ENUM_OR(...) ::nameof::nameof_enum_or(__VA_ARGS__)

// Obtains name of enum value known at compile time.
// This version has a lower compile-time cost and is not restricted by the enum_range limitation.
#define NAMEOF_ENUM_CONST(...) ::nameof::nameof_enum<__VA_ARGS__>()

// Obtains name of enum flag value.
#define NAMEOF_ENUM_FLAG(...) ::nameof::nameof_enum_flag(__VA_ARGS__)

// Obtains type name; reference and cv-qualifiers are ignored.
#define NAMEOF_TYPE(...) ::nameof::nameof_type<__VA_ARGS__>()

// Obtains full type name with reference and cv-qualifiers.
#define NAMEOF_FULL_TYPE(...) ::nameof::nameof_full_type<__VA_ARGS__>()

// Obtains short type name.
#define NAMEOF_SHORT_TYPE(...) ::nameof::nameof_short_type<__VA_ARGS__>()

// Obtains type name of expression; reference and cv-qualifiers are ignored.
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

#undef NAMEOF_ARRAY_CONSTEXPR
#undef NAMEOF_FOR_EACH_256
#undef NAMEOF_DETAIL_USE_STD_REFLECTION

#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

#endif // NEARGYE_NAMEOF_HPP
