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

#if (__cplusplus >= 201402L || (defined(_MSVC_LANG) && _MSC_VER >= 1910 && _MSVC_LANG >= 201402L))
#  define NAMEOF_CONSTEXPR14 constexpr
#else
#  define NAMEOF_CONSTEXPR14
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
                             remove_all_pointers<typename std::remove_cv<typename std::remove_pointer<T>::type>::type>,
                             identity<T>>::type {};

template <typename T>
using Decay = std::remove_reference<typename std::remove_cv<typename remove_all_pointers<T>::type>::type>;

inline constexpr bool StrEquals(const char* lhs, const char* rhs, std::size_t size) {
  return (size == 0) ? (lhs[0] == rhs[0]) : ((lhs[size - 1] == rhs[size - 1]) && StrEquals(lhs, rhs, size - 1));
}

inline constexpr std::size_t StrLen(const char* str, std::size_t size = 0) {
  return (str == nullptr) ? 0 : ((str[size] == '\0') ? size : StrLen(str, size + 1));
}

// STD like compile-time const char* string.
class cstring final {
  const char* str_;
  std::size_t size_;

 public:
  constexpr cstring(const char* str, std::size_t length, std::size_t prefix = 0, std::size_t suffix = 0) noexcept
      : str_{str + prefix},
        size_(length - prefix - suffix) {}

  constexpr cstring() noexcept : cstring{nullptr, 0, 0, 0} {}

  constexpr cstring(const char* str) noexcept : cstring{str, StrLen(str), 0, 0} {}

  cstring(const cstring&) = default;

  cstring(cstring&&) = default;

  cstring& operator=(const cstring&) = default;

  cstring& operator=(cstring&&) = default;

  ~cstring() = default;

  inline constexpr std::size_t size() const noexcept { return size_; }

  inline constexpr std::size_t length() const noexcept { return size_; }

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
    return (lhs.size_ == rhs.size_) && StrEquals(lhs.begin(), rhs.begin(), lhs.size());
  }

  inline friend constexpr bool operator!=(const cstring& lhs, const cstring& rhs) noexcept {
    return !(lhs == rhs);
  }

  inline friend std::ostream& operator<<(std::ostream& os, const cstring& str) {
    os.write(str.begin(), str.size());
    return os;
  }

  inline operator std::string() const { return std::string(begin(), size()); }
};

inline constexpr bool IsLexeme(char s) noexcept {
  return !((s >= '0' && s <= '9') || (s >= 'a' && s <= 'z') || (s >= 'A' && s <= 'Z') || s == '_');
}

inline NAMEOF_CONSTEXPR14 cstring NameofBase(const char* name, std::size_t length, bool with_suffix) noexcept {
  std::size_t p = 0;
  if(IsLexeme(name[length - 1])) {
    for (std::size_t i = length, h = 0; i > 0; --i) {
      if (h == 0 && (IsLexeme(name[i - 1]) && name[i - 1] != '>')) {
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

      if (h == 0) {
        break;
      }
    }
  }

  for (std::size_t i = length - p; i > 0; --i) {
      if (IsLexeme(name[i - 1])) {
        return {&name[i], length - i - (with_suffix ? 0 : p)};
      }
    }

  return {name, length - (with_suffix ? 0 : p)};
}

inline constexpr cstring NameofRaw(const char* name, std::size_t length) noexcept {
  return {name, length};
}

} // namespace detail

template <typename T,
          typename = typename std::enable_if<!std::is_reference<T>::value &&
                                             !std::is_void<T>::value>::type>
inline NAMEOF_CONSTEXPR14 detail::cstring Nameof(const T&, const char* name, std::size_t length, bool with_suffix = false) noexcept {
  return detail::NameofBase(name, length, with_suffix);
}

template <typename T,
          typename = typename std::enable_if<!std::is_enum<T>::value &&
                                             !std::is_function<T>::value &&
                                             !std::is_member_function_pointer<T>::value>::type>
inline NAMEOF_CONSTEXPR14 detail::cstring Nameof(T&&, const char*, std::size_t, bool) = delete;

template <typename T>
inline detail::cstring NameofTypeRaw() noexcept {
#if defined(__clang__)
    return {__PRETTY_FUNCTION__,
          sizeof(__PRETTY_FUNCTION__) - 1,
          sizeof("detail::cstring nameof::NameofTypeRaw() [T = ") - 1,
          sizeof("]") - 1};
#elif defined(__GNUC__)
  return {__PRETTY_FUNCTION__,
          sizeof(__PRETTY_FUNCTION__) - 1,
          sizeof("nameof::detail::cstring nameof::NameofTypeRaw() [with T = ") - 1,
          sizeof("]") - 1};
#elif defined(_MSC_VER)
  return {__FUNCSIG__,
          sizeof(__FUNCSIG__) - 1,
          sizeof("class nameof::detail::cstring __cdecl nameof::NameofTypeRaw<") - 1,
          sizeof(">(void) noexcept") - 1};
#else
  return {};
#endif
}

template <typename T, typename D = typename detail::Decay<T>::type>
inline detail::cstring NameofType() noexcept {
  const auto raw_type_name = NameofTypeRaw<D>();
  return detail::NameofBase(raw_type_name.begin(), raw_type_name.length(), false);
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
#define NAMEOF(name) ::nameof::Nameof<decltype(name)>(name, #name, (sizeof(#name) / sizeof(char)) - 1, false)

// Used to obtain the full string name of a variable, member, function.
#define NAMEOF_FULL(name) ::nameof::Nameof<decltype(name)>(name, #name, (sizeof(#name) / sizeof(char)) - 1, true)

// Used to obtain the simple (unqualified) string name of a type.
#define NAMEOF_TYPE(name) ::nameof::NameofType<decltype(name)>()

// Used to obtain the raw string name of a type.
#define NAMEOF_TYPE_RAW(name) ::nameof::NameofTypeRaw<decltype(name)>()
