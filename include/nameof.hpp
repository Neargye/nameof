//  _   _                             __    _____
// | \ | |                           / _|  / ____|_     _
// |  \| | __ _ _ __ ___   ___  ___ | |_  | |   _| |_ _| |_
// | . ` |/ _` | '_ ` _ \ / _ \/ _ \|  _| | |  |_   _|_   _|
// | |\  | (_| | | | | | |  __/ (_) | |   | |____|_|   |_|
// |_| \_|\__,_|_| |_| |_|\___|\___/|_|    \_____|
// https://github.com/Neargye/nameof
// version 0.10.0
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

#define NAMEOF_VERSION_MAJOR 0
#define NAMEOF_VERSION_MINOR 10
#define NAMEOF_VERSION_PATCH 0

#include <array>
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <iosfwd>
#include <iterator>
#include <limits>
#include <type_traits>
#include <utility>

#if !defined(NAMEOF_USING_ALIAS_STRING)
#include <string>
#endif
#if !defined(NAMEOF_USING_ALIAS_STRING_VIEW)
#include <string_view>
#endif

#if __has_include(<cxxabi.h>)
#include <cxxabi.h>
#include <cstdlib>
#endif

#if defined(__clang__)
#  pragma clang diagnostic push
#elif defined(__GNUC__)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wstringop-overflow" // Missing terminating nul 'enum_name_v'.
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 26495) // Variable 'cstring<N>::chars_' is uninitialized.
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

// Checks nameof_enum compiler aliases compatibility.
#if defined(__clang__) && __clang_major__ >= 5 || defined(__GNUC__) && __GNUC__ >= 9 || defined(_MSC_VER) && _MSC_VER >= 1920
#  undef  NAMEOF_ENUM_SUPPORTED_ALIASES
#  define NAMEOF_ENUM_SUPPORTED_ALIASES 1
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

// If need another string_view type, define the macro NAMEOF_USING_ALIAS_STRING_VIEW.
#if defined(NAMEOF_USING_ALIAS_STRING_VIEW)
NAMEOF_USING_ALIAS_STRING_VIEW
#else
using string_view = std::string_view;
#endif

// If need another string type, define the macro NAMEOF_USING_ALIAS_STRING.
#if defined(NAMEOF_USING_ALIAS_STRING)
NAMEOF_USING_ALIAS_STRING
#else
using string = std::string;
#endif

namespace customize {

// Enum value must be in range [NAMEOF_ENUM_RANGE_MIN, NAMEOF_ENUM_RANGE_MAX]. By default NAMEOF_ENUM_RANGE_MIN = -128, NAMEOF_ENUM_RANGE_MAX = 128.
// If need another range for all enum types by default, redefine the macro NAMEOF_ENUM_RANGE_MIN and NAMEOF_ENUM_RANGE_MAX.
// If need another range for specific enum type, add specialization enum_range for necessary enum type.
template <typename E>
struct enum_range {
  static_assert(std::is_enum_v<E>, "nameof::customize::enum_range requires enum type.");
  inline static constexpr int min = NAMEOF_ENUM_RANGE_MIN;
  inline static constexpr int max = NAMEOF_ENUM_RANGE_MAX;
  static_assert(max > min, "nameof::customize::enum_range requires max > min.");
};

static_assert(NAMEOF_ENUM_RANGE_MIN <= 0, "NAMEOF_ENUM_RANGE_MIN must be less or equals than 0.");
static_assert(NAMEOF_ENUM_RANGE_MIN > (std::numeric_limits<std::int16_t>::min)(), "NAMEOF_ENUM_RANGE_MIN must be greater than INT16_MIN.");

static_assert(NAMEOF_ENUM_RANGE_MAX > 0, "NAMEOF_ENUM_RANGE_MAX must be greater than 0.");
static_assert(NAMEOF_ENUM_RANGE_MAX < (std::numeric_limits<std::int16_t>::max)(), "NAMEOF_ENUM_RANGE_MAX must be less than INT16_MAX.");

static_assert(NAMEOF_ENUM_RANGE_MAX > NAMEOF_ENUM_RANGE_MIN, "NAMEOF_ENUM_RANGE_MAX must be greater than NAMEOF_ENUM_RANGE_MIN.");

// If need cunstom names for enum, add specialization enum_name for necessary enum type.
template <typename E>
constexpr string_view enum_name(E) noexcept {
  static_assert(std::is_enum_v<E>, "nameof::customize::enum_name requires enum type.");

  return {};
}

// If need cunstom name for type, add specialization type_name for necessary type.
template <typename T>
constexpr string_view type_name() noexcept {
  return {};
}

} // namespace nameof::customize

template <std::size_t N>
class [[nodiscard]] cstring {
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

  constexpr explicit cstring(string_view str) noexcept : cstring{str, std::make_index_sequence<N>{}} {
    assert(str.size() > 0 && str.size() == N);
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

  [[nodiscard]] constexpr int compare(string_view str) const noexcept {
    return string_view{data(), size()}.compare(str);
  }

  [[nodiscard]] constexpr const char* c_str() const noexcept { return data(); }

  [[nodiscard]] string str() const { return {begin(), end()}; }

  [[nodiscard]] constexpr operator string_view() const noexcept { return {data(), size()}; }

  [[nodiscard]] constexpr explicit operator const_pointer() const noexcept { return data(); }

  [[nodiscard]] explicit operator string() const { return {begin(), end()}; }

 private:
  template <std::size_t... I>
  constexpr cstring(string_view str, std::index_sequence<I...>) noexcept : chars_{{str[I]..., '\0'}} {}

  std::array<char, N + 1> chars_;
};

template <std::size_t N>
[[nodiscard]] constexpr bool operator==(const cstring<N>& lhs, string_view rhs) noexcept {
  return lhs.compare(rhs) == 0;
}

template <std::size_t N>
[[nodiscard]] constexpr bool operator==(string_view lhs, const cstring<N>& rhs) noexcept {
  return lhs.compare(rhs) == 0;
}

template <std::size_t N>
[[nodiscard]] constexpr bool operator!=(const cstring<N>& lhs, string_view rhs) noexcept {
  return lhs.compare(rhs) != 0;
}

template <std::size_t N>
[[nodiscard]] constexpr bool operator!=(string_view lhs, const cstring<N>& rhs) noexcept {
  return lhs.compare(rhs) != 0;
}

template <std::size_t N>
[[nodiscard]] constexpr bool operator>(const cstring<N>& lhs, string_view rhs) noexcept {
  return lhs.compare(rhs) > 0;
}

template <std::size_t N>
[[nodiscard]] constexpr bool operator>(string_view lhs, const cstring<N>& rhs) noexcept {
  return lhs.compare(rhs) > 0;
}

template <std::size_t N>
[[nodiscard]] constexpr bool operator>=(const cstring<N>& lhs, string_view rhs) noexcept {
  return lhs.compare(rhs) >= 0;
}

template <std::size_t N>
[[nodiscard]] constexpr bool operator>=(string_view lhs, const cstring<N>& rhs) noexcept {
  return lhs.compare(rhs) >= 0;
}

template <std::size_t N>
[[nodiscard]] constexpr bool operator<(const cstring<N>& lhs, string_view rhs) noexcept {
  return lhs.compare(rhs) < 0;
}

template <std::size_t N>
[[nodiscard]] constexpr bool operator<(string_view lhs, const cstring<N>& rhs) noexcept {
  return lhs.compare(rhs) < 0;
}

template <std::size_t N>
[[nodiscard]] constexpr bool operator<=(const cstring<N>& lhs, string_view rhs) noexcept {
  return lhs.compare(rhs) <= 0;
}

template <std::size_t N>
[[nodiscard]] constexpr bool operator<=(string_view lhs, const cstring<N>& rhs) noexcept {
  return lhs.compare(rhs) <= 0;
}

template <typename Char, typename Traits, std::size_t N>
std::basic_ostream<Char, Traits>& operator<<(std::basic_ostream<Char, Traits>& os, const cstring<N>& srt) {
  for (const auto c : srt) {
    os.put(c);
  }

  return os;
}

namespace detail {

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
  if (remove_suffix) {
    name.remove_suffix(s);
  }

  if (name.size() > 0 && ((name.front() >= 'a' && name.front() <= 'z') ||
                          (name.front() >= 'A' && name.front() <= 'Z') ||
                          (name.front() == '_'))) {
    return name;
  }

  return {}; // Invalid name.
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

template <typename I>
constexpr I log2(I value) noexcept {
  static_assert(std::is_integral_v<I>, "nameof::detail::log2 requires integral type.");

  auto ret = I{0};
  for (; value > I{1}; value >>= I{1}, ++ret) {}

  return ret;
}

template <typename T>
struct nameof_enum_supported
#if defined(NAMEOF_ENUM_SUPPORTED) && NAMEOF_ENUM_SUPPORTED || defined(NAMEOF_ENUM_NO_CHECK_SUPPORT)
    : std::true_type {};
#else
    : std::false_type {};
#endif

template <typename T, typename R>
using enable_if_enum_t = std::enable_if_t<std::is_enum_v<std::decay_t<T>>, R>;

template <typename T>
inline constexpr bool is_enum_v = std::is_enum_v<T> && std::is_same_v<T, std::decay_t<T>>;

template <typename E, E V>
constexpr auto n() noexcept {
  static_assert(is_enum_v<E>, "nameof::detail::n requires enum type.");
  constexpr auto custom_name = customize::enum_name<E>(V);

  if constexpr (custom_name.empty()) {
    static_cast<void>(custom_name);
#if defined(NAMEOF_ENUM_SUPPORTED) && NAMEOF_ENUM_SUPPORTED
#  if defined(__clang__) || defined(__GNUC__)
    constexpr auto name = pretty_name({__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2});
#  elif defined(_MSC_VER)
    constexpr auto name = pretty_name({__FUNCSIG__, sizeof(__FUNCSIG__) - 17});
#  endif
    if constexpr (name.size() > 0) {
      return cstring<name.size()>{name};
    } else {
      return string_view{};
    }
#else
  return string_view{}; // Unsupported compiler.
#endif
  } else {
    return cstring<custom_name.size()>{custom_name};
  }
}

template <typename E, E V>
inline constexpr auto enum_name_v = n<E, V>();

template <typename E, auto V>
constexpr bool is_valid() noexcept {
  static_assert(is_enum_v<E>, "nameof::detail::is_valid requires enum type.");

  return n<E, static_cast<E>(V)>().size() != 0;
}

template <typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
constexpr int reflected_min() noexcept {
  static_assert(is_enum_v<E>, "nameof::detail::reflected_min requires enum type.");

  if constexpr (IsFlags) {
    return 0;
  } else {
    constexpr auto lhs = customize::enum_range<E>::min;
    static_assert(lhs > (std::numeric_limits<std::int16_t>::min)(), "nameof::enum_range requires min must be greater than INT16_MIN.");
    constexpr auto rhs = (std::numeric_limits<U>::min)();

    if constexpr (cmp_less(lhs, rhs)) {
      return rhs;
    } else {
      return lhs;
    }
  }
}

template <typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
constexpr int reflected_max() noexcept {
  static_assert(is_enum_v<E>, "nameof::detail::reflected_max requires enum type.");

  if constexpr (IsFlags) {
    return std::numeric_limits<U>::digits - 1;
  } else {
    constexpr auto lhs = customize::enum_range<E>::max;
    static_assert(lhs < (std::numeric_limits<std::int16_t>::max)(), "nameof::enum_range requires max must be less than INT16_MAX.");
    constexpr auto rhs = (std::numeric_limits<U>::max)();

    if constexpr (cmp_less(lhs, rhs)) {
      return lhs;
    } else {
      return rhs;
    }
  }
}

template <typename E, bool IsFlags = false>
inline constexpr auto reflected_min_v = reflected_min<E, IsFlags>();

template <typename E, bool IsFlags = false>
inline constexpr auto reflected_max_v = reflected_max<E, IsFlags>();

template <typename E, int O, bool IsFlags = false, typename U = std::underlying_type_t<E>>
constexpr E value(std::size_t i) noexcept {
  static_assert(is_enum_v<E>, "nameof::detail::value requires enum type.");

  if constexpr (IsFlags) {
    return static_cast<E>(U{1} << static_cast<U>(static_cast<int>(i) + O));
  } else {
    return static_cast<E>(static_cast<int>(i) + O);
  }
}

template <std::size_t N>
constexpr std::size_t values_count(const std::array<bool, N>& valid) noexcept {
  auto count = std::size_t{0};
  for (std::size_t i = 0; i < valid.size(); ++i) {
    if (valid[i]) {
      ++count;
    }
  }

  return count;
}

template <typename E, bool IsFlags, int Min, std::size_t... I>
constexpr auto values(std::index_sequence<I...>) noexcept {
  static_assert(is_enum_v<E>, "nameof::detail::values requires enum type.");
  constexpr std::array<bool, sizeof...(I)> valid{{is_valid<E, value<E, Min, IsFlags>(I)>()...}};
  constexpr std::size_t count = values_count(valid);

  std::array<E, count> values{};
  for (std::size_t i = 0, v = 0; v < count; ++i) {
    if (valid[i]) {
      values[v++] = value<E, Min, IsFlags>(i);
    }
  }

  return values;
}

template <typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
constexpr auto values() noexcept {
  static_assert(is_enum_v<E>, "nameof::detail::values requires enum type.");
  constexpr auto min = reflected_min_v<E, IsFlags>;
  constexpr auto max = reflected_max_v<E, IsFlags>;
  constexpr auto range_size = max - min + 1;
  static_assert(range_size > 0, "nameof::enum_range requires valid size.");
  static_assert(range_size < (std::numeric_limits<std::uint16_t>::max)(), "nameof::enum_range requires valid size.");
  if constexpr (cmp_less((std::numeric_limits<U>::min)(), min) && !IsFlags) {
    static_assert(!is_valid<E, value<E, min - 1, IsFlags>(0)>(), "nameof::enum_range detects enum value smaller than min range size.");
  }
  if constexpr (cmp_less(range_size, (std::numeric_limits<U>::max)()) && !IsFlags) {
    static_assert(!is_valid<E, value<E, min, IsFlags>(range_size + 1)>(), "nameof::enum_range detects enum value larger than max range size.");
  }

  return values<E, IsFlags, reflected_min_v<E, IsFlags>>(std::make_index_sequence<range_size>{});
}

template <typename E, bool IsFlags = false>
inline constexpr auto values_v = values<E, IsFlags>();

template <typename E, bool IsFlags = false, typename D = std::decay_t<E>>
using values_t = decltype((values_v<D, IsFlags>));

template <typename E, bool IsFlags = false>
inline constexpr auto count_v = values_v<E, IsFlags>.size();

template <typename E, bool IsFlags = false, typename U = std::underlying_type_t<E>>
inline constexpr auto min_v = static_cast<U>(values_v<E, IsFlags>.front());

template <typename E, bool IsFlags = false, typename U = std::underlying_type_t<E>>
inline constexpr auto max_v = static_cast<U>(values_v<E, IsFlags>.back());

template <typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
constexpr std::size_t range_size() noexcept {
  static_assert(is_enum_v<E>, "nameof::detail::range_size requires enum type.");
  constexpr auto max = IsFlags ? log2(max_v<E, IsFlags>) : max_v<E, IsFlags>;
  constexpr auto min = IsFlags ? log2(min_v<E, IsFlags>) : min_v<E, IsFlags>;
  constexpr auto range_size = max - min + U{1};
  static_assert(range_size > 0, "nameof::enum_range requires valid size.");
  static_assert(range_size < (std::numeric_limits<std::uint16_t>::max)(), "nameof::enum_range requires valid size.");

  return static_cast<std::size_t>(range_size);
}

template <typename E, bool IsFlags = false>
inline constexpr auto range_size_v = range_size<E, IsFlags>();

template <typename E, bool IsFlags = false>
using index_t = std::conditional_t<range_size_v<E, IsFlags> < (std::numeric_limits<std::uint8_t>::max)(), std::uint8_t, std::uint16_t>;

template <typename E, bool IsFlags = false>
inline constexpr auto invalid_index_v = (std::numeric_limits<index_t<E, IsFlags>>::max)();

template <typename E, bool IsFlags, std::size_t... I>
constexpr auto indexes(std::index_sequence<I...>) noexcept {
  static_assert(is_enum_v<E>, "nameof::detail::indexes requires enum type.");
  constexpr auto min = IsFlags ? log2(min_v<E, IsFlags>) : min_v<E, IsFlags>;
  [[maybe_unused]] auto i = index_t<E, IsFlags>{0};

  return std::array<decltype(i), sizeof...(I)>{{(is_valid<E, value<E, min, IsFlags>(I)>() ? i++ : invalid_index_v<E, IsFlags>)...}};
}

template <typename E, bool IsFlags = false>
inline constexpr auto indexes_v = indexes<E, IsFlags>(std::make_index_sequence<range_size_v<E, IsFlags>>{});

template <typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
constexpr bool is_sparse() noexcept {
  static_assert(is_enum_v<E>, "nameof::detail::is_sparse requires enum type.");

  if constexpr (IsFlags) {
    return (sizeof(const char*) * range_size_v<E, IsFlags>) > (sizeof(E) * count_v<E, IsFlags> + sizeof(const char*) * count_v<E, IsFlags>);
  } else {
    return (sizeof(const char*) * range_size_v<E, IsFlags>) > (sizeof(index_t<E>) * range_size_v<E, IsFlags> + sizeof(const char*) * count_v<E, IsFlags>);
  }
}

template <typename E, bool IsFlags = false>
inline constexpr bool is_sparse_v = is_sparse<E, IsFlags>();

template <typename E, bool IsFlags, typename U = std::underlying_type_t<E>>
[[nodiscard]] constexpr E get_value(std::size_t i) noexcept {
  static_assert(is_enum_v<E>, "nameof::detail::strings requires enum type.");

  if constexpr (is_sparse_v<E, IsFlags>) {
    return values_v<E, IsFlags>[i];
  } else {
    constexpr auto min = IsFlags ? log2(min_v<E, IsFlags>) : min_v<E, IsFlags>;

    return value<E, min, IsFlags>(i);
  }
}

template <typename E, bool IsFlags, std::size_t... I>
constexpr auto strings(std::index_sequence<I...>) noexcept {
  static_assert(is_enum_v<E>, "nameof::detail::strings requires enum type.");

  return std::array<const char*, sizeof...(I)>{{enum_name_v<E, get_value<E, IsFlags>(I)>.data()...}};
}

template <typename E, bool IsFlags>
constexpr auto strings() noexcept {
  static_assert(is_enum_v<E>, "nameof::detail::strings requires enum type.");
  constexpr auto count = is_sparse_v<E, IsFlags> ? count_v<E, IsFlags> : range_size_v<E, IsFlags>;

  return strings<E, IsFlags>(std::make_index_sequence<count>{});
}

template <typename E, bool IsFlags = false>
inline static constexpr auto strings_v = strings<E, IsFlags>();

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

#if defined(_MSC_VER) && !defined(__clang__)
template <typename T>
struct identity {
  using type = T;
};
#else
template <typename T>
using identity = T;
#endif

template <typename T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template <typename T, typename R>
using enable_if_has_short_name_t = std::enable_if_t<!std::is_array_v<T> && !std::is_pointer_v<T>, R>;

template <typename... T>
constexpr auto n() noexcept {
#  if defined(_MSC_VER) && !defined(__clang__)
  constexpr auto custom_name = customize::type_name<typename T::type...>();
#else
  constexpr auto custom_name = customize::type_name<T...>();
#  endif

  if constexpr (custom_name.empty()) {
    static_cast<void>(custom_name);
#if defined(NAMEOF_TYPE_SUPPORTED) && NAMEOF_TYPE_SUPPORTED
#  if defined(__clang__)
    constexpr string_view name{__PRETTY_FUNCTION__ + 31, sizeof(__PRETTY_FUNCTION__) - 34};
#  elif defined(__GNUC__)
    constexpr string_view name{__PRETTY_FUNCTION__ + 46, sizeof(__PRETTY_FUNCTION__) - 49};
#  elif defined(_MSC_VER)
    constexpr string_view name{__FUNCSIG__ + 63, sizeof(__FUNCSIG__) - 81 - (__FUNCSIG__[sizeof(__FUNCSIG__) - 19] == ' ' ? 1 : 0)};
#  endif

    return cstring<name.size()>{name};
#else
    return string_view{}; // Unsupported compiler.
#endif
  } else {
    return cstring<custom_name.size()>{custom_name};
  }
}

template <typename... T>
inline constexpr auto type_name_v = n<T...>();

#if __has_include(<cxxabi.h>)
template <typename T>
string nameof_type_rtti(const char* tn) {
  static_assert(nameof_type_rtti_supported<T>::value, "nameof::nameof_type_rtti unsupported compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  const auto dmg = abi::__cxa_demangle(tn, nullptr, nullptr, nullptr);
  const auto name = string{dmg};
  std::free(dmg);
  assert(name.size() > 0 && "Type does not have a name.");

  return name;
}

template <typename T>
string nameof_full_type_rtti(const char* tn) {
static_assert(nameof_type_rtti_supported<T>::value, "nameof::nameof_type_rtti unsupported compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  const auto dmg = abi::__cxa_demangle(tn, nullptr, nullptr, nullptr);
  auto name = string{dmg};
  std::free(dmg);
  assert(name.size() > 0 && "Type does not have a name.");
  if constexpr (std::is_const_v<std::remove_reference_t<T>>) {
    name = "const " + name;
  }
  if constexpr (std::is_volatile_v<std::remove_reference_t<T>>) {
    name = "volatile " + name;
  }
  if constexpr (std::is_lvalue_reference_v<T>) {
    name += '&';
  }
  if constexpr (std::is_rvalue_reference_v<T>) {
    name += "&&";
  }

  return name;
}

template <typename T, enable_if_has_short_name_t<T, int> = 0>
string nameof_short_type_rtti(const char* tn) {
  static_assert(nameof_type_rtti_supported<T>::value, "nameof::nameof_type_rtti unsupported compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  const auto dmg = abi::__cxa_demangle(tn, nullptr, nullptr, nullptr);
  const auto name = string{pretty_name(dmg)};
  std::free(dmg);
  assert(name.size() > 0 && "Type does not have a short name.");

  return name;
}
#else
template <typename T>
string nameof_type_rtti(const char* tn) noexcept {
  static_assert(nameof_type_rtti_supported<T>::value, "nameof::nameof_type_rtti unsupported compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  const auto name = string_view{tn};
  assert(name.size() > 0 && "Type does not have a name.");

  return {name.begin(), name.end()};
}

template <typename T>
string nameof_full_type_rtti(const char* tn) noexcept {
static_assert(nameof_type_rtti_supported<T>::value, "nameof::nameof_type_rtti unsupported compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  auto name = string{tn};
  assert(name.size() > 0 && "Type does not have a name.");
  if constexpr (std::is_const_v<std::remove_reference_t<T>>) {
    name = "const " + name;
  }
  if constexpr (std::is_volatile_v<std::remove_reference_t<T>>) {
    name = "volatile " + name;
  }
  if constexpr (std::is_lvalue_reference_v<T>) {
    name += '&';
  }
  if constexpr (std::is_rvalue_reference_v<T>) {
    name += "&&";
  }

  return name;
}

template <typename T, enable_if_has_short_name_t<T, int> = 0>
string nameof_short_type_rtti(const char* tn) noexcept {
  static_assert(nameof_type_rtti_supported<T>::value, "nameof::nameof_type_rtti unsupported compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  const auto name = pretty_name(tn);
  assert(name.size() > 0 && "Type does not have a short name.");

  return {name.begin(), name.end()};
}
#endif

} // namespace nameof::detail

// Checks is nameof_type supported compiler.
inline constexpr bool is_nameof_type_supported = detail::nameof_type_supported<void>::value;

// Checks is nameof_type_rtti supported compiler.
inline constexpr bool is_nameof_type_rtti_supported = detail::nameof_type_rtti_supported<void>::value;

// Checks is nameof_enum supported compiler.
inline constexpr bool is_nameof_enum_supported = detail::nameof_enum_supported<void>::value;

// Obtains simple (unqualified) name of enum variable.
template <typename E>
[[nodiscard]] constexpr auto nameof_enum(E value) noexcept -> detail::enable_if_enum_t<E, string_view> {
  using D = std::decay_t<E>;
  using U = std::underlying_type_t<D>;
  static_assert(detail::nameof_enum_supported<D>::value, "nameof::nameof_enum unsupported compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  static_assert(detail::count_v<D> > 0, "nameof::nameof_enum requires enum implementation and valid max and min.");

  const bool valid = static_cast<U>(value) >= static_cast<U>(detail::min_v<D>) && static_cast<U>(value) <= static_cast<U>(detail::max_v<D>);
  if (const auto i = static_cast<int>(value) - detail::min_v<D>; valid) {
    if constexpr (detail::is_sparse_v<D>) {
      if (const auto idx = detail::indexes_v<D>[i]; idx != detail::invalid_index_v<D>) {
        return detail::strings_v<D>[idx];
      }
    } else {
      return detail::strings_v<D>[i];
    }
  }

  assert(valid && "enum variable does not have a name.");
  return {}; // Value out of range.
}

// Obtains simple (unqualified) name of enum-flags variable.
template <typename E>
[[nodiscard]] auto nameof_enum_flag(E value) -> detail::enable_if_enum_t<E, string> {
  using D = std::decay_t<E>;
  using U = std::underlying_type_t<D>;
  static_assert(detail::nameof_enum_supported<D>::value, "nameof::nameof_enum_flag unsupported compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  static_assert(detail::count_v<D, true> > 0, "nameof::nameof_enum_flag requires enum-flags implementation.");
  constexpr auto size = detail::is_sparse_v<D, true> ? detail::count_v<D, true> : detail::range_size_v<D, true>;

  string name;
  auto check_value = U{0};
  for (std::size_t i = 0; i < size; ++i) {
    if (const auto v = static_cast<U>(detail::get_value<D, true>(i)); (static_cast<U>(value) & v) != 0) {
      if (const auto n = detail::strings_v<D, true>[i]; n != nullptr) {
        check_value |= v;
        if (!name.empty()) {
          name.append(1, '|');
        }
        name.append(n);
      } else {
        return {}; // Value out of range.
      }
    }
  }

  const bool valid = check_value != 0 && check_value == static_cast<U>(value);
  if (valid) {
    return name;
  }

  assert(valid && "enum-flags variable does not have a name.");
  return {}; // Invalid value or out of range.
}

// Obtains simple (unqualified) name of static storage enum variable.
// This version is much lighter on the compile times and is not restricted to the enum_range limitation.
template <auto V>
[[nodiscard]] constexpr auto nameof_enum() noexcept -> detail::enable_if_enum_t<decltype(V), string_view> {
  using D = std::decay_t<decltype(V)>;
  static_assert(detail::nameof_enum_supported<D>::value, "nameof::nameof_enum unsupported compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  constexpr string_view name = detail::enum_name_v<D, V>;
  static_assert(name.size() > 0, "Enum value does not have a name.");

  return name;
}

// Obtains name of type, reference and cv-qualifiers are ignored.
template <typename T>
[[nodiscard]] constexpr string_view nameof_type() noexcept {
  static_assert(detail::nameof_type_supported<T>::value, "nameof::nameof_type unsupported compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  using U = detail::identity<detail::remove_cvref_t<T>>;
  constexpr string_view name = detail::type_name_v<U>;
  static_assert(name.size() > 0, "Type does not have a name.");

  return name;
}

// Obtains full name of type, with reference and cv-qualifiers.
template <typename T>
[[nodiscard]] constexpr string_view nameof_full_type() noexcept {
  static_assert(detail::nameof_type_supported<T>::value, "nameof::nameof_type unsupported compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  using U = detail::identity<T>;
  constexpr string_view name = detail::type_name_v<U>;
  static_assert(name.size() > 0, "Type does not have a full name.");

  return name;
}

// Obtains short name of type.
template <typename T>
[[nodiscard]] constexpr auto nameof_short_type() noexcept -> detail::enable_if_has_short_name_t<T, string_view> {
  static_assert(detail::nameof_type_supported<T>::value, "nameof::nameof_type unsupported compiler (https://github.com/Neargye/nameof#compiler-compatibility).");
  using U = detail::identity<detail::remove_cvref_t<T>>;
  constexpr string_view name = detail::pretty_name(detail::type_name_v<U>);
  static_assert(name.size() > 0, "Type does not have a short name.");

  return name;
}

} // namespace nameof

// Obtains name of variable, function, macro.
#define NAMEOF(...) []() constexpr noexcept {                          \
  ::std::void_t<decltype(__VA_ARGS__)>();                              \
  constexpr auto _name = ::nameof::detail::pretty_name(#__VA_ARGS__);  \
  static_assert(_name.size() > 0, "Expression does not have a name."); \
  constexpr auto _size = _name.size();                                 \
  constexpr auto _nameof = ::nameof::cstring<_size>{_name};            \
  return _nameof; }()

// Obtains full name of variable, function, macro.
#define NAMEOF_FULL(...) []() constexpr noexcept {                           \
  ::std::void_t<decltype(__VA_ARGS__)>();                                    \
  constexpr auto _name = ::nameof::detail::pretty_name(#__VA_ARGS__, false); \
  static_assert(_name.size() > 0, "Expression does not have a name.");       \
  constexpr auto _size = _name.size();                                       \
  constexpr auto _nameof_full = ::nameof::cstring<_size>{_name};             \
  return _nameof_full; }()

// Obtains raw name of variable, function, macro.
#define NAMEOF_RAW(...) []() constexpr noexcept {                      \
  ::std::void_t<decltype(__VA_ARGS__)>();                              \
  constexpr auto _name = ::nameof::string_view{#__VA_ARGS__};          \
  static_assert(_name.size() > 0, "Expression does not have a name."); \
  constexpr auto _size = _name.size();                                 \
  constexpr auto _nameof_raw = ::nameof::cstring<_size>{_name};        \
  return _nameof_raw; }()

// Obtains name of enum variable.
#define NAMEOF_ENUM(...) ::nameof::nameof_enum<::std::decay_t<decltype(__VA_ARGS__)>>(__VA_ARGS__)

// Obtains name of static storage enum variable.
// This version is much lighter on the compile times and is not restricted to the enum_range limitation.
#define NAMEOF_ENUM_CONST(...) ::nameof::nameof_enum<__VA_ARGS__>()

// Obtains name of enum-flags variable.
#define NAMEOF_ENUM_FLAG(...) ::nameof::nameof_enum_flag<::std::decay_t<decltype(__VA_ARGS__)>>(__VA_ARGS__)

// Obtains type name, reference and cv-qualifiers are ignored.
#define NAMEOF_TYPE(...) ::nameof::nameof_type<__VA_ARGS__>()

// Obtains type name of expression, reference and cv-qualifiers are ignored.
#define NAMEOF_TYPE_EXPR(...) ::nameof::nameof_type<decltype(__VA_ARGS__)>()

// Obtains full type name, with reference and cv-qualifiers.
#define NAMEOF_FULL_TYPE(...) ::nameof::nameof_full_type<__VA_ARGS__>()

// Obtains full type name of expression, with reference and cv-qualifiers.
#define NAMEOF_FULL_TYPE_EXPR(...) ::nameof::nameof_full_type<decltype(__VA_ARGS__)>()

// Obtains short type name.
#define NAMEOF_SHORT_TYPE(...) ::nameof::nameof_short_type<__VA_ARGS__>()

// Obtains short type name of expression.
#define NAMEOF_SHORT_TYPE_EXPR(...) ::nameof::nameof_short_type<decltype(__VA_ARGS__)>()

// Obtains type name, with reference and cv-qualifiers, using RTTI.
#define NAMEOF_TYPE_RTTI(...) ::nameof::detail::nameof_type_rtti<::std::void_t<decltype(__VA_ARGS__)>>(typeid(__VA_ARGS__).name())

// Obtains full type name, using RTTI.
#define NAMEOF_FULL_TYPE_RTTI(...) ::nameof::detail::nameof_full_type_rtti<decltype(__VA_ARGS__)>(typeid(__VA_ARGS__).name())

// Obtains short type name, using RTTI.
#define NAMEOF_SHORT_TYPE_RTTI(...) ::nameof::detail::nameof_short_type_rtti<decltype(__VA_ARGS__)>(typeid(__VA_ARGS__).name())

#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(__GNUC__)
#  pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

#endif // NEARGYE_NAMEOF_HPP
