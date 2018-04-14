// nameof c++ example
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// Copyright (c) 2018 Daniil Goncharov <neargye@gmail.com>.
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

#include <nameof.hpp>

#include <iostream>
#include <stdexcept>

struct SomeStruct {
  int somefield;

  void SomeMethod1(const int i) {
    somefield = i;
    std::cout << "No called!" << std::endl;
  }

  int SomeMethod2() const {
    std::cout << "No called!" << std::endl;
    return somefield;
  }
};

void SomeMethod3() { std::cout << "No called!" << std::endl; }

struct Long {
  struct LL {
    int field;
  };
  LL ll;
};

int somevar;

enum class Color { RED, GREEN, BLUE };

void TestCase1() {
  SomeStruct somevar;
  Long othervar;
  int intvar;
  SomeStruct* ptrvar;
  SomeStruct** ptrptrvar;

  // constexpr
  constexpr auto constexpr_work_fine = NAMEOF(intvar);
  std::cout << constexpr_work_fine << std::endl; // intvar
  // enum
  std::cout << NAMEOF(Color::RED) << std::endl; // RED
  // variable
  std::cout << NAMEOF(somevar) << std::endl; // somevar
  std::cout << NAMEOF(::somevar) << std::endl; // somevar
  std::cout << NAMEOF(&somevar) << std::endl; // somevar
  // member
  std::cout << NAMEOF(somevar.somefield) << std::endl; // somefield
  std::cout << NAMEOF((&somevar)->somefield) << std::endl; // somefield
  // member
  std::cout << NAMEOF(othervar.ll.field) << std::endl; // field
  // variable
  std::cout << NAMEOF(ptrvar) << std::endl; // ptrvar
  std::cout << NAMEOF(*ptrvar) << std::endl; // ptrvar
  // variable
  std::cout << NAMEOF(ptrptrvar) << std::endl; // ptrptrvar
  std::cout << NAMEOF(*ptrptrvar) << std::endl; // ptrptrvar
  std::cout << NAMEOF(**ptrptrvar) << std::endl; // ptrptrvar
  // variable
  std::cout << NAMEOF(+intvar) << std::endl; // intvar
  std::cout << NAMEOF(-intvar) << std::endl; // intvar
  std::cout << NAMEOF(~intvar) << std::endl; // intvar
  std::cout << NAMEOF(!intvar) << std::endl; // intvar
  // function
  std::cout << NAMEOF(&SomeStruct::SomeMethod2) << std::endl; // SomeMethod2
  std::cout << NAMEOF(SomeMethod3) << std::endl; // SomeMethod3
  // type
  std::cout << NAMEOF(int[]) << std::endl; // int[]
  std::cout << NAMEOF(SomeStruct) << std::endl; // SomeStruct
  std::cout << NAMEOF(Long::LL) << std::endl; // LL
  std::cout << NAMEOF(volatile const int) << std::endl; // volatile const int
  // macros
  std::cout << NAMEOF(__LINE__) << std::endl; // __LINE__
  std::cout << NAMEOF(__FILE__) << std::endl; // __FILE__
  // full name
  std::cout << NAMEOF_FULL(somevar.somefield) << std::endl; // somevar.somefield
  std::cout << NAMEOF_FULL(&SomeStruct::SomeMethod2) << std::endl; // &SomeStruct::SomeMethod2
  std::cout << NAMEOF_FULL(Long::LL) << std::endl; // Long::LL
  std::cout << NAMEOF_FULL(std::string) << std::endl; // std::string
}

void TestCase2() {
  const auto div = [](int x, int y) {
    if (y == 0) {
      throw std::invalid_argument(std::string(NAMEOF(y)).append(" should not be zero!"));
    }
    return x / y;
  };

  try {
    const int z = div(10, 0);
    std::cout << z << std::endl;
  } catch (const std::exception& e) {
    std::cout << e.what() << std::endl; // y should not be zero!
  }
}

void TestCase3() {
  std::cout << NAMEOF(TestCase3) << " method entry" << std::endl; // TestCase3 method entry
}

int main() {
  TestCase1();

  TestCase2();

  TestCase3();

  return 0;
}
