// MIT License
//
// Copyright(c) 2016 - 2018 Terik23
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once
#include <cstddef>

#define NAMEOF_RAW(x) (#x)

inline constexpr const char* nameof(const char* name, const size_t length) {
  return length == 0 ? name
                     : (name[length - 1] == ' ' || name[length - 1] == '.' ||
                        name[length - 1] == '>' || name[length - 1] == ':' ||
                        name[length - 1] == '&' || name[length - 1] == '*' ||
                        name[length - 1] == '+' || name[length - 1] == '~' ||
                        name[length - 1] == '-' || name[length - 1] == '!')
                           ? &name[length]
                           : nameof(name, length - 1);
}

// Used to obtain the string name of a variable, function and etc.
template <typename T>
inline constexpr const char* template_nameof(const char* name, const size_t length) { return nameof(name, length); }
#define NAMEOF(name) template_nameof<decltype(name)>(NAMEOF_RAW(name), sizeof(NAMEOF_RAW(name)) / sizeof(char) - 1)

#define NAMEOF_VARIABLE(variable) NAMEOF(variable)
#define NAMEOF_VAR(var) NAMEOF_VARIABLE(var)

#define NAMEOF_FUNCTION(function) NAMEOF(function)
#define NAMEOF_FUN(fun) NAMEOF_FUNCTION(fun)

// Used to obtain the string name of a type.
#define NAMEOF_TYPE(type) template_nameof<type>(NAMEOF_RAW(type), sizeof(NAMEOF_RAW(type)) / sizeof(char) - 1)
