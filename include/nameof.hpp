//  _   _                             __    _____
// | \ | |                           / _|  / ____|_     _
// |  \| | __ _ _ __ ___   ___  ___ | |_  | |   _| |_ _| |_
// | . ` |/ _` | '_ ` _ \ / _ \/ _ \|  _| | |  |_   _|_   _|
// | |\  | (_| | | | | | |  __/ (_) | |   | |____|_|   |_|
// |_| \_|\__,_|_| |_| |_|\___|\___/|_|    \_____|
// https://github.com/Neargye/nameof
// vesion 0.8.0
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

// Enum value must be greater or equals than NAMEOF_ENUM_RANGE_MIN. By default NAMEOF_ENUM_RANGE_MIN = -128.
// If need another min range for all enum types by default, redefine the macro NAMEOF_ENUM_RANGE_MIN.
#if !defined(NAMEOF_ENUM_RANGE_MIN)
#  define NAMEOF_ENUM_RANGE_MIN -128
#endif

// Enum value must be less or equals than NAMEOF_ENUM_RANGE_MAX. By default NAMEOF_ENUM_RANGE_MAX = 128.
// If need another max range for all enum types by default, redefine the macro NAMEOF_ENUM_RANGE_MAX.
#if !defined(NAMEOF_ENUM_RANGE_MAX)
#  define NAMEOF_ENUM_RANGE_MAX 128
#endif

namespace nameof {

// Enum value must be in range [-NAMEOF_ENUM_RANGE_MAX, NAMEOF_ENUM_RANGE_MIN]. By default  NAMEOF_ENUM_RANGE_MIN = -128, NAMEOF_ENUM_RANGE_MAX = 128.
// If need another range for all enum types by default, redefine the macro NAMEOF_ENUM_RANGE_MAX and NAMEOF_ENUM_RANGE_MIN.
// If need another range for specific enum type, add specialization enum_range for necessary enum type.
template <typename E>
struct enum_range final {
  static_assert(std::is_enum_v<E>, "nameof::enum_range requires enum type.");
  static constexpr int min = std::is_signed_v<std::underlying_type_t<E>> ? NAMEOF_ENUM_RANGE_MIN : 0;
  static constexpr int max = NAMEOF_ENUM_RANGE_MAX;
};

namespace detail {

template <typename T>
struct identity final {
  using type = T;
};

template <typename... T>
[[nodiscard]] constexpr std::string_view nameof_type_impl() noexcept {
#if defined(__clang__)
  constexpr auto prefix = sizeof("std::string_view nameof::detail::nameof_type_impl() [T = <nameof::detail::identity<") - 1;
  constexpr auto suffix = sizeof(">>]") - 1;
  constexpr std::string_view name{__PRETTY_FUNCTION__ + prefix, sizeof(__PRETTY_FUNCTION__) - prefix - suffix - 1};
#elif defined(__GNUC__)
  constexpr auto prefix = sizeof("constexpr std::string_view nameof::detail::nameof_type_impl() [with T = {nameof::detail::identity<") - 1;
  constexpr auto suffix = sizeof(">}; std::string_view = std::basic_string_view<char>]") - 1;
  constexpr std::string_view name{__PRETTY_FUNCTION__ + prefix, sizeof(__PRETTY_FUNCTION__) - prefix - suffix - 1};
#elif defined(_MSC_VER)
  constexpr auto prefix = sizeof("class std::basic_string_view<char,struct std::char_traits<char> > __cdecl nameof::detail::nameof_type_impl<struct nameof::detail::identity<") - 1;
  constexpr auto suffix = sizeof(">>(void) noexcept") - 1;
  constexpr std::string_view name{__FUNCSIG__ + prefix, sizeof(__FUNCSIG__) - prefix - suffix - 1};
#else
  return {}; // Unsupported compiler.
#endif

#if defined(__clang__) || defined(__GNUC__) || defined(_MSC_VER)
 return name.substr(0, name.length() - (name.back() == ' ' ? 1 : 0));
#endif
}

template <typename E, E V>
[[nodiscard]] constexpr std::string_view enum_name_impl() noexcept {
  static_assert(std::is_enum_v<E>, "nameof::enum_name_impl requires enum type.");
#if defined(__clang__)
  constexpr std::string_view name{__PRETTY_FUNCTION__};
  constexpr auto suffix = sizeof("]") - 1;
#elif defined(__GNUC__) && __GNUC__ >= 9
  constexpr std::string_view name{__PRETTY_FUNCTION__};
  constexpr auto suffix = sizeof("; std::string_view = std::basic_string_view<char>]") - 1;
#elif defined(_MSC_VER)
  constexpr std::string_view name{__FUNCSIG__};
  constexpr auto suffix = sizeof(">(void) noexcept") - 1;
#else
  return {}; // Unsupported compiler.
#endif

#if defined(__clang__) || (defined(__GNUC__) && __GNUC__ >= 9) || defined(_MSC_VER)
  constexpr auto prefix = name.find_last_of(" :,-)", name.length() - suffix) + 1;

  if constexpr (name[prefix] >= '0' && name[prefix] <= '9') {
    return {}; // Value does not have name.
  } else {
    return name.substr(prefix, name.length() - prefix - suffix);
  }
#endif
}

template <typename E, int O, int... I>
[[nodiscard]] constexpr decltype(auto) enum_strings_impl(std::integer_sequence<int, I...>) noexcept {
  static_assert(std::is_enum_v<E>, "nameof::detail::enum_strings_impl requires enum type.");
  constexpr std::array<std::string_view, sizeof...(I)> names{{enum_name_impl<E, static_cast<E>(I + O)>()...}};

  return names;
}

template <typename E, typename U = std::underlying_type_t<E>>
[[nodiscard]] constexpr std::string_view nameof_enum_impl(int value) noexcept {
  static_assert(std::is_enum_v<E>, "nameof::detail::nameof_enum_impl requires enum type.");
  static_assert(enum_range<E>::max > enum_range<E>::min, "nameof::enum_range requires max > min.");
  constexpr int max = enum_range<E>::max < (std::numeric_limits<U>::max)() ? enum_range<E>::max : (std::numeric_limits<U>::max)();
  constexpr int min = enum_range<E>::min > (std::numeric_limits<U>::min)() ? enum_range<E>::min : (std::numeric_limits<U>::min)();
  constexpr auto range = std::make_integer_sequence<int, max - min + 1>{};
  constexpr auto names = enum_strings_impl<E, min>(range);
  const int i = value - min;

  if (i >= 0 && static_cast<std::size_t>(i) < names.size()) {
    return names[i];
  } else {
    return {}; // Value out of range.
  }
}

template <typename T>
[[nodiscard]] constexpr std::string_view nameof_impl(std::string_view name, bool with_template_suffix) noexcept {
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

  for (std::size_t i = name.length(), h = 0, s = 0; i > 0; --i) {
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
  for (std::size_t i = name.length(), h = 0; i > 0; --i) {
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

  for (std::size_t i = name.length() - s; i > 0; --i) {
    if (!((name[i - 1] >= '0' && name[i - 1] <= '9') ||
          (name[i - 1] >= 'a' && name[i - 1] <= 'z') ||
          (name[i - 1] >= 'A' && name[i - 1] <= 'Z') ||
          (name[i - 1] == '_'))) {
      name.remove_prefix(i);
      break;
    }
  }
  name.remove_suffix(with_template_suffix ? 0 : s);

  if (name.length() > 0 && ((name.front() >= 'a' && name.front() <= 'z') ||
                            (name.front() >= 'A' && name.front() <= 'Z') ||
                            (name.front() == '_'))) {
    return name;
  } else {
    return {};  // Invalid name.
  }
}

template <typename T>
[[nodiscard]] constexpr std::string_view nameof_raw_impl(std::string_view name) noexcept {
  return name;
}

} // namespace nameof::detail

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
