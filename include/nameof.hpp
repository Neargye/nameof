//  _   _                             __    _____
// | \ | |                           / _|  / ____|_     _
// |  \| | __ _ _ __ ___   ___  ___ | |_  | |   _| |_ _| |_
// | . ` |/ _` | '_ ` _ \ / _ \/ _ \|  _| | |  |_   _|_   _|
// | |\  | (_| | | | | | |  __/ (_) | |   | |____|_|   |_|
// |_| \_|\__,_|_| |_| |_|\___|\___/|_|    \_____|
// https://github.com/Neargye/nameof
// vesion 0.7.5
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2016, 2018 - 2019 Daniil Goncharov <neargye@gmail.com>.
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

#pragma once

#include <cstddef>
#include <type_traits>
#include <limits>
#include <string_view>

#if !defined(NAMEOF_ENUM_MAX_SEARCH_DEPTH)
#  define NAMEOF_ENUM_MAX_SEARCH_DEPTH 128
#endif

namespace nameof {

static_assert(NAMEOF_ENUM_MAX_SEARCH_DEPTH > 0,
              "NAMEOF_ENUM_MAX_SEARCH_DEPTH must be positive and greater than zero.");
static_assert(NAMEOF_ENUM_MAX_SEARCH_DEPTH % 8 == 0,
              "NAMEOF_ENUM_MAX_SEARCH_DEPTH must be a multiple of 8.");
static_assert(NAMEOF_ENUM_MAX_SEARCH_DEPTH < std::numeric_limits<int>::max(),
              "NAMEOF_ENUM_MAX_SEARCH_DEPTH must be less INT_MAX.");

namespace detail {

template <typename T>
struct identity final {
  using type = T;
};

[[nodiscard]] constexpr bool is_name_char(char c) noexcept {
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

[[nodiscard]] constexpr bool is_bracket_char(char c) noexcept {
  return c == ')' || c == '}' || c == '>' || c == '(' || c == '{' || c == '<';
}

[[nodiscard]] constexpr std::string_view pretty_name(std::string_view name, bool with_suffix) noexcept {
  for (std::size_t i = name.size(), h = 0, s = 0; i > 0; --i) {
    if (h == 0 && !is_name_char(name[i - 1]) && !is_bracket_char(name[i - 1])) {
      ++s;
      continue;
    }

    if (name[i - 1] == ')' || name[i - 1] == '}') {
      ++h;
      ++s;
      continue;
    } else if (name[i - 1] == '(' || name[i - 1] == '{') {
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

  std::size_t s = 0;
  for (std::size_t i = name.size(), h = 0; i > 0; --i) {
    if (name[i - 1] == '>') {
      ++h;
      ++s;
      continue;
    } else if (name[i - 1] == '<') {
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

  for (std::size_t i = name.size() - s; i > 0; --i) {
    if (!is_name_char(name[i - 1])) {
      name.remove_prefix(i);
      break;
    }
  }
  name.remove_suffix(with_suffix ? 0 : s);

  return name;
}

template <typename T>
[[nodiscard]] constexpr std::string_view nameof_type_impl() noexcept {
#if defined(__clang__)
  std::string_view name{__PRETTY_FUNCTION__};
  constexpr auto prefix = sizeof("std::string_view nameof::detail::nameof_type_impl() [T = nameof::detail::identity<") - 1;
  constexpr auto suffix = sizeof(">]") - 1;
#elif defined(__GNUC__)
  std::string_view name{__PRETTY_FUNCTION__};
  constexpr auto prefix = sizeof("constexpr std::string_view nameof::detail::nameof_type_impl() [with T = nameof::detail::identity<") - 1;
  constexpr auto suffix = sizeof(">; std::string_view = std::basic_string_view<char>]") - 1;
#elif defined(_MSC_VER)
  std::string_view name{__FUNCSIG__};
  constexpr auto prefix = sizeof("class std::basic_string_view<char,struct std::char_traits<char> > __cdecl nameof::detail::nameof_type_impl<struct nameof::detail::identity<") - 1;
  constexpr auto suffix = sizeof(">>(void) noexcept") - 1;
#else
  return {}; // Unsupported compiler.
#endif

#if defined(__clang__) || defined(__GNUC__) || defined(_MSC_VER)
  name.remove_prefix(prefix);
  name.remove_suffix(suffix);
#  if defined(_MSC_VER)
  if (name.size() > sizeof("enum") && name[0] == 'e' && name[1] == 'n' && name[2] == 'u' && name[3] == 'm' && name[4] == ' ') {
    name.remove_prefix(sizeof("enum"));
  }
  if (name.size() > sizeof("class") && name[0] == 'c' && name[1] == 'l' && name[2] == 'a' && name[3] == 's' && name[4] == 's' && name[5] == ' ') {
    name.remove_prefix(sizeof("class"));
  }
  if (name.size() > sizeof("struct") && name[0] == 's' && name[1] == 't' && name[2] == 'r' && name[3] == 'u' && name[4] == 'c' && name[5] == 't' && name[6] == ' ') {
    name.remove_prefix(sizeof("struct"));
  }
#  endif
  while (name.back() == ' ') {
    name.remove_suffix(1);
  }

  return name;
#endif
}

template <auto V>
[[nodiscard]] constexpr std::string_view nameof_enum_impl() noexcept {
  static_assert(std::is_enum_v<decltype(V)>);
#if defined(__clang__)
  std::string_view name{__PRETTY_FUNCTION__};
  constexpr auto suffix = sizeof("]") - 1;
#elif defined(__GNUC__) && __GNUC__ >= 9
  std::string_view name{__PRETTY_FUNCTION__};
  constexpr auto suffix = sizeof("; std::string_view = std::basic_string_view<char>]") - 1;
#elif defined(_MSC_VER)
  std::string_view name{__FUNCSIG__};
  constexpr auto suffix = sizeof(">(void) noexcept") - 1;
#else
  return {}; // Unsupported compiler.
#endif

#if defined(__clang__) || (defined(__GNUC__) && __GNUC__ >= 9) || defined(_MSC_VER)
  name.remove_suffix(suffix);
  for (std::size_t i = name.size(); i > 0; --i) {
    if (!is_name_char(name[i - 1])) {
      name.remove_prefix(i);
      break;
    }
  }

  if (name.front() >= '0' && name.front() <= '9') {
    return {}; // Enum variable does not have name.
  } else {
    return name;
  }
#endif
}

template <typename E, int V>
struct nameof_enum_impl_t final {
  [[nodiscard]] constexpr std::string_view operator()(int value) const noexcept {
    static_assert(std::is_enum_v<E>);
    if constexpr (V > std::numeric_limits<std::underlying_type_t<E>>::max()) {
      return {}; // Enum variable out of range.
    }

    switch (value - V) {
      case 0:
        return nameof_enum_impl<static_cast<E>(V)>();
      case 1:
        return nameof_enum_impl<static_cast<E>(V + 1)>();
      case 2:
        return nameof_enum_impl<static_cast<E>(V + 2)>();
      case 3:
        return nameof_enum_impl<static_cast<E>(V + 3)>();
      case 4:
        return nameof_enum_impl<static_cast<E>(V + 4)>();
      case 5:
        return nameof_enum_impl<static_cast<E>(V + 5)>();
      case 6:
        return nameof_enum_impl<static_cast<E>(V + 6)>();
      case 7:
        return nameof_enum_impl<static_cast<E>(V + 7)>();
      default:
        return nameof_enum_impl_t<E, V + 8>{}(value);
    }
  }
};

template <typename E>
struct nameof_enum_impl_t<E, NAMEOF_ENUM_MAX_SEARCH_DEPTH> final {
  [[nodiscard]] constexpr std::string_view operator()(int) const noexcept {
    static_assert(std::is_enum_v<E>);
    return {}; // Enum variable out of range NAMEOF_ENUM_MAX_SEARCH_DEPTH.
  }
};

template <typename T, typename = std::enable_if_t<!std::is_reference_v<T>>>
[[nodiscard]] constexpr std::string_view nameof_impl(std::string_view name, bool with_suffix) noexcept {
  return pretty_name(name, with_suffix);
}

[[nodiscard]] constexpr std::string_view nameof_raw_impl(std::string_view name) noexcept {
  return name;
}

} // namespace detail

// nameof_enum used to obtain the simple (unqualified) string enum name of enum variable.
template <typename T, typename = std::enable_if_t<std::is_enum_v<std::decay_t<T>>>>
[[nodiscard]] constexpr std::string_view nameof_enum(T value) noexcept {
  constexpr bool s = std::is_signed_v<std::underlying_type_t<std::decay_t<T>>>;
  constexpr int min = s ? -NAMEOF_ENUM_MAX_SEARCH_DEPTH : 0;
  return detail::nameof_enum_impl_t<std::decay_t<T>, min>{}(static_cast<int>(value));
}

// nameof_enum used to obtain the simple (unqualified) string enum name of static storage enum variable.
template <auto V, typename = std::enable_if_t<std::is_enum_v<std::decay_t<decltype(V)>>>>
[[nodiscard]] constexpr std::string_view nameof_enum() noexcept {
  return detail::nameof_enum_impl<V>();
}

// nameof_type used to obtain the string name of type.
template <typename T>
[[nodiscard]] constexpr std::string_view nameof_type() noexcept {
  return detail::nameof_type_impl<detail::identity<T>>();
}

} // namespace nameof

// NAMEOF used to obtain the simple (unqualified) string name of variable, function, enum, macro.
#define NAMEOF(...) ::nameof::detail::nameof_impl<decltype(__VA_ARGS__)>(#__VA_ARGS__, false)

// NAMEOF_FULL used to obtain the full string name of variable, function, enum, macro.
#define NAMEOF_FULL(...) ::nameof::detail::nameof_impl<decltype(__VA_ARGS__)>(#__VA_ARGS__, true)

// NAMEOF_ENUM used to obtain the simple (unqualified) string enum name of enum variable.
#define NAMEOF_ENUM(...) ::nameof::nameof_enum<decltype(__VA_ARGS__)>(__VA_ARGS__)

// NAMEOF_TYPE used to obtain the string name of variable type.
#define NAMEOF_TYPE(...) ::nameof::nameof_type<decltype(__VA_ARGS__)>()

// NAMEOF_TYPE_T used to obtain the string name of type.
#define NAMEOF_TYPE_T(...) ::nameof::nameof_type<__VA_ARGS__>()

// NAMEOF_RAW used to obtain the raw string name of variable, function, enum, macro.
#define NAMEOF_RAW(...) ::nameof::detail::nameof_raw_impl(#__VA_ARGS__)
