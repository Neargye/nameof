//  _   _                             __    _____
// | \ | |                           / _|  / ____|_     _
// |  \| | __ _ _ __ ___   ___  ___ | |_  | |   _| |_ _| |_
// | . ` |/ _` | '_ ` _ \ / _ \/ _ \|  _| | |  |_   _|_   _|
// | |\  | (_| | | | | | |  __/ (_) | |   | |____|_|   |_|
// |_| \_|\__,_|_| |_| |_|\___|\___/|_|    \_____|
// https://github.com/Neargye/nameof
// vesion 0.5.0
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// Copyright (c) 2016, 2018 Daniil Goncharov <neargye@gmail.com>.
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
#include <stdexcept>
#include <string>
#include <ostream>

#if defined(__cpp_constexpr) && __cpp_constexpr >= 201304L
#  define NAMEOF_HAS_CONSTEXPR14 1
#endif

#if (defined(__clang__) || defined(_MSC_VER)) || (defined(__GNUC__) && __GNUC__ >= 5)
#  define NAMEOF_HAS_CONSTEXPR 1
#  define NAMEOF_CONSTEXPR constexpr
#else
#  define NAMEOF_CONSTEXPR inline
#endif

#if !defined(NAMEOF_ENUM_MAX_SEARCH_DEPTH)
#  define NAMEOF_ENUM_MAX_SEARCH_DEPTH 64
#endif

namespace nameof {

namespace detail {

namespace nstd {

template <typename T>
struct identity {
  using type = T;
};

} // namespace nstd

#if !defined(NAMEOF_HAS_CONSTEXPR14)
constexpr int CharCompare(char lhs, char rhs) {
  return (lhs > rhs) ? 1 : ((lhs < rhs) ? -1 : 0);
}
#endif

constexpr int StrCompare(const char* lhs, const char* rhs, std::size_t size) {
#if !defined(_MSC_VER) && __cplusplus >= 201703L
  return std::char_traits<char>::compare(lhs, rhs, size);
#elif defined(NAMEOF_HAS_CONSTEXPR14)
  for (std::size_t i = 0; i < size; ++i) {
    if (lhs[i] > rhs[i]) {
      return 1;
    }
    if (lhs[i] < rhs[i]) {
      return -1;
    }
  }
  return 0;
#else
  return (size == 0) ? CharCompare(lhs[0], rhs[0])
                     : (CharCompare(lhs[size - 1], rhs[size - 1]) == 0)
                           ? StrCompare(lhs, rhs, size - 1)
                           : CharCompare(lhs[size - 1], rhs[size - 1]);
#endif
}

constexpr std::size_t StrLen(const char* str, std::size_t size = 0) {
#if !defined(_MSC_VER) && __cplusplus >= 201703L
  static_cast<void>(size);
  return std::char_traits<char>::length(str);
#elif defined(NAMEOF_HAS_CONSTEXPR14)
  for (; str != nullptr; ++size) {
    if (str[size] == '\0') {
      return size;
    }
  }
  return size;
#else
  return (str[size] == '\0') ? size : StrLen(str, size + 1);
#endif
}

} // namespace detail

// std::string like compile-time const char* string.
class cstring final {
  const char* str_;
  std::size_t size_;

 public:
  constexpr cstring(const char* str, std::size_t size, std::size_t prefix = 0, std::size_t suffix = 0) noexcept
      : str_{str + prefix},
        size_{size - prefix - suffix} {}

  constexpr cstring(const char* str) noexcept : cstring{str, detail::StrLen(str), 0, 0} {}

  constexpr cstring() noexcept : cstring{nullptr, 0, 0, 0} {}

  cstring(const std::string& str) noexcept : cstring{str.data(), str.size(), 0, 0} {}

  constexpr cstring(const cstring&) = default;

  cstring& operator=(const cstring&) = default;

  constexpr std::size_t size() const noexcept { return size_; }

  constexpr std::size_t length() const noexcept { return size_; }

  constexpr std::size_t max_size() const noexcept {
    return (std::numeric_limits<std::size_t>::max)();
  }

  constexpr bool empty() const noexcept { return size_ == 0; }

  constexpr const char* begin() const noexcept { return str_; }

  constexpr const char* end() const noexcept { return str_ + size_; }

  constexpr const char* cbegin() const noexcept { return begin(); }

  constexpr const char* cend() const noexcept { return end(); }

  constexpr const char& operator[](std::size_t i) const { return str_[i]; }

  constexpr const char& at(std::size_t i) const {
    return (i < size_) ? str_[i]
                       : (throw std::out_of_range{"cstring::at"}, str_[0]);
  }

  constexpr const char& front() const { return str_[0]; }

  constexpr const char& back() const { return str_[size_ - 1]; }

  constexpr const char* data() const noexcept { return str_; }

  constexpr cstring remove_prefix(std::size_t n) const {
    return {str_ + n, size_ - n};
  }

  constexpr cstring add_prefix(std::size_t n) const {
    return {str_ - n, size_ + n};
  }

  constexpr cstring remove_suffix(std::size_t n) const {
    return {str_, size_ - n};
  }

  constexpr cstring add_suffix(std::size_t n) const {
    return {str_, size_ + n};
  }

  constexpr cstring substr(std::size_t pos, std::size_t n) const {
    return {str_ + pos, n};
  }

  constexpr int compare(cstring other) const {
    return (size_ == other.size_) ? detail::StrCompare(str_, other.str_, size_)
                                  : ((size_ > other.size_) ? 1 : -1);
  }

  friend constexpr bool operator==(cstring lhs, cstring rhs) {
    return lhs.compare(rhs) == 0;
  }

  friend constexpr bool operator!=(cstring lhs, cstring rhs) {
    return !(lhs == rhs);
  }

  std::string append(cstring s) const {
    return std::string{str_, size_}.append(s.str_, s.size_);
  }

  friend std::string operator+(cstring lhs, cstring rhs) {
    return std::string{lhs.str_, lhs.size_} + std::string{rhs.str_, rhs.size_};
  }

  friend std::ostream& operator<<(std::ostream& os, cstring str) {
    os.write(str.str_, str.size_);
    return os;
  }

  operator std::string() const { return std::string{str_, size_}; }
};

namespace detail {

constexpr bool IsLexeme(char s) noexcept {
  return !((s >= '0' && s <= '9') || (s >= 'a' && s <= 'z') || (s >= 'A' && s <= 'Z') || s == '_');
}

constexpr bool IsBracket(char s) noexcept {
  return s == ')' || s == '}' || s == '>' || s == '(' || s == '{' || s == '<';
}

#if defined(NAMEOF_HAS_CONSTEXPR14)
constexpr cstring NameofPretty(cstring name, bool with_suffix) {
  for (std::size_t i = name.size(), h = 0, s = 0; i > 0; --i) {
    if (h == 0 && IsLexeme(name[i - 1]) && !IsBracket(name[i - 1])) {
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
      name = name.remove_suffix(s);
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
    if (IsLexeme(name[i - 1])) {
      return name.remove_prefix(i).remove_suffix(with_suffix ? 0 : s);
    }
  }

  return name.remove_suffix(with_suffix ? 0 : s);
}
#else
constexpr cstring RemoveSuffix(cstring name, std::size_t h = 0) {
  return (h == 0 && IsLexeme(name.back()) && !IsBracket(name.back()))
             ? RemoveSuffix(name.remove_suffix(1), h)
             : (name.back() == ')' || name.back() == '}')
                   ? RemoveSuffix(name.remove_suffix(1), h + 1)
                   : (name.back() == '(' || name.back() == '{')
                         ? RemoveSuffix(name.remove_suffix(1), h - 1)
                         : (h == 0) ? name
                                    : RemoveSuffix(name.remove_suffix(1), h);
}

constexpr std::size_t FindSuffix(cstring name, std::size_t h = 0, std::size_t s = 0) {
  return (name[name.size() - 1 - s] == '>')
             ? FindSuffix(name, h + 1, s + 1)
             : (name[name.size() - 1 - s] == '<')
                   ? FindSuffix(name, h - 1, s + 1)
                   : (h == 0) ? s : FindSuffix(name, h, s + 1);
}

constexpr cstring RemovePrefix(cstring name, const std::size_t p = 0) {
  return p == name.size() ? name : IsLexeme(name[name.size() - 1 - p])
                                       ? name.remove_prefix(name.size() - p)
                                       : RemovePrefix(name, p + 1);
}

constexpr cstring NameofPrettyImpl1(cstring name, std::size_t s, bool with_suffix) {
  return RemovePrefix(name.remove_suffix(s)).add_suffix(with_suffix ? s : 0);
}

constexpr cstring NameofPrettyImpl(cstring name, bool with_suffix) {
  return NameofPrettyImpl1(name, FindSuffix(name), with_suffix);
}

constexpr cstring NameofPretty(cstring name, bool with_suffix) {
  return NameofPrettyImpl(RemoveSuffix(name), with_suffix);
}
#endif

#if defined(_MSC_VER)
constexpr cstring RemoveSpaceSuffix(cstring name) {
  return (name.back() == ' ') ? RemoveSpaceSuffix(name.remove_suffix(1)) : name;
}

constexpr cstring RemoveClassPrefix(cstring name) {
  return (name.size() > sizeof("class") && name[0] == 'c' && name[1] == 'l' &&
          name[2] == 'a' && name[3] == 's' && name[4] == 's' && name[5] == ' ')
             ? name.remove_prefix(sizeof("class"))
             : name;
}

constexpr cstring RemoveEnumPrefix(cstring name) {
  return (name.size() > sizeof("enum") && name[0] == 'e' && name[1] == 'n' &&
          name[2] == 'u' && name[3] == 'm' && name[4] == ' ')
             ? name.remove_prefix(sizeof("enum"))
             : name;
}

constexpr cstring RemoveStructPrefix(cstring name) {
  return (name.size() > sizeof("struct") && name[0] == 's' && name[1] == 't' &&
          name[2] == 'r' && name[3] == 'u' && name[4] == 'c' &&
          name[5] == 't' && name[6] == ' ')
             ? name.remove_prefix(sizeof("struct"))
             : name;
}

constexpr cstring NameofTypePretty(cstring name) {
  return RemoveClassPrefix(RemoveStructPrefix(RemoveEnumPrefix(RemoveSpaceSuffix(name))));
}
#elif defined(__clang__) || defined(__GNUC__)
constexpr cstring NameofTypePretty(const char* str, std::size_t size, std::size_t prefix, std::size_t suffix) {
  return {str, size, prefix, suffix + ((str[size - suffix - 1] == ' ') ? 1 : 0)};
}
#endif

template <typename T>
NAMEOF_CONSTEXPR int NameofEnumImpl1() {
#if defined(__clang__)
  return sizeof(__PRETTY_FUNCTION__) - sizeof("int nameof::detail::NameofEnumImpl1() [T = ") - sizeof("]") + 1;
#elif defined(__GNUC__)
  return sizeof(__PRETTY_FUNCTION__) - sizeof("constexpr int nameof::detail::NameofEnumImpl1() [with T = ") - sizeof("]") + 1;
#elif defined(_MSC_VER)
  return sizeof(__FUNCSIG__) - sizeof("int __cdecl nameof::detail::NameofEnumImpl1<") - sizeof(">(void)") + 1;
#else
  return 0;
#endif
}

template <typename T, T V>
NAMEOF_CONSTEXPR cstring NameofEnumImpl2() {
#if defined(__clang__)
  return {__PRETTY_FUNCTION__,
          sizeof(__PRETTY_FUNCTION__) - 1,
          sizeof("nameof::cstring nameof::detail::NameofEnumImpl2() [T = ") + NameofEnumImpl1<T>() + sizeof("; V = ") - 2,
          sizeof("]") - 1};
#elif defined(__GNUC__)
  return {__PRETTY_FUNCTION__,
          sizeof(__PRETTY_FUNCTION__) - 1,
          sizeof("constexpr nameof::cstring nameof::detail::NameofEnumImpl2() [with T = ") + NameofEnumImpl1<T>() + sizeof("; T V = ") - 2,
          sizeof("]") - 1};
#elif defined(_MSC_VER)
  return {__FUNCSIG__,
          sizeof(__FUNCSIG__) - 1,
          sizeof("class nameof::cstring __cdecl nameof::detail::NameofEnumImpl2<") + NameofEnumImpl1<T>(),
          sizeof(">(void)") - 1};
#else
  return {};
#endif
}

template <typename T, int I>
struct NameofEnumImpl {
  NAMEOF_CONSTEXPR cstring operator()(int value) const {
    return (value - I == 0)
               ? NameofEnumImpl2<T, T(I)>()
               : (value - I == 1)
                     ? NameofEnumImpl2<T, T(I + 1)>()
                     : (value - I == 2)
                           ? NameofEnumImpl2<T, T(I + 2)>()
                           : (value - I == 3)
                                 ? NameofEnumImpl2<T, T(I + 3)>()
                                 : (value - I == 4)
                                       ? NameofEnumImpl2<T, T(I + 4)>()
                                       : (value - I == 5)
                                             ? NameofEnumImpl2<T, T(I + 5)>()
                                             : (value - I == 6)
                                                   ? NameofEnumImpl2<T, T(I + 6)>()
                                                   : (value - I == 7)
                                                         ? NameofEnumImpl2<T, T(I + 7)>()
                                                         : NameofEnumImpl<T, I + 8>{}(value);
  }
};

template <typename T>
struct NameofEnumImpl<T, NAMEOF_ENUM_MAX_SEARCH_DEPTH> {
  NAMEOF_CONSTEXPR cstring operator()(int) const { return {}; }
};

template <typename T>
NAMEOF_CONSTEXPR cstring NameofType() {
#if defined(__clang__)
  return NameofTypePretty(
      __PRETTY_FUNCTION__,
      sizeof(__PRETTY_FUNCTION__) - 1,
      sizeof("nameof::cstring nameof::detail::NameofType() [T = nameof::detail::nstd::identity<") - 1,
      sizeof(">]") - 1);
#elif defined(__GNUC__)
  return NameofTypePretty(
      __PRETTY_FUNCTION__,
      sizeof(__PRETTY_FUNCTION__) - 1,
#  if defined(NAMEOF_HAS_CONSTEXPR)
      sizeof("constexpr nameof::cstring nameof::detail::NameofType() [with T = nameof::detail::nstd::identity<") - 1,
#  else
      sizeof("nameof::cstring nameof::detail::NameofType() [with T = nameof::detail::nstd::identity<") - 1,
#  endif
      sizeof(">]") - 1);
#elif defined(_MSC_VER)
  return NameofTypePretty(
      {__FUNCSIG__,
      sizeof(__FUNCSIG__) - 1,
      sizeof("class nameof::cstring __cdecl nameof::detail::NameofType<struct nameof::detail::nstd::identity<") - 1,
      sizeof(">>(void)") - 1});
#else
  return {};
#endif
}

} // namespace detail

template <typename T,
          typename = typename std::enable_if<!std::is_reference<T>::value>::type>
constexpr cstring Nameof(const char* name, std::size_t size, bool with_suffix = false) {
  return detail::NameofPretty({name, size}, with_suffix);
}

template <typename T,
          typename = typename std::enable_if<!std::is_reference<T>::value &&
                                             std::is_enum<T>::value>::type>
NAMEOF_CONSTEXPR cstring NameofEnum(T value) {
#if defined(__clang__) || defined(_MSC_VER)
  return detail::NameofPretty(
      std::is_unsigned<typename std::underlying_type<T>::type>::value
          ? detail::NameofEnumImpl<T, 0>{}(static_cast<int>(value))
          : detail::NameofEnumImpl<T, -NAMEOF_ENUM_MAX_SEARCH_DEPTH>{}(static_cast<int>(value)),
      false);
#elif defined(__GNUC__)
  return std::is_unsigned<typename std::underlying_type<T>::type>::value
             ? detail::NameofEnumImpl<T, 0>{}(static_cast<int>(value))
             : detail::NameofEnumImpl<T, -NAMEOF_ENUM_MAX_SEARCH_DEPTH>{}(static_cast<int>(value));
#else
  return {};
#endif
}

template <typename T>
NAMEOF_CONSTEXPR cstring NameofType() {
  return true ? detail::NameofType<detail::nstd::identity<T>>()
              : detail::NameofType<detail::nstd::identity<T>>();
}

template <typename T>
constexpr cstring NameofRaw(const char* name, std::size_t size) {
  return {name, size};
}

} // namespace nameof

// Used to obtain the simple (unqualified) string name of a variable, member, function, enum, macros.
#define NAMEOF(...) ::nameof::Nameof<decltype(__VA_ARGS__)>(#__VA_ARGS__, (sizeof(#__VA_ARGS__) / sizeof(char)) - 1, false)
// Used to obtain the full string name of a variable, member, function, enum, macros.
#define NAMEOF_FULL(...) ::nameof::Nameof<decltype(__VA_ARGS__)>(#__VA_ARGS__, (sizeof(#__VA_ARGS__) / sizeof(char)) - 1, true)
// Used to obtain the raw string name of a variable, member, function, enum, macros.
#define NAMEOF_RAW(...) ::nameof::NameofRaw<decltype(__VA_ARGS__)>(#__VA_ARGS__, (sizeof(#__VA_ARGS__) / sizeof(char)) - 1)

// Used to obtain the simple (unqualified) string name of a enum variable.
#define NAMEOF_ENUM(...) ::nameof::NameofEnum<decltype(__VA_ARGS__)>(__VA_ARGS__)

// Used to obtain the string name of a type.
#define NAMEOF_TYPE(...) ::nameof::NameofType<decltype(__VA_ARGS__)>()
#define NAMEOF_TYPE_T(...) ::nameof::NameofType<__VA_ARGS__>()
