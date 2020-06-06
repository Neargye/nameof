//  _   _                             __    _____
// | \ | |                           / _|  / ____|_     _
// |  \| | __ _ _ __ ___   ___  ___ | |_  | |   _| |_ _| |_
// | . ` |/ _` | '_ ` _ \ / _ \/ _ \|  _| | |  |_   _|_   _|
// | |\  | (_| | | | | | |  __/ (_) | |   | |____|_|   |_|
// |_| \_|\__,_|_| |_| |_|\___|\___/|_|    \_____|
// https://github.com/Neargye/nameof
// version 0.9.4
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2016, 2018 - 2020 Daniil Goncharov <neargye@gmail.com>.
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
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <iosfwd>
#include <iterator>
#include <limits>
#include <string_view>
#include <type_traits>
#include <utility>

#if __has_include(<cxxabi.h>)
#include <cxxabi.h>
#include <cstdlib>
#include <string>
#endif

#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 26495) // Variable 'nameof::cstring<N>::chars_' is uninitialized.
#endif

// Checks nameof_type compiler compatibility.
#if defined(__clang__) && __clang_major__ >= 5 || defined(__GNUC__) && __GNUC__ >= 7 || defined(_MSC_VER) && _MSC_VER >= 1910
#  undef  NAMEOF_TYPE_SUPPORTED
#  define NAMEOF_TYPE_SUPPORTED 1
#endif

// Checks nameof_type_rtti compiler compatibility.
#if defined(__clang__)
#  if __has_feature(cxx_rtti)
#    undef  NAMEOF_TYPE_RTTI_SUPPORTED
#    define NAMEOF_TYPE_RTTI_SUPPORTED 1
#  endif
#elif defined(__GNUC__)
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

// Checks nameof_enum compiler compatibility.
#if defined(__clang__) && __clang_major__ >= 5 || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER) && _MSC_VER >= 1910
#  undef  NAMEOF_ENUM_SUPPORTED
#  define NAMEOF_ENUM_SUPPORTED 1
#endif

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
struct enum_range {
  static_assert(std::is_enum_v<E>, "nameof::enum_range requires enum type.");
  inline static constexpr int min = NAMEOF_ENUM_RANGE_MIN;
  inline static constexpr int max = NAMEOF_ENUM_RANGE_MAX;
  static_assert(max > min, "nameof::enum_range requires max > min.");
};

static_assert(NAMEOF_ENUM_RANGE_MIN <= 0, "NAMEOF_ENUM_RANGE_MIN must be less or equals than 0.");
static_assert(NAMEOF_ENUM_RANGE_MIN > (std::numeric_limits<std::int16_t>::min)(), "NAMEOF_ENUM_RANGE_MIN must be greater than INT16_MIN.");

static_assert(NAMEOF_ENUM_RANGE_MAX > 0, "NAMEOF_ENUM_RANGE_MAX must be greater than 0.");
static_assert(NAMEOF_ENUM_RANGE_MAX < (std::numeric_limits<std::int16_t>::max)(), "NAMEOF_ENUM_RANGE_MAX must be less than INT16_MAX.");

static_assert(NAMEOF_ENUM_RANGE_MAX > NAMEOF_ENUM_RANGE_MIN, "NAMEOF_ENUM_RANGE_MAX must be greater than NAMEOF_ENUM_RANGE_MIN.");

template <std::size_t N>
class [[nodiscard]] cstring {
  static_assert(N > 0, "nameof::cstring requires size greater than 0.");

  std::array<char, N + 1> chars_;

  template <std::size_t... I>
  constexpr cstring(std::string_view str, std::index_sequence<I...>) noexcept : chars_{{str[I]..., '\0'}} {}

 public:
  using value_type      = const char;
  using size_type       = std::size_t;
  using difference_type = std::ptrdiff_t;
  using pointer         = const char*;
  using const_pointer   = const char*;
  using reference       = const char&;
  using const_reference = const char&;

  using iterator       = const char*;
  using const_iterator = const char*;

  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  constexpr explicit cstring(std::string_view str) noexcept : cstring{str, std::make_index_sequence<N>{}} {
    assert(str.size() == N);
  }

  constexpr cstring() = delete;

  constexpr cstring(const cstring&) = default;

  constexpr cstring(cstring&&) = default;

  ~cstring() = default;

  cstring& operator=(const cstring&) = default;

  cstring& operator=(cstring&&) = default;

  [[nodiscard]] constexpr const_pointer data() const noexcept { return chars_.data(); }

  [[nodiscard]] constexpr size_type size() const noexcept { return N; }

  [[nodiscard]] constexpr const_iterator begin() const noexcept { return data(); }

  [[nodiscard]] constexpr const_iterator end() const noexcept { return data() + size(); }

  [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return begin(); }

  [[nodiscard]] constexpr const_iterator cend() const noexcept { return end(); }

  [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept { return end(); }

  [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept { return begin(); }

  [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept { return rbegin(); }

  [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept { return rend(); }

  [[nodiscard]] constexpr const_reference operator[](size_type i) const noexcept { return assert(i < size()), chars_[i]; }

  [[nodiscard]] constexpr const_reference at(size_type i) const { return assert(i < size()), chars_.at(i); }

  [[nodiscard]] constexpr const_reference front() const noexcept { return chars_[0]; }

  [[nodiscard]] constexpr const_reference back() const noexcept { return chars_[N]; }

  [[nodiscard]] constexpr size_type length() const noexcept { return size(); }

  [[nodiscard]] constexpr bool empty() const noexcept { return false; }

  [[nodiscard]] constexpr int compare(std::string_view str) const noexcept {
    return std::string_view{data(), size()}.compare(str);
  }

  [[nodiscard]] constexpr const char* c_str() const noexcept { return data(); }

  template <typename Char = char, typename Traits = std::char_traits<Char>, typename Allocator = std::allocator<Char>>
  [[nodiscard]] std::basic_string<Char, Traits, Allocator> str() const { return {begin(), end()}; }

  [[nodiscard]] constexpr operator std::string_view() const noexcept { return {data(), size()}; }

  [[nodiscard]] constexpr explicit operator const char*() const noexcept { return data(); }

  template <typename Char = char, typename Traits = std::char_traits<Char>, typename Allocator = std::allocator<Char>>
  [[nodiscard]] explicit operator std::basic_string<Char, Traits, Allocator>() const { return {begin(), end()}; }
};

template <std::size_t N>
[[nodiscard]] constexpr bool operator==(const cstring<N>& lhs, std::string_view rhs) noexcept {
  return lhs.compare(rhs) == 0;
}

template <std::size_t N>
[[nodiscard]] constexpr bool operator==(std::string_view lhs, const cstring<N>& rhs) noexcept {
  return lhs.compare(rhs) == 0;
}

template <std::size_t N>
[[nodiscard]] constexpr bool operator!=(const cstring<N>& lhs, std::string_view rhs) noexcept {
  return lhs.compare(rhs) != 0;
}

template <std::size_t N>
[[nodiscard]] constexpr bool operator!=(std::string_view lhs, const cstring<N>& rhs) noexcept {
  return lhs.compare(rhs) != 0;
}

template <std::size_t N>
[[nodiscard]] constexpr bool operator>(const cstring<N>& lhs, std::string_view rhs) noexcept {
  return lhs.compare(rhs) > 0;
}

template <std::size_t N>
[[nodiscard]] constexpr bool operator>(std::string_view lhs, const cstring<N>& rhs) noexcept {
  return lhs.compare(rhs) > 0;
}

template <std::size_t N>
[[nodiscard]] constexpr bool operator>=(const cstring<N>& lhs, std::string_view rhs) noexcept {
  return lhs.compare(rhs) >= 0;
}

template <std::size_t N>
[[nodiscard]] constexpr bool operator>=(std::string_view lhs, const cstring<N>& rhs) noexcept {
  return lhs.compare(rhs) >= 0;
}

template <std::size_t N>
[[nodiscard]] constexpr bool operator<(const cstring<N>& lhs, std::string_view rhs) noexcept {
  return lhs.compare(rhs) < 0;
}

template <std::size_t N>
[[nodiscard]] constexpr bool operator<(std::string_view lhs, const cstring<N>& rhs) noexcept {
  return lhs.compare(rhs) < 0;
}

template <std::size_t N>
[[nodiscard]] constexpr bool operator<=(const cstring<N>& lhs, std::string_view rhs) noexcept {
  return lhs.compare(rhs) <= 0;
}

template <std::size_t N>
[[nodiscard]] constexpr bool operator<=(std::string_view lhs, const cstring<N>& rhs) noexcept {
  return lhs.compare(rhs) <= 0;
}

template <typename Char, typename Traits, std::size_t N>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, const cstring<N>& srt) {
  for (const auto c : std::string_view{srt}) {
    os.put(c);
  }

  return os;
}

namespace detail {

template <typename T>
struct identity {
  using type = T;
};

template <typename... T>
struct nameof_type_supported
#if defined(NAMEOF_TYPE_SUPPORTED) && NAMEOF_TYPE_SUPPORTED || defined(NAMEOF_TYPE_NO_CHECK_SUPPORT)
    : std::true_type {};
#else
    : std::false_type {};
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
using enable_if_enum_t = std::enable_if_t<std::is_enum_v<std::decay_t<T>>, R>;

template <typename T>
inline constexpr bool is_enum_v = std::is_enum_v<T> && std::is_same_v<T, std::decay_t<T>>;

constexpr std::string_view pretty_name(std::string_view name, bool remove_template_suffix = true) noexcept {
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

template <typename E, E V>
constexpr auto n() noexcept {
  static_assert(is_enum_v<E>, "nameof::detail::n requires enum type.");
#if defined(NAMEOF_ENUM_SUPPORTED) && NAMEOF_ENUM_SUPPORTED
#  if defined(__clang__) || defined(__GNUC__)
  constexpr auto name = pretty_name({__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2});
#  elif defined(_MSC_VER)
  constexpr auto name = pretty_name({__FUNCSIG__, sizeof(__FUNCSIG__) - 17});
#  endif
  if constexpr (name.size() > 0) {
    return cstring<name.size()>{name};
  } else {
    return std::string_view{};
  }
#else
  return std::string_view{}; // Unsupported compiler.
#endif
}

template <typename E, E V>
inline constexpr auto enum_name_v = n<E, V>();

template <typename E, auto V>
constexpr bool is_valid() noexcept {
  static_assert(is_enum_v<E>, "nameof::detail::is_valid requires enum type.");

  return n<E, static_cast<E>(V)>().size() != 0;
}

template <typename L, typename R>
constexpr bool cmp_less(L lhs, R rhs) noexcept {
  static_assert(std::is_integral_v<L> && std::is_integral_v<R>, "nameof::detail::cmp_less requires integral type.");

  if constexpr (std::is_signed_v<L> == std::is_signed_v<R>) {
    // If same signedness (both signed or both unsigned).
    return lhs < rhs;
  } else if constexpr (std::is_signed_v<R>) {
    // If 'right' is negative, then result is 'false', otherwise cast & compare.
    return rhs > 0 && lhs < static_cast<std::make_unsigned_t<R>>(rhs);
  } else {
    // If 'left' is negative, then result is 'true', otherwise cast & compare.
    return lhs < 0 || static_cast<std::make_unsigned_t<L>>(lhs) < rhs;
  }
}

template <typename E, int Min, int Max>
constexpr std::size_t range_size() noexcept {
  static_assert(is_enum_v<E>, "nameof::detail::range_size requires enum type.");
  constexpr auto size = Max - Min + 1;
  static_assert(size > 0, "nameof::enum_range requires valid size.");
  static_assert(size < (std::numeric_limits<std::uint16_t>::max)(), "nameof::enum_range requires valid size.");

  return static_cast<std::size_t>(size);
}

template <typename E>
constexpr int reflected_min() noexcept {
  static_assert(is_enum_v<E>, "nameof::detail::reflected_min requires enum type.");
  constexpr auto lhs = enum_range<E>::min;
  static_assert(lhs > (std::numeric_limits<std::int16_t>::min)(), "nameof::enum_range requires min must be greater than INT16_MIN.");
  constexpr auto rhs = (std::numeric_limits<std::underlying_type_t<E>>::min)();

  return cmp_less(lhs, rhs) ? rhs : lhs;
}

template <typename E>
constexpr int reflected_max() noexcept {
  static_assert(is_enum_v<E>, "nameof::detail::reflected_max requires enum type.");
  constexpr auto lhs = enum_range<E>::max;
  static_assert(lhs < (std::numeric_limits<std::int16_t>::max)(), "nameof::enum_range requires max must be less than INT16_MAX.");
  constexpr auto rhs = (std::numeric_limits<std::underlying_type_t<E>>::max)();

  return cmp_less(lhs, rhs) ? lhs : rhs;
}

template <typename E>
inline constexpr int reflected_min_v = reflected_min<E>();

template <typename E>
inline constexpr int reflected_max_v = reflected_max<E>();

template <typename E, int... I>
constexpr auto values(std::integer_sequence<int, I...>) noexcept {
  static_assert(is_enum_v<E>, "nameof::detail::values requires enum type.");
  constexpr std::array<bool, sizeof...(I)> valid{{is_valid<E, I + reflected_min_v<E>>()...}};
  constexpr int count = ((valid[I] ? 1 : 0) + ...);

  std::array<E, count> values{};
  for (int i = 0, v = 0; v < count; ++i) {
    if (valid[i]) {
      values[v++] = static_cast<E>(i + reflected_min_v<E>);
    }
  }

  return values;
}

template <typename E>
inline constexpr auto values_v = values<E>(std::make_integer_sequence<int, range_size<E, reflected_min_v<E>, reflected_max_v<E>>()>{});

template <typename E>
inline constexpr std::size_t count_v = values_v<E>.size();

template <typename E>
inline constexpr int min_v = static_cast<int>(values_v<E>.front());

template <typename E>
inline constexpr int max_v = static_cast<int>(values_v<E>.back());

template <typename E>
inline constexpr std::size_t range_size_v = range_size<E, min_v<E>, max_v<E>>();

template <typename E>
using index_t = std::conditional_t<range_size_v<E> < (std::numeric_limits<std::uint8_t>::max)(), std::uint8_t, std::uint16_t>;

template <typename E>
inline constexpr auto invalid_index_v = (std::numeric_limits<index_t<E>>::max)();

template <typename E, int... I>
constexpr auto indexes(std::integer_sequence<int, I...>) noexcept {
  static_assert(is_enum_v<E>, "nameof::detail::indexes requires enum type.");
  [[maybe_unused]] index_t<E> i = 0;

  return std::array<index_t<E>, sizeof...(I)>{{(is_valid<E, I + min_v<E>>() ? i++ : invalid_index_v<E>)...}};
}

template <typename E>
inline constexpr auto indexes_v = indexes<E>(std::make_integer_sequence<int, range_size_v<E>>{});

template <typename E, int... I>
constexpr auto strings(std::integer_sequence<int, I...>) noexcept {
  static_assert(is_enum_v<E>, "nameof::detail::strings requires enum type.");

  return std::array<const char*, sizeof...(I)>{{enum_name_v<E, static_cast<E>(I + min_v<E>)>.data()...}};
}

template <typename E, std::size_t... I>
constexpr auto strings(std::index_sequence<I...>) noexcept {
  static_assert(is_enum_v<E>, "nameof::detail::strings requires enum type.");

  return std::array<const char*, sizeof...(I)>{{enum_name_v<E, values_v<E>[I]>.data()...}};
}

template <typename E>
inline constexpr bool sparsity_v = (sizeof(const char*) * range_size_v<E>) > (sizeof(index_t<E>) * range_size_v<E> + sizeof(const char*) * count_v<E>);

template <typename E>
constexpr auto strings() noexcept {
  static_assert(is_enum_v<E>, "nameof::detail::strings requires enum type.");

  if constexpr (sparsity_v<E>) {
    return strings<E>(std::make_index_sequence<count_v<E>>{});
  } else {
    return strings<E>(std::make_integer_sequence<int, range_size_v<E>>{});
  }
}

template <typename E>
inline static constexpr auto strings_v = strings<E>();

template <typename... T>
constexpr auto n() noexcept {
#if defined(NAMEOF_TYPE_SUPPORTED) && NAMEOF_TYPE_SUPPORTED
#  if defined(__clang__)
  constexpr std::string_view name{__PRETTY_FUNCTION__ + 31, sizeof(__PRETTY_FUNCTION__) - 34};
#  elif defined(__GNUC__)
  constexpr std::string_view name{__PRETTY_FUNCTION__ + 46, sizeof(__PRETTY_FUNCTION__) - 49};
#  elif defined(_MSC_VER)
  constexpr std::string_view name{__FUNCSIG__ + 63, sizeof(__FUNCSIG__) - 81 - (__FUNCSIG__[sizeof(__FUNCSIG__) - 19] == ' ' ? 1 : 0)};
#  endif

  return cstring<name.size()>{name};
#else
  return std::string_view{}; // Unsupported compiler.
#endif
}

template <typename... T>
inline constexpr auto type_name_v = n<T...>();

#if __has_include(<cxxabi.h>)
inline std::string demangle(const char* tn) {
  if (tn == nullptr) {
    return {};
  }

  auto dmg = abi::__cxa_demangle(tn, nullptr, nullptr, nullptr);
  auto r = std::string{dmg != nullptr ? dmg : tn};
  std::free(dmg);
  return r;
}
#else
constexpr std::string_view demangle(const char* tn) noexcept {
  return {tn};
}
#endif

} // namespace nameof::detail

// Checks is nameof_type supported compiler.
inline constexpr bool is_nameof_type_supported = detail::nameof_type_supported<void>::value;

// Checks is nameof_enum supported compiler.
inline constexpr bool is_nameof_enum_supported = detail::nameof_enum_supported<void>::value;

// Obtains simple (unqualified) string enum name of enum variable.
template <typename E>
[[nodiscard]] constexpr auto nameof_enum(E value) noexcept -> detail::enable_if_enum_t<E, std::string_view> {
  using D = std::decay_t<E>;
  using U = std::underlying_type_t<D>;
  static_assert(detail::nameof_enum_supported<D>::value, "nameof::nameof_enum unsupported compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  static_assert(detail::count_v<D> > 0, "nameof::nameof_enum requires enum implementation and valid max and min.");

  if (const auto i = static_cast<int>(value) - detail::min_v<D>; static_cast<U>(value) >= static_cast<U>(detail::min_v<D>) &&
                                                                 static_cast<U>(value) <= static_cast<U>(detail::max_v<D>)) {
    if constexpr (detail::sparsity_v<D>) {
      if (const auto idx = detail::indexes_v<D>[i]; idx != detail::invalid_index_v<D>) {
        return detail::strings_v<D>[idx];
      }
    } else {
      return detail::strings_v<D>[i];
    }
  }

  return {}; // Value out of range.
}

// Obtains simple (unqualified) string enum name of static storage enum variable.
// This version is much lighter on the compile times and is not restricted to the enum_range limitation.
template <auto V>
[[nodiscard]] constexpr auto nameof_enum() noexcept -> detail::enable_if_enum_t<decltype(V), std::string_view> {
  using D = std::decay_t<decltype(V)>;
  static_assert(detail::nameof_enum_supported<D>::value, "nameof::nameof_enum unsupported compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  constexpr std::string_view name = detail::enum_name_v<D, V>;
  static_assert(name.size() > 0, "Enum value does not have a name.");

  return name;
}

// Obtains string name of type, reference and cv-qualifiers are ignored.
template <typename T>
[[nodiscard]] constexpr std::string_view nameof_type() noexcept {
  static_assert(detail::nameof_type_supported<T>::value, "nameof::nameof_type unsupported compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
#if defined(_MSC_VER)
  using U = detail::identity<detail::remove_cvref_t<T>>;
#else
  using U = detail::remove_cvref_t<T>;
#endif
  constexpr std::string_view name = detail::type_name_v<U>;
  static_assert(name.size() > 0, "Type does not have a name.");

  return name;
}

// Obtains string name of full type, with reference and cv-qualifiers.
template <typename T>
[[nodiscard]] constexpr std::string_view nameof_full_type() noexcept {
  static_assert(detail::nameof_type_supported<T>::value, "nameof::nameof_type unsupported compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
#if defined(_MSC_VER)
  using U = detail::identity<T>;
#else
  using U = T;
#endif
  constexpr std::string_view name = detail::type_name_v<U>;
  static_assert(name.size() > 0, "Type does not have a name.");

  return name;
}

} // namespace nameof

// Obtains simple (unqualified) string name of variable, function, macro.
#define NAMEOF(...) []() constexpr noexcept {                              \
  ::std::void_t<decltype(__VA_ARGS__)>();                                  \
  constexpr auto name = ::nameof::detail::pretty_name(#__VA_ARGS__, true); \
  static_assert(name.size() > 0, "Expression does not have a name.");      \
  constexpr auto size = name.size();                                       \
  constexpr auto nameof = ::nameof::cstring<size>{name};                   \
  return nameof; }()

// Obtains simple (unqualified) full (with template suffix) string name of variable, function, macro.
#define NAMEOF_FULL(...) []() constexpr noexcept {                          \
  ::std::void_t<decltype(__VA_ARGS__)>();                                   \
  constexpr auto name = ::nameof::detail::pretty_name(#__VA_ARGS__, false); \
  static_assert(name.size() > 0, "Expression does not have a name.");       \
  constexpr auto size = name.size();                                        \
  constexpr auto nameof_full = ::nameof::cstring<size>{name};               \
  return nameof_full; }()

// Obtains raw string name of variable, function, macro.
#define NAMEOF_RAW(...) []() constexpr noexcept {                     \
  ::std::void_t<decltype(__VA_ARGS__)>();                             \
  constexpr auto name = ::std::string_view{#__VA_ARGS__};             \
  static_assert(name.size() > 0, "Expression does not have a name."); \
  constexpr auto size = name.size();                                  \
  constexpr auto nameof_raw = ::nameof::cstring<size>{name};          \
  return nameof_raw; }()

// Obtains simple (unqualified) string enum name of enum variable.
#define NAMEOF_ENUM(...) ::nameof::nameof_enum<::std::decay_t<decltype(__VA_ARGS__)>>(__VA_ARGS__)

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

// Obtains string name of type, using RTTI.
#define NAMEOF_TYPE_RTTI(...) ::nameof::detail::demangle(typeid(__VA_ARGS__).name())

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

#endif // NEARGYE_NAMEOF_HPP
