//  _   _                             __    _____
// | \ | |                           / _|  / ____|_     _
// |  \| | __ _ _ __ ___   ___  ___ | |_  | |   _| |_ _| |_
// | . ` |/ _` | '_ ` _ \ / _ \/ _ \|  _| | |  |_   _|_   _|
// | |\  | (_| | | | | | |  __/ (_) | |   | |____|_|   |_|
// |_| \_|\__,_|_| |_| |_|\___|\___/|_|    \_____|
// https://github.com/Neargye/nameof
// vesion 0.2.3
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

namespace nameof {

namespace detail {

inline constexpr bool IsLexeme(const char s) noexcept {
  return (s == '.' || s == '>' || s == ':' || s == '&' || s == '*' ||
          s == '+' || s == '~' || s == '-' || s == '!');
}

} // namespace detail

inline constexpr const char* Nameof(const char* name, const ::std::size_t length, const ::std::size_t) noexcept {
  return length == 0 ? name : detail::IsLexeme(name[length - 1])
                                  ? &name[length]
                                  : Nameof(name, length - 1, 0);
}

} // namespace nameof

#if defined(__GNUC__) || defined(__clang__)
// Used to obtain the string name of a variable, type, function, macros and etc.
#  define NAMEOF(name) ::nameof::Nameof(#name, sizeof(#name) / sizeof(char) - 1, sizeof(void(*)(__typeof__(name))))
// Used to obtain the string full name of a variable, type, function, macros and etc.
#  define NAMEOF_FULL(name) ::nameof::Nameof(#name, 0, sizeof(void(*)(__typeof__(name))))
#elif defined(_MSC_VER)
// Used to obtain the string name of a variable, type, function, macros and etc.
#  define NAMEOF(name) ::nameof::Nameof(#name, sizeof(#name) / sizeof(char) - 1, sizeof(typeid(name)))
// Used to obtain the string full name of a variable, type, function, macros and etc.
#  define NAMEOF_FULL(name) ::nameof::Nameof(#name, 0, sizeof(typeid(name)))
#endif
