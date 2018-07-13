//  _   _                             __    _____
// | \ | |                           / _|  / ____|_     _
// |  \| | __ _ _ __ ___   ___  ___ | |_  | |   _| |_ _| |_
// | . ` |/ _` | '_ ` _ \ / _ \/ _ \|  _| | |  |_   _|_   _|
// | |\  | (_| | | | | | |  __/ (_) | |   | |____|_|   |_|
// |_| \_|\__,_|_| |_| |_|\___|\___/|_|    \_____|
// https://github.com/Neargye/nameof
// vesion 0.4.0
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

namespace nameof {

namespace detail {

inline constexpr bool IsLexeme(char s) noexcept {
  return (s == '.' || s == '>' || s == ':' || s == '&' || s == '*' ||
          s == '+' || s == '~' || s == '-' || s == '!');
}

} // namespace detail

inline constexpr const char* Nameof(const char* name, std::size_t length) noexcept {
  return ((length == 0) ? name : (detail::IsLexeme(name[length - 1])
                                      ? &name[length]
                                      : Nameof(name, length - 1)));
}

template <typename T,
          typename = typename std::enable_if<
              !std::is_reference<T>::value &&
              !std::is_void<T>::value &&
              !std::is_function<T>::value &&
              !std::is_member_function_pointer<T>::value
              >::type>
inline constexpr const char* NameofVariable(const T&, const char* name, std::size_t length) noexcept {
  return Nameof(name, length);
}

template <typename T>
inline constexpr const char* NameofVariable(T&&, const char*, std::size_t) = delete;

} // namespace nameof

#if defined(__GNUC__) || defined(__clang__)
// Used to obtain the simple (unqualified) string name of a variable, type, member, function, macros.
#  define NAMEOF(name) ::nameof::Nameof(#name, (((sizeof(#name) / sizeof(char)) - 1) + (0 * sizeof(void(*)(__typeof__(name))))))
// Used to obtain the full string name of a variable, type, member, function, macros.
#  define NAMEOF_FULL(name) ::nameof::Nameof(#name, (0 * sizeof(void(*)(__typeof__(name)))))
#elif defined(_MSC_VER)
// Used to obtain the simple (unqualified) string name of a variable, type, member, function, macros.
#  define NAMEOF(name) ::nameof::Nameof(#name, (((sizeof(#name) / sizeof(char)) - 1) + (0 * sizeof(typeid(name)))))
// Used to obtain the full string name of a variable, type, member, function, macros.
#  define NAMEOF_FULL(name) ::nameof::Nameof(#name, (0 * sizeof(typeid(name))))
#endif

// Used to obtain the simple (unqualified) string name of a variable or member.
#define NAMEOF_VARIABLE(name) ::nameof::NameofVariable<decltype(name)>(name, #name, (sizeof(#name) / sizeof(char)) - 1)
// Used to obtain the full string name of a variable or member.
#define NAMEOF_VARIABLE_FULL(name) ::nameof::NameofVariable<decltype(name)>(name, #name, 0)
