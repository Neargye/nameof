// nameof() c++11 https://github.com/Neargye/nameof
// Vesion 0.1.5
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

inline constexpr bool IsLexeme(const char s) {
  return (s == '.' || s == '>' || s == ':' || s == '&' || s == '*' ||
          s == '+' || s == '~' || s == '-' || s == '!');
}
template <typename T, std::size_t N>
inline constexpr const char* Nameof(const char(&name)[N], const std::size_t length = N) {
  return length == 0 ? name : IsLexeme(name[length - 1])
                                  ? &name[length]
                                  : Nameof<T>(name, length - 1);
}

}

#define NAMEOF_RAW_(x) #x
#define NAMEOF_RAW(x) NAMEOF_RAW_(x)

// Used to obtain the string name of a variable, function and etc.
#define NAMEOF(name) nameof::Nameof<decltype(name)>(NAMEOF_RAW(name))

#define NAMEOF_FULL(name) nameof::Nameof<decltype(name)>(NAMEOF_RAW(name), 0)

// Used to obtain the string name of a type.
#define NAMEOF_TYPE(type) nameof::Nameof<type>(NAMEOF_RAW(type))

#define NAMEOF_TYPE_FULL(type) nameof::Nameof<type>(NAMEOF_RAW(type), 0)
