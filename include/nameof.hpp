//  _   _                             __    _____
// | \ | |                           / _|  / ____|_     _
// |  \| | __ _ _ __ ___   ___  ___ | |_  | |   _| |_ _| |_
// | . ` |/ _` | '_ ` _ \ / _ \/ _ \|  _| | |  |_   _|_   _|
// | |\  | (_| | | | | | |  __/ (_) | |   | |____|_|   |_|
// |_| \_|\__,_|_| |_| |_|\___|\___/|_|    \_____|
// https://github.com/Neargye/nameof
// vesion 0.9.0
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

#ifndef NEARGYE_NAMEOF_HPP
#define NEARGYE_NAMEOF_HPP

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

// Enum value must be in range [NAMEOF_ENUM_RANGE_MIN, NAMEOF_ENUM_RANGE_MAX]. By default NAMEOF_ENUM_RANGE_MIN = -128, NAMEOF_ENUM_RANGE_MAX = 128.
// If need another range for all enum types by default, redefine the macro NAMEOF_ENUM_RANGE_MIN and NAMEOF_ENUM_RANGE_MAX.
// If need another range for specific enum type, add specialization enum_range for necessary enum type.
template <typename E>
struct enum_range final {
  static_assert(std::is_enum_v<E>, "nameof::enum_range requires enum type.");
  static constexpr int min = NAMEOF_ENUM_RANGE_MIN;
  static constexpr int max = NAMEOF_ENUM_RANGE_MAX;
  static_assert(max > min, "nameof::enum_range requires max > min.");
};

static_assert(NAMEOF_ENUM_RANGE_MIN <= 0,
              "NAMEOF_ENUM_RANGE_MIN must be less or equals than 0.");
static_assert(NAMEOF_ENUM_RANGE_MIN > (std::numeric_limits<int>::min)(),
              "NAMEOF_ENUM_RANGE_MIN must be greater than INT_MIN.");

static_assert(NAMEOF_ENUM_RANGE_MAX > 0,
              "NAMEOF_ENUM_RANGE_MAX must be greater than 0.");
static_assert(NAMEOF_ENUM_RANGE_MAX < (std::numeric_limits<int>::max)(),
              "NAMEOF_ENUM_RANGE_MAX must be less than INT_MAX.");

static_assert(NAMEOF_ENUM_RANGE_MAX > NAMEOF_ENUM_RANGE_MIN,
              "NAMEOF_ENUM_RANGE_MAX must be greater than NAMEOF_ENUM_RANGE_MIN.");

namespace detail {

template <typename T>
struct identity final {
  using type = T;
};

template <typename T>
struct nameof_type_supported final
#if defined(__clang__) || defined(__GNUC__) || defined(_MSC_VER) || defined(NAMEOF_TYPE_NO_CHECK_SUPPORT)
    : std::true_type {};
#else
    : std::false_type {};
#endif

template <typename T>
struct nameof_enum_supported final
#if defined(__clang__) || defined(__GNUC__) && __GNUC__>= 9 || defined(_MSC_VER) || defined(NAMEOF_ENUM_NO_CHECK_SUPPORT)
    : std::true_type {};
#else
    : std::false_type {};
#endif

template <std::size_t N>
struct static_string final {
  constexpr static_string(std::string_view str) noexcept : static_string(str, std::make_index_sequence<N>{}) {}

  constexpr operator std::string_view() const noexcept { return {chars.data(), chars.size()}; }

 private:
  template <std::size_t... I>
  constexpr static_string(std::string_view str, std::index_sequence<I...>) noexcept : chars{{str[I]...}} {}

  const std::array<char, N> chars;
};

template <>
struct static_string<0> final {
  constexpr static_string(std::string_view) noexcept {}

  constexpr operator std::string_view() const noexcept { return {}; }
};

template <typename T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <typename T, typename R>
using enable_if_enum_t = std::enable_if_t<std::is_enum_v<remove_cvref_t<T>>, R>;

template <typename T = void>
[[nodiscard]] constexpr std::string_view nameof(std::string_view name, bool remove_template_suffix = true) noexcept {
  static_assert(std::is_void_v<T>, "nameof::detail::nameof requires void type.");
  if (name.size() >= 1 && (name.front() == '"' || name.front() == '\'')) {
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
  } else if (name.size() >= 1 && (name.front() >= '0' && name.front() <= '9')) {
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
    if (!((name[i - 1] >= '0' && name[i - 1] <= '9') ||
          (name[i - 1] >= 'a' && name[i - 1] <= 'z') ||
          (name[i - 1] >= 'A' && name[i - 1] <= 'Z') ||
          (name[i - 1] == '_'))) {
      name.remove_prefix(i);
      break;
    }
  }
  if (remove_template_suffix) {
    name.remove_suffix(s);
  }

  if (name.size() > 0 && ((name.front() >= 'a' && name.front() <= 'z') ||
                          (name.front() >= 'A' && name.front() <= 'Z') ||
                          (name.front() == '_'))) {
    return name;
  }

  return {}; // Invalid name.
}

template <typename T>
[[nodiscard]] constexpr std::string_view nameof_raw(std::string_view name) noexcept {
  static_assert(std::is_void_v<T>, "nameof::detail::nameof_raw requires void type.");

  return name;
}

template <typename E, E V>
[[nodiscard]] constexpr auto n() noexcept {
  static_assert(std::is_enum_v<E>, "nameof::detail::nameof_enum requires enum type.");
#if defined(__clang__) || defined(__GNUC__) && __GNUC__ >= 9
  constexpr auto name = nameof({__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2});
#elif defined(_MSC_VER)
  constexpr auto name = nameof({__FUNCSIG__, sizeof(__FUNCSIG__) - 17});
#else
  return std::string_view{}; // Unsupported compiler.
#endif

#if defined(__clang__) || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER)
  return static_string<name.size()>{name};
#endif
}

template <typename E, E V>
inline constexpr auto nameof_enum_v = n<E, V>();

template <typename E, int O, int... I>
[[nodiscard]] constexpr auto enum_names(std::integer_sequence<int, I...>) noexcept {
  static_assert(std::is_enum_v<E>, "nameof::detail::enum_names requires enum type.");

  return std::array<std::string_view, sizeof...(I)>{{nameof_enum_v<E, static_cast<E>(I + O)>...}};
}

template <typename... T>
[[nodiscard]] constexpr auto n() noexcept {
#if defined(__clang__)
  constexpr std::string_view name{__PRETTY_FUNCTION__ + 31, sizeof(__PRETTY_FUNCTION__) - 34};
#elif defined(__GNUC__)
  constexpr std::string_view name{__PRETTY_FUNCTION__ + 46, sizeof(__PRETTY_FUNCTION__) - 49};
#elif defined(_MSC_VER)
  constexpr std::string_view name{__FUNCSIG__ + 63, sizeof(__FUNCSIG__) - 81 - (__FUNCSIG__[sizeof(__FUNCSIG__) - 19] == ' ' ? 1 : 0)};
#else
  return std::string_view{}; // Unsupported compiler.
#endif

#if defined(__clang__) || defined(__GNUC__) || defined(_MSC_VER)
  return static_string<name.size()>{name};
#endif
}

#if defined(_MSC_VER)
template <typename U, typename T = identity<U>>
#else
template <typename T>
#endif
inline constexpr auto nameof_type_v = n<T>();

} // namespace nameof::detail

// Checks is nameof_type supported compiler.
inline constexpr bool is_nameof_type_supported = detail::nameof_type_supported<void>::value;

// Checks is nameof_enum supported compiler.
inline constexpr bool is_nameof_enum_supported = detail::nameof_enum_supported<void>::value;

// Obtains simple (unqualified) string enum name of enum variable.
template <typename E>
[[nodiscard]] constexpr detail::enable_if_enum_t<E, std::string_view> nameof_enum(E value) noexcept {
  using D = detail::remove_cvref_t<E>;
  static_assert(detail::nameof_enum_supported<D>::value, "nameof::nameof_enum: Unsupported compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  static_assert(std::is_enum_v<D>, "nameof::nameof_enum requires enum type.");
  static_assert(enum_range<D>::min > (std::numeric_limits<int>::min)(), "nameof::enum_range requires min must be greater than INT_MIN.");
  static_assert(enum_range<D>::max < (std::numeric_limits<int>::max)(), "nameof::enum_range requires max must be less than INT_MAX.");
  static_assert(enum_range<D>::max > enum_range<D>::min, "nameof::enum_range requires max > min.");
  using U = std::underlying_type_t<D>;
  constexpr int max = static_cast<int>(enum_range<D>::max < (std::numeric_limits<U>::max)() ? enum_range<D>::max : (std::numeric_limits<U>::max)());
  constexpr int min = static_cast<int>(enum_range<D>::min > (std::numeric_limits<U>::min)() ? enum_range<D>::min : (std::numeric_limits<U>::min)());
  constexpr auto names = detail::enum_names<D, min>(std::make_integer_sequence<int, max - min + 1>{});

  if (static_cast<U>(value) > static_cast<U>(max) || static_cast<U>(value) < static_cast<U>(min)) {
    return {}; // Value out of range.
  }

  if (auto i = static_cast<std::size_t>(static_cast<U>(value) - min); i < names.size()) {
    return names[i];
  }

  return {}; // Value out of range.
}

// Obtains simple (unqualified) string enum name of static storage enum variable.
// This version is much lighter on the compile times and is not restricted to the enum_range limitation.
template <auto V>
[[nodiscard]] constexpr detail::enable_if_enum_t<decltype(V), std::string_view> nameof_enum() noexcept {
  using D = detail::remove_cvref_t<decltype(V)>;
  static_assert(detail::nameof_enum_supported<D>::value, "nameof::nameof_enum: Unsupported compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  static_assert(std::is_enum_v<D>, "nameof::nameof_enum requires enum type.");

  return detail::nameof_enum_v<D, V>;
}

// Obtains string name of type, reference and cv-qualifiers are ignored.
template <typename T>
[[nodiscard]] constexpr std::string_view nameof_type() noexcept {
  static_assert(detail::nameof_type_supported<T>::value, "nameof::nameof_type: Unsupported compiler (https://github.com/Neargye/nameof#compiler-compatibility).");

  return detail::nameof_type_v<detail::remove_cvref_t<T>>;
}

// Obtains string name of full type, with reference and cv-qualifiers.
template <typename T>
[[nodiscard]] constexpr std::string_view nameof_full_type() noexcept {
  static_assert(detail::nameof_type_supported<T>::value, "nameof::nameof_type: Unsupported compiler (https://github.com/Neargye/nameof#compiler-compatibility).");

  return detail::nameof_type_v<T>;
}

} // namespace nameof

// Obtains simple (unqualified) string name of variable, function, enum, macro.
#define NAMEOF(...) ::nameof::detail::nameof<::std::void_t<decltype(__VA_ARGS__)>>(#__VA_ARGS__, true)

// Obtains simple (unqualified) full (with template suffix) string name of variable, function, enum, macro.
#define NAMEOF_FULL(...) ::nameof::detail::nameof<::std::void_t<decltype(__VA_ARGS__)>>(#__VA_ARGS__, false)

// Obtains raw string name of variable, function, enum, macro.
#define NAMEOF_RAW(...) ::nameof::detail::nameof_raw<::std::void_t<decltype(__VA_ARGS__)>>(#__VA_ARGS__)

// Obtains simple (unqualified) string enum name of enum variable.
#define NAMEOF_ENUM(...) ::nameof::nameof_enum<decltype(__VA_ARGS__)>(__VA_ARGS__)

// Obtains simple (unqualified) string enum name of static storage enum variable.
// This version is much lighter on the compile times and is not restricted to the enum_range limitation.
#define NAMEOF_CONST_ENUM(...) ::nameof::nameof_enum<__VA_ARGS__>()

// Obtains string name of type, reference and cv-qualifiers are ignored.
#define NAMEOF_TYPE(...) ::nameof::nameof_type<__VA_ARGS__>()

// Obtains string name of full type, with reference and cv-qualifiers.
#define NAMEOF_FULL_TYPE(...) ::nameof::nameof_full_type<__VA_ARGS__>()

// Obtains string name type of expression, reference and cv-qualifiers are ignored.
#define NAMEOF_TYPE_EXPR(...) ::nameof::nameof_type<decltype(__VA_ARGS__)>()

// Obtains string name full type of expression, with reference and cv-qualifiers.
#define NAMEOF_FULL_TYPE_EXPR(...) ::nameof::nameof_full_type<decltype(__VA_ARGS__)>()

#endif // NEARGYE_NAMEOF_HPP
