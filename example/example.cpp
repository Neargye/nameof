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
  void SomeMethod1() { std::cout << "No called!" << std::endl; }
  int SomeMethod2() {
    std::cout << "No called!" << std::endl;
    return 1;
  }
};

void SomeMethod3() { std::cout << "No called!" << std::endl; }

struct Long {
  struct LL {
    int LLLField;
  };
  LL LLField;
};

struct TestRValue {
  int Field;
};

int someVar = 0;

int main() {
  SomeStruct someVar{1};
  Long otherVar{2};
  int intValue{3};
  SomeStruct* ptrVar = &someVar;
  SomeStruct** ptrptrVar = &ptrVar;

  constexpr auto constexpr_work_fine = NAMEOF(intValue);
  std::cout << constexpr_work_fine << std::endl; // intValue

  std::cout << NAMEOF(someVar) << std::endl; // someVar
  std::cout << NAMEOF(someVar.SomeField) << std::endl; // SomeField
  std::cout << NAMEOF((&someVar)->SomeField) << std::endl; // SomeField
  std::cout << NAMEOF(::someVar) << std::endl; // someVar
  std::cout << NAMEOF(otherVar.LLField.LLLField) << std::endl; // LLLField
  std::cout << NAMEOF(&someVar) << std::endl; // someVar
  std::cout << NAMEOF(ptrVar) << std::endl; // ptrVar
  std::cout << NAMEOF(*ptrVar) << std::endl; // ptrVar
  std::cout << NAMEOF(ptrptrVar) << std::endl; // ptrptrVar
  std::cout << NAMEOF(*ptrptrVar) << std::endl; // ptrptrVar
  std::cout << NAMEOF(**ptrptrVar) << std::endl; // ptrptrVar
  std::cout << NAMEOF(+intValue) << std::endl; // intValue
  std::cout << NAMEOF(-intValue) << std::endl; // intValue
  std::cout << NAMEOF(~intValue) << std::endl; // intValue
  std::cout << NAMEOF(!intValue) << std::endl; // intValue
  std::cout << NAMEOF(someVar.SomeMethod1()) << std::endl; // SomeMethod1()
  std::cout << NAMEOF(&someVar.SomeMethod2) << std::endl; // SomeMethod2
  std::cout << NAMEOF(SomeMethod3) << std::endl; // SomeMethod3

  std::cout << NAMEOF_TYPE(int[]) << std::endl; // int[]
  std::cout << NAMEOF_TYPE(SomeStruct) << std::endl; // SomeStruct
  std::cout << NAMEOF_TYPE(Long::LL) << std::endl; // LL

  std::cout << NAMEOF_FUN(someVar.SomeMethod1()) << std::endl; // SomeMethod1()
  std::cout << NAMEOF_FUN(&someVar.SomeMethod2) << std::endl; // SomeMethod2
  std::cout << NAMEOF_FUN(SomeMethod3) << std::endl; // SomeMethod3

  std::cout << NAMEOF_VAR(someVar.SomeField) << std::endl; // SomeField
  std::cout << NAMEOF_VAR((&someVar)->SomeField) << std::endl; // SomeField
  std::cout << NAMEOF_VAR(::someVar) << std::endl; // someVar

  return 0;
}