// nameof() c++11 https://github.com/Neargye/nameof
// Vesion 0.2.0
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

#define NAMEOF_RAW_(x) #x
#define NAMEOF_RAW(x) NAMEOF_RAW_(x)

namespace nameof {
namespace detail {
inline constexpr bool IsLexeme(const char s) {
  return (s == '.' || s == '>' || s == ':' || s == '&' || s == '*' ||
          s == '+' || s == '~' || s == '-' || s == '!');
}
}
}

#if defined(__GXX_RTTI) || defined(_CPPRTTI) || defined(__RTTI) || defined(__INTEL_RTTI__)

#include <typeinfo>

namespace nameof {
inline constexpr const char* Nameof(const char* name, const std::size_t length, const std::size_t) {
  return length == 0 ? name : detail::IsLexeme(name[length - 1])
                                  ? &name[length]
                                  : Nameof(name, length - 1, 0);
}
}

// Used to obtain the string name of a variable, type, function and etc.
#define NAMEOF(name) nameof::Nameof(NAMEOF_RAW(name), sizeof(NAMEOF_RAW(name)) / sizeof(char) - 1, sizeof(typeid(name)))

// Used to obtain the string full name of a variable, type, function and etc.
#define NAMEOF_FULL(name) nameof::Nameof(NAMEOF_RAW(name), 0, sizeof(typeid(name)))

// Alias
#define NAMEOF_TYPE(type) NAMEOF(type)
#define NAMEOF_TYPE_FULL(type) NAMEOF_FULL(type)

#else

namespace nameof {
template <typename T>
inline constexpr const char* Nameof(const char* name, const std::size_t length) {
  return length == 0 ? name : detail::IsLexeme(name[length - 1])
                                  ? &name[length]
                                  : Nameof<T>(name, length - 1);
}
}

// Used to obtain the string name of a variable, function and etc.
#define NAMEOF(name) nameof::Nameof<decltype(name)>(NAMEOF_RAW(name), sizeof(NAMEOF_RAW(name)) / sizeof(char) - 1)

// Used to obtain the string full name of a variable, function and etc.
#define NAMEOF_FULL(name) nameof::Nameof<decltype(name)>(NAMEOF_RAW(name), 0)

// Used to obtain the string name of a type.
#define NAMEOF_TYPE(type) nameof::Nameof<type>(NAMEOF_RAW(type), sizeof(NAMEOF_RAW(type)) / sizeof(char) - 1)

// Used to obtain the string full name of a type.
#define NAMEOF_TYPE_FULL(type) nameof::Nameof<type>(NAMEOF_RAW(type), 0)

#endif
