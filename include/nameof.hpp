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

#include <array>
#include <cstddef>
#include <limits>
#include <type_traits>
#include <string_view>

namespace nameof {

// Enum value must be in range [-256, 256]. If you need another range, add specialization enum_range for necessary enum type.
template <typename E>
struct enum_range final {
  static_assert(std::is_enum_v<E>, "nameof::enum_range requires enum type.");
  static constexpr int min = std::is_signed_v<std::underlying_type_t<E>> ? -256 : 0;
  static constexpr int max = 256;
};

namespace detail {

template <typename T>
struct identity final {
  using type = T;
};

[[nodiscard]] constexpr bool is_name_char(char c, bool front) noexcept {
  return (!front && c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

[[nodiscard]] constexpr std::string_view pretty_name(std::string_view name, bool with_template_suffix) noexcept {
  if (name.length() >= 1 && (name.front() == '"' || name.front() == '\'')) {
    return {}; // Narrow multibyte string literal.
  } else if (name.length() >= 2 && name[0] == 'R' && (name[1] == '"' || name[1] == '\'')) {
    return {}; // Raw string literal.
  } else if (name.length() >= 2 && name[0] == 'L' && (name[1] == '"' || name[1] == '\'')) {
    return {}; // Wide string literal.
  } else if (name.length() >= 2 && name[0] == 'U' && (name[1] == '"' || name[1] == '\'')) {
    return {}; // UTF-32 encoded string literal.
  } else if (name.length() >= 2 && name[0] == 'u' && (name[1] == '"' || name[1] == '\'')) {
    return {}; // UTF-16 encoded string literal.
  } else if (name.length() >= 3 && name[0] == 'u' && name[1] == '8' && (name[2] == '"' || name[2] == '\'')) {
    return {}; // UTF-8 encoded string literal.
  } else if (name.length() >= 1 && (name.front() >= '0' && name.front() <= '9')) {
    return {}; // Invalid name.
  }

  for (std::size_t i = name.size(), h = 0, s = 0; i > 0; --i) {
    if (name[i - 1] == ')') {
      ++h;
      ++s;
      continue;
    } else if (name[i - 1] == '(') {
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
    if (!is_name_char(name[i - 1], false)) {
      name.remove_prefix(i);
      break;
    }
  }
  name.remove_suffix(with_template_suffix ? 0 : s);

  if (name.length() > 0 && is_name_char(name.front(), true)) {
    return name;
  } else {
    return {}; // Invalid name.
  }
}

template <typename... T>
[[nodiscard]] constexpr std::string_view nameof_type_impl() noexcept {
#if defined(__clang__)
  std::string_view name{__PRETTY_FUNCTION__};
  constexpr auto prefix = sizeof("std::string_view nameof::detail::nameof_type_impl() [T = <nameof::detail::identity<") - 1;
  constexpr auto suffix = sizeof(">>]") - 1;
#elif defined(__GNUC__)
  std::string_view name{__PRETTY_FUNCTION__};
  constexpr auto prefix = sizeof("constexpr std::string_view nameof::detail::nameof_type_impl() [with T = {nameof::detail::identity<") - 1;
  constexpr auto suffix = sizeof(">}; std::string_view = std::basic_string_view<char>]") - 1;
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
  while (name.back() == ' ') {
    name.remove_suffix(1);
  }

  return name;
#endif
}

template <typename E, E V>
[[nodiscard]] constexpr std::string_view enum_name_impl() noexcept {
  static_assert(std::is_enum_v<E>, "nameof::enum_name_impl requires enum type.");
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

  return pretty_name(name, false);
#endif
}

template <typename E, int O, int... I>
[[nodiscard]] constexpr decltype(auto) enum_strings_impl(std::integer_sequence<int, I...>) noexcept {
  static_assert(std::is_enum_v<E>, "nameof::detail::enum_strings_impl requires enum type.");
  constexpr std::array<std::string_view, sizeof...(I)> enum_names{{enum_name_impl<E, static_cast<E>(I + O)>()...}};

  return enum_names;
}

template <typename E, typename U = std::underlying_type_t<E>>
[[nodiscard]] constexpr std::string_view nameof_enum_impl(int value) noexcept {
  static_assert(std::is_enum_v<E>, "nameof::detail::nameof_enum_impl requires enum type.");
  static_assert(enum_range<E>::max > enum_range<E>::min, "nameof::enum_range requires max > min.");
  constexpr int max = enum_range<E>::max < (std::numeric_limits<U>::max)() ? enum_range<E>::max : (std::numeric_limits<U>::max)();
  constexpr int min = enum_range<E>::min > (std::numeric_limits<U>::min)() ? enum_range<E>::min : (std::numeric_limits<U>::min)();
  constexpr auto enum_range = std::make_integer_sequence<int, max - min + 1>{};
  constexpr auto enum_names = enum_strings_impl<E, min>(enum_range);
  const int i = value - min;

  if (i >= 0 && static_cast<std::size_t>(i) < enum_names.size()) {
    return enum_names[i];
  } else {
    return {}; // Value out of range.
  }
}

template <typename T>
[[nodiscard]] constexpr std::string_view nameof_impl(std::string_view name, bool with_template_suffix) noexcept {
  return pretty_name(name, with_template_suffix);
}

template <typename T>
[[nodiscard]] constexpr std::string_view nameof_raw_impl(std::string_view name) noexcept {
  return name;
}

} // namespace detail

// Obtains simple (unqualified) string enum name of enum variable.
template <typename E, typename D = std::decay_t<E>, typename = std::enable_if_t<std::is_enum_v<D>>>
[[nodiscard]] constexpr std::string_view nameof_enum(E value) noexcept {
  static_assert(std::is_enum_v<D>, "nameof::nameof_enum requires enum type.");

  return detail::nameof_enum_impl<D>(static_cast<int>(value));
}

// Obtains string name of type.
template <typename T>
[[nodiscard]] constexpr std::string_view nameof_type() noexcept {
  return detail::nameof_type_impl<detail::identity<T>>();
}

} // namespace nameof

// NAMEOF obtains simple (unqualified) string name of variable, function, enum, macro.
#define NAMEOF(...) ::nameof::detail::nameof_impl<decltype(__VA_ARGS__)>(#__VA_ARGS__, false)

// Obtains simple (unqualified) full (with template suffix) string name of variable, function, enum, macro.
#define NAMEOF_FULL(...) ::nameof::detail::nameof_impl<decltype(__VA_ARGS__)>(#__VA_ARGS__, true)

// Obtains raw string name of variable, function, enum, macro.
#define NAMEOF_RAW(...) ::nameof::detail::nameof_raw_impl<decltype(__VA_ARGS__)>(#__VA_ARGS__)

// Obtains simple (unqualified) string enum name of enum variable.
#define NAMEOF_ENUM(...) ::nameof::nameof_enum<decltype(__VA_ARGS__)>(__VA_ARGS__)

// Obtains string name of type.
#define NAMEOF_TYPE(...) ::nameof::nameof_type<__VA_ARGS__>()

// Obtains string name of variable type.
#define NAMEOF_VAR_TYPE(...) ::nameof::nameof_type<decltype(__VA_ARGS__)>()
