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
#include <ostream>

#if !(__cplusplus >= 201402L || (defined(_MSVC_LANG) && _MSVC_LANG >= 201402L))
#error "Request C++14."
#endif

namespace nameof {

namespace detail {

template <typename T>
struct identity {
  using type = T;
};

template <typename T>
struct remove_all_pointers
    : std::conditional<std::is_pointer<T>::value,
                             remove_all_pointers<typename std::remove_pointer<T>::type>,
                             identity<T>>::type {};

// STD like compile-time string.
class cstring final {
  const char* str_;
  std::size_t size_;

 public:
  constexpr cstring(const char* str, std::size_t size) noexcept : str_(str), size_(size) {}

  template <std::size_t N>
  constexpr cstring(const char(&str)[N]) noexcept : str_(str), size_(N - 1) {}

  cstring() = delete;

  cstring(const cstring&) = default;

  cstring(cstring&&) = default;

  cstring& operator=(const cstring&) = default;

  cstring& operator=(cstring&&) = default;

  ~cstring() = default;

  inline constexpr std::size_t size() const noexcept { return size_; }

  inline constexpr std::size_t length() const noexcept { return size_; }

  inline constexpr std::size_t max_size() const noexcept { return std::numeric_limits<decltype(size_)>::max(); }

  inline constexpr bool empty() const noexcept { return size_ == 0; }

  inline constexpr const char* begin() const noexcept { return str_; }

  inline constexpr const char* end() const noexcept { return str_ + size_; }

  inline constexpr const char* cbegin() const noexcept { return begin(); }

  inline constexpr const char* cend() const noexcept { return end(); }

  inline constexpr const char& operator[](std::size_t i) const { return str_[i]; }

  inline constexpr const char& front() const { return str_[0]; }

  inline constexpr const char& back() const { return str_[size_ - 1]; }

  inline constexpr const char* data() const noexcept { return str_; }

  inline constexpr cstring remove_prefix(std::size_t n) const {
    return {str_ + n, size_ - n};
  }

  inline constexpr cstring remove_suffix(std::size_t n) const {
    return {str_, size_ - n};
  }

  inline constexpr cstring substr(std::size_t pos, std::size_t n) const {
    return {str_ + pos, n};
  }

  inline friend constexpr bool operator==(const cstring& lhs, const cstring& rhs) noexcept {
    if (lhs.size_ != rhs.size_) {
      return false;
    }

    for (std::size_t i = 0; i < lhs.size_; ++i) {
      if (lhs.str_[i] != rhs.str_[i]) {
        return false;
      }
    }

    return true;
  }

  inline friend constexpr bool operator!=(const cstring& lhs, const cstring& rhs) noexcept {
    return !(lhs == rhs);
  }

  template <std::size_t N>
  inline friend constexpr bool operator==(const cstring& lhs, const char(&str)[N]) noexcept {
    return lhs == cstring{str, N - 1};
  }

  template <std::size_t N>
  inline friend constexpr bool operator!=(const cstring& lhs, const char(&str)[N]) noexcept {
    return !(lhs == cstring{str, N - 1});
  }

  inline friend std::ostream& operator<<(std::ostream& os, const cstring& str) {
    os.write(str.begin(), str.size());
    return os;
  }

  inline operator std::string() const { return std::string(begin(), size()); }
};

inline constexpr bool IsLexeme(char s) noexcept {
  return !((s >= '0' && s <= '9') || (s >= 'a' && s <= 'z') ||
           (s >= 'A' && s <= 'Z') || s == '_');
}

inline constexpr cstring NameofBase(const char* name, std::size_t length) noexcept {
  for (std::size_t i = length; i > 0; --i) {
    if (IsLexeme(name[i - 1])) {
      return {&name[i], length - i};
    }
  }

  return {name, length};
}

inline constexpr cstring NameofTemplate(const char* name, std::size_t length, bool with_suffix) noexcept {
  std::size_t p = 0;
  for (std::size_t i = length, h = 0; i > 0; --i) {
    if (h == 0 && (name[i - 1] == '&' || name[i - 1] == '*')) {
      ++p;
      continue;
    }

    if (name[i - 1] == '>') {
      ++h;
      ++p;
      continue;
    }

    if (name[i - 1] == '<') {
      --h;
      ++p;
      continue;
    }

    if (h != 0) {
      ++p;
      continue;
    }

    if (IsLexeme(name[i - 1]) && h == 0) {
      return {&name[i], length - i - (with_suffix ? 0 : p)};
    }
  }

  return NameofBase(name, length - (with_suffix ? 0 : p));
}

inline constexpr cstring NameofRaw(const char* name, std::size_t length) noexcept {
  return {name, length};
}

} // namespace detail

template <typename T,
          typename = typename std::enable_if<!std::is_reference<T>::value &&
                                             !std::is_void<T>::value>::type>
inline constexpr detail::cstring Nameof(const T&, const char* name, std::size_t length) noexcept {
  // TODO: conditional expression is constant
  return (std::is_function<T>::value || std::is_member_function_pointer<T>::value)
             ? detail::NameofTemplate(name, length, false)
             : detail::NameofBase(name, length);
}

template <typename T,
          typename = typename std::enable_if<!std::is_enum<T>::value &&
                                             !std::is_function<T>::value &&
                                             !std::is_member_function_pointer<T>::value>::type>
inline constexpr detail::cstring Nameof(T&&, const char*, std::size_t) = delete;

template <typename T>
inline constexpr detail::cstring NameofType(bool full = false) noexcept {
#if defined(__clang__)
  const auto function_name = __PRETTY_FUNCTION__;
  const auto total_length = sizeof(__PRETTY_FUNCTION__) - 1;
  constexpr auto prefix_length = sizeof("nameof::detail::cstring nameof::NameofType(bool) [T = ") - 1;
  constexpr auto suffix_length = sizeof("]") - 1;
#elif defined(__GNUC__)
  const auto function_name = __PRETTY_FUNCTION__;
  const auto total_length = sizeof(__PRETTY_FUNCTION__) - 1;
  constexpr auto prefix_length = sizeof("constexpr nameof::detail::cstring nameof::NameofType(bool) [T = ") - 1;
  constexpr auto suffix_length = sizeof("]") - 1;
#elif defined(_MSC_VER)
  const auto function_name = __FUNCSIG__;
  const auto total_length = sizeof(__FUNCSIG__) - 1;
  constexpr auto prefix_length = sizeof("class nameof::detail::cstring __cdecl nameof::NameofType<") - 1;
  constexpr auto suffix_length = sizeof(">(bool) noexcept") - 1;
#endif

  const auto raw_type_name = detail::cstring{function_name + prefix_length, total_length - prefix_length - suffix_length};

  const auto type_name =  full ? raw_type_name : detail::NameofTemplate(raw_type_name.begin(), raw_type_name.length(), false);

#if defined(_MSC_VER)

  constexpr auto class_length = sizeof("class") - 1;
  constexpr auto struct_length = sizeof("struct") - 1;
  constexpr auto enum_length = sizeof("enum") - 1;

  using D = typename detail::remove_all_pointers<typename std::remove_reference<T>::type>::type;

  if (!full && (std::is_class<D>::value || std::is_enum<D>::value) && (std::is_reference<T>::value || std::is_pointer<T>::value)) {
    if (std::is_class<D>::value && type_name[0] == 'c' && type_name[1] == 'l' && type_name[2] == 'a' && type_name[3] == 's' && type_name[4] == 's') {
      return type_name.remove_prefix(class_length);
    } else if (std::is_class<D>::value && type_name[0] == 's' && type_name[1] == 't' && type_name[2] == 'r' && type_name[3] == 'u' && type_name[4] == 'c' && type_name[5] == 't') {
      return type_name.remove_prefix(struct_length);
    } else if(std::is_enum<D>::value && type_name[0] == 'e' && type_name[1] == 'n' && type_name[2] == 'u' && type_name[3] == 'm') {
      return type_name.remove_prefix(enum_length);
    }
  }

#endif

  return type_name;
}

} // namespace nameof

#if defined(__GNUC__) || defined(__clang__)
// Used to obtain the raw string name of a variable, type, member, function, macros.
#  define NAMEOF_RAW(name) ::nameof::detail::NameofRaw(#name, ((sizeof(#name) / sizeof(char)) - 1) + (0 * sizeof(void (*)(__typeof__(name)))))
#elif defined(_MSC_VER)
// Used to obtain the raw string name of a variable, type, member, function, macros.
#  define NAMEOF_RAW(name) ::nameof::detail::NameofRaw(#name, ((sizeof(#name) / sizeof(char)) - 1) + (0 * sizeof(typeid(name))))
#endif

// Used to obtain the simple (unqualified) string name of a variable, member, function.
#define NAMEOF(name) ::nameof::Nameof<decltype(name)>(name, #name, (sizeof(#name) / sizeof(char)) - 1)

// Used to obtain the simple (unqualified) string name of a type.
#define NAMEOF_TYPE(name) ::nameof::NameofType<decltype(name)>(false)

// Used to obtain the raw string name of a type.
#define NAMEOF_TYPE_RAW(name) ::nameof::NameofType<decltype(name)>(true)
