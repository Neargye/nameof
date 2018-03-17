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
#include <cstdint>
#include <nameof.hpp>

struct SomeStruct {
  int SomeField;
  void SomeMethod() { std::cout << "No called!" << std::endl; }
};

int someVar = 0;

int main() {
  SomeStruct someVar{1};

  constexpr auto a = nameof_variable(someVar.SomeField);
  constexpr auto b = nameof_variable((&someVar)->SomeField);
  constexpr auto c = nameof_variable(someVar);
  constexpr auto d = nameof_variable(::someVar);
  constexpr auto e = nameof_variable(&SomeStruct::SomeMethod);
  constexpr auto f = nameof_function(someVar.SomeMethod());
  constexpr auto g = nameof_type(SomeStruct);

  std::cout << a << std::endl;  // SomeField
  std::cout << b << std::endl;  // SomeField
  std::cout << c << std::endl;  // someVar
  std::cout << d << std::endl;  // someVar
  std::cout << e << std::endl;  // SomeMethod
  std::cout << f << std::endl;  // SomeMethod()
  std::cout << g << std::endl;  // SomeStruct
}