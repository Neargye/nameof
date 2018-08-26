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
#include <string>
#include <ostream>

#if defined(__cpp_constexpr) && __cpp_constexpr >= 201304L
#  define NAMEOF_HAS_CONSTEXPR14 1
#endif

#if defined(__clang__) || defined(_MSC_VER)
#  define NAMEOF_TYPE_CONSTEXPR constexpr
#  define NAMEOF_TYPE_HAS_CONSTEXPR 1
#else
#  define NAMEOF_TYPE_CONSTEXPR inline
#endif

namespace nameof {

namespace detail {

namespace nstd {

template <typename T>
struct identity {
  using type = T;
};

template <typename T>
using identity_t = typename identity<T>::type;

// Removes all pointer from the given type.
template <typename T>
struct remove_all_p
    : std::conditional<std::is_pointer<T>::value,
          remove_all_p<typename std::remove_pointer<T>::type>,
          identity<T>
              >::type {};

template <typename T>
using remove_all_p_t = typename remove_all_p<T>::type;

// Removes const, volatile, reference specifiers from the given type.
template <typename T>
struct remove_cvr {
  using type = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
};

template <typename T>
using remove_cvr_t = typename remove_cvr<T>::type;

// Removes all const, volatile, reference, pointer specifiers from the given type.
template <typename T>
struct remove_all_cvrp {
  using type = typename remove_cvr<typename remove_all_p<typename remove_cvr<T>::type>::type>::type;
};

template <typename T>
using remove_all_cvrp_t = typename remove_all_cvrp<T>::type;

// Removes all const, volatile, reference, pointer, array extents specifiers from the given type.
template <typename T, typename U = typename remove_all_cvrp<T>::type>
struct remove_all_cvrpe
    : std::conditional<std::is_array<U>::value,
          remove_all_cvrpe<typename std::remove_all_extents<U>::type>,
          identity<U>
              >::type {};

template <typename T>
using remove_all_cvrpe_t = typename remove_all_cvrpe<T>::type;

} // namespace nstd

constexpr bool StrEquals(const char* lhs, const char* rhs, std::size_t size) {
#if defined(NAMEOF_HAS_CONSTEXPR14)
  for (std::size_t i = 0; i < size; ++i) {
    if (lhs[i] != rhs[i]) {
      return false;
    }
  }

  return true;
#else
  return (size == 0) ? (lhs[0] == rhs[0]) : ((lhs[size - 1] == rhs[size - 1]) && StrEquals(lhs, rhs, size - 1));
#endif
}

constexpr std::size_t StrLen(const char* str, std::size_t size = 0) {
#if defined(NAMEOF_HAS_CONSTEXPR14)
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

// STD like compile-time const char* string.
class cstring final {
  const char* str_;
  std::size_t size_;

 public:
  constexpr cstring(const char* str, std::size_t size, std::size_t prefix = 0, std::size_t suffix = 0) noexcept
      : str_{str + prefix},
        size_{size - prefix - suffix} {}

  constexpr cstring() noexcept : cstring{nullptr, 0, 0, 0} {}

  constexpr cstring(const char* str) noexcept : cstring{str, StrLen(str), 0, 0} {}

  cstring(const cstring&) = default;

  cstring(cstring&&) = default;

  cstring& operator=(const cstring&) = default;

  cstring& operator=(cstring&&) = default;

  ~cstring() = default;

  constexpr std::size_t size() const noexcept { return size_; }

  constexpr std::size_t length() const noexcept { return size_; }

  constexpr bool empty() const noexcept { return size_ == 0; }

  constexpr const char* begin() const noexcept { return str_; }

  constexpr const char* end() const noexcept { return str_ + size_; }

  constexpr const char* cbegin() const noexcept { return begin(); }

  constexpr const char* cend() const noexcept { return end(); }

  constexpr const char& operator[](std::size_t i) const { return str_[i]; }

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

  friend constexpr bool operator==(cstring lhs, cstring rhs) noexcept {
    return (lhs.size_ == rhs.size_) && StrEquals(lhs.str_, rhs.str_, lhs.size_);
  }

  friend constexpr bool operator!=(cstring lhs, cstring rhs) noexcept {
    return !(lhs == rhs);
  }

  friend std::ostream& operator<<(std::ostream& os, cstring str) {
    os.write(str.str_, str.size_);
    return os;
  }

  operator std::string() const { return std::string(str_, size_); }
};

constexpr bool IsLexeme(char s) noexcept {
  return !((s >= '0' && s <= '9') || (s >= 'a' && s <= 'z') || (s >= 'A' && s <= 'Z') || s == '_');
}

#if defined(NAMEOF_HAS_CONSTEXPR14)
constexpr cstring NameofPretty(cstring name, bool with_suffix) noexcept {
  std::size_t s = 0;
  for (std::size_t i = name.size(), h = 0; i > 0; --i) {
    if (name[i - 1] == '>') {
      ++h;
      ++s;
      continue;
    }

    if (name[i - 1] == '<') {
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
constexpr std::size_t NameofBaseImpl1(cstring name, std::size_t h = 0, std::size_t s = 0) noexcept {
  return name[name.size() - 1 - s] == '>'
             ? NameofBaseImpl1(name, h + 1, s + 1)
             : name[name.size() - 1 - s] == '<'
                   ? NameofBaseImpl1(name, h - 1, s + 1)
                   : h == 0 ? s : NameofBaseImpl1(name, h, s + 1);
}

constexpr cstring NameofBaseImpl2(cstring name, const std::size_t p = 0) noexcept {
  return p == name.size() ? name : IsLexeme(name[name.size() - 1 - p])
                                       ? name.remove_prefix(name.size() - p)
                                       : NameofBaseImpl2(name, p + 1);
}

constexpr cstring NameofBaseImpl3(cstring name, std::size_t s, bool with_suffix) noexcept {
  return NameofBaseImpl2(name.remove_suffix(s)).add_suffix(with_suffix ? s : 0);
}

constexpr cstring NameofPretty(cstring name, bool with_suffix) noexcept {
  return NameofBaseImpl3(name, NameofBaseImpl1(name), with_suffix);
}
#endif

constexpr cstring RemoveSpace(cstring name) noexcept {
  return name.back() == ' ' ? RemoveSpace(name.remove_suffix(1)) : name;
}

constexpr cstring RemoveClassPrefix(cstring name) noexcept {
  return (name.size() > sizeof("class") && name[0] == 'c' && name[1] == 'l' &&
          name[2] == 'a' && name[3] == 's' && name[4] == 's' && name[5] == ' ')
             ? name.remove_prefix(sizeof("class"))
             : name;
}

constexpr cstring RemoveEnumPrefix(cstring name) noexcept {
  return (name.size() > sizeof("enum") && name[0] == 'e' && name[1] == 'n' &&
          name[2] == 'u' && name[3] == 'm' && name[4] == ' ')
             ? name.remove_prefix(sizeof("enum"))
             : name;
}

constexpr cstring RemoveStructPrefix(cstring name) noexcept {
  return (name.size() > sizeof("struct") && name[0] == 's' && name[1] == 't' &&
          name[2] == 'r' && name[3] == 'u' && name[4] == 'c' && name[5] == 't' && name[6] == ' ')
             ? name.remove_prefix(sizeof("struct"))
             : name;
}

constexpr cstring NameofTypeRawPretty(cstring name) noexcept {
  return RemoveClassPrefix(RemoveStructPrefix(RemoveEnumPrefix(RemoveSpace(name))));
}

template <typename T>
NAMEOF_TYPE_CONSTEXPR cstring NameofTypeRaw() noexcept {
#if defined(__clang__)
  return {__PRETTY_FUNCTION__,
          sizeof(__PRETTY_FUNCTION__) - 1,
          sizeof("nameof::detail::cstring nameof::detail::NameofTypeRaw() [T = nameof::detail::nstd::identity<") - 1,
          sizeof(">]") - 1};
#elif defined(__GNUC__)
  return {__PRETTY_FUNCTION__,
          sizeof(__PRETTY_FUNCTION__) - 1,
          sizeof("nameof::detail::cstring nameof::detail::NameofTypeRaw() [with T = nameof::detail::nstd::identity<") - 1,
          sizeof(">]") - 1};
#elif defined(_MSC_VER)
  return {__FUNCSIG__,
          sizeof(__FUNCSIG__) - 1,
          sizeof("class nameof::detail::cstring __cdecl nameof::detail::NameofTypeRaw<struct nameof::detail::nstd::identity<") - 1,
          sizeof(">>(void) noexcept") - 1};
#else
  return {};
#endif
}

template <typename T,
          typename = typename std::enable_if<!std::is_reference<T>::value &&
                                             !std::is_void<T>::value>::type>
constexpr cstring Nameof(const T&, const char* name, std::size_t size, bool with_suffix) noexcept {
  return NameofPretty({name, size}, with_suffix);
}

template <typename T,
          typename = typename std::enable_if<!std::is_enum<T>::value &&
                                             !std::is_function<T>::value &&
                                             !std::is_member_function_pointer<T>::value>::type>
constexpr cstring Nameof(T&&, const char*, std::size_t, bool) = delete;

constexpr cstring NameofRaw(const char* name, std::size_t size) noexcept {
  return {name, size};
}

} // namespace detail

template <typename T, typename H = detail::nstd::identity<T>>
NAMEOF_TYPE_CONSTEXPR detail::cstring NameofType(bool pretty = true) noexcept {
  return pretty ? detail::NameofTypeRawPretty(detail::NameofTypeRaw<H>()) : detail::NameofTypeRaw<H>();
}

} // namespace nameof

#if defined(__clang__)
#  if __has_feature(cxx_rtti)
#    define NAMEOF_HAS_RTTI 1
#  endif
#elif defined(__GNUC__)
#  if defined(__GXX_RTTI)
#    define NAMEOF_HAS_RTTI 1
#  endif
#elif defined(_MSC_VER) && defined(_CPPRTTI)
#  if defined(_CPPRTTI)
#    define NAMEOF_HAS_RTTI 1
#  endif
#endif

#if defined(NAMEOF_HAS_RTTI) || defined(_MSC_VER)
#include <typeinfo>

// Used to obtain the raw string name of a variable, type, member, function, macros.
#  define NAMEOF_RAW(name) ::nameof::detail::NameofRaw(#name, ((sizeof(#name) / sizeof(char)) - 1) + (0 * sizeof(typeid(name))))
#elif defined(__clang__) || defined(__GNUC__)
// Used to obtain the raw string name of a variable, type, member, function, macros.
#  define NAMEOF_RAW(name) ::nameof::detail::NameofRaw(#name, ((sizeof(#name) / sizeof(char)) - 1) + (0 * sizeof(void(*)(__typeof__(name)))))
#endif

// Used to obtain the simple (unqualified) string name of a variable, member, function.
#define NAMEOF(name) ::nameof::detail::Nameof<decltype(name)>(name, #name, (sizeof(#name) / sizeof(char)) - 1, false)

// Used to obtain the full string name of a variable, member, function.
#define NAMEOF_FULL(name) ::nameof::detail::Nameof<decltype(name)>(name, #name, (sizeof(#name) / sizeof(char)) - 1, true)

// Used to obtain the string name of a type.
#define NAMEOF_TYPE(var) ::nameof::NameofType<decltype(var)>(true)
#define NAMEOF_TYPE_T(type) ::nameof::NameofType<type>(true)
