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

// Simple Example.

#include <iostream>
#include <nameof.hpp>

struct SomeStruct {
  int SomeField;
  void SomeMethod() { std::cout << "No called!" << std::endl; }
};

int someVar = 0;

int main() {
  SomeStruct someVar{1};

  constexpr auto a = NAMEOF_VAR(someVar.SomeField);
  constexpr auto b = NAMEOF_VAR((&someVar)->SomeField);
  constexpr auto c = NAMEOF_VAR(someVar);
  constexpr auto d = NAMEOF_VAR(::someVar);
  constexpr auto e = NAMEOF_VAR(&SomeStruct::SomeMethod);
  constexpr auto f = NAMEOF_FUN(someVar.SomeMethod());
  constexpr auto g = NAMEOF_TYPE(SomeStruct);

  std::cout << a << std::endl;  // SomeField
  std::cout << b << std::endl;  // SomeField
  std::cout << c << std::endl;  // someVar
  std::cout << d << std::endl;  // someVar
  std::cout << e << std::endl;  // SomeMethod
  std::cout << f << std::endl;  // SomeMethod()
  std::cout << g << std::endl;  // SomeStruct
}
