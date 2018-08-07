// nameof example
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
#include <string>
#include <stdexcept>
#include <typeinfo>

constexpr long double operator"" _deg(long double deg) {
  return deg * 3.141592 / 180.0;
}

std::string operator"" _string(const char* str, std::size_t) {
  return std::string{str};
}

struct SomeStruct {
  int somefield = 0;

  void SomeMethod1(const int i) { somefield = i; }

  int SomeMethod2() const { return somefield; }
};

void SomeMethod3() {
  std::cout << NAMEOF(SomeMethod3) << " no called!" << std::endl;
}

template <typename T>
T SomeMethod4() {
  return T{};
}

template <typename T>
class SomeClass {
public:
  void SomeMethod5() const {
    std::cout << nameof::NameofType<T>() << std::endl;
  }

  template <typename C>
  C SomeMethod6() const {
    C t{};
    std::cout << NAMEOF_TYPE(t) << std::endl;
    return t;
  }
};

struct Long {
  struct LL {
    int field = 0;
  };
  LL ll;
};

enum class Color { RED, GREEN, BLUE };

SomeStruct somevar;
Long othervar;
SomeStruct& refvar = somevar;
SomeStruct* ptrvar = &somevar;

int main() {
#if (__cplusplus >= 201402L || (defined(_MSVC_LANG) && _MSC_VER >= 1910 && _MSVC_LANG >= 201402L))
  // Compile-time supported by C++14.
  constexpr auto constexpr_work_fine = NAMEOF(somevar);
  static_assert("somevar" == constexpr_work_fine, "");
#endif

  // Enum name.
  std::cout << NAMEOF(Color::RED) << std::endl; // RED

  // Variable name.
  std::cout << NAMEOF(somevar) << std::endl; // somevar
  std::cout << NAMEOF(::somevar) << std::endl; // somevar

  // Member name.
  std::cout << NAMEOF(somevar.somefield) << std::endl; // somefield
  std::cout << NAMEOF((&somevar)->somefield) << std::endl; // somefield
  std::cout << NAMEOF(othervar.ll.field) << std::endl; // field

  // Function name.
  std::cout << NAMEOF(&SomeStruct::SomeMethod1) << std::endl; // SomeMethod1
  std::cout << NAMEOF(&SomeStruct::SomeMethod2) << std::endl; // SomeMethod2
  std::cout << NAMEOF(SomeMethod3) << std::endl; // SomeMethod3
  std::cout << NAMEOF(SomeMethod4<int>) << std::endl; // SomeMethod4
  std::cout << NAMEOF(&SomeClass<int>::SomeMethod5) << std::endl; // SomeMethod5
  std::cout << NAMEOF(&SomeClass<int>::SomeMethod6<long int>) << std::endl; // SomeMethod6

  // Type name.
  std::cout << NAMEOF_TYPE(somevar) << std::endl; // SomeStruct
  std::cout << NAMEOF_TYPE(refvar) << std::endl; // SomeStruct
  std::cout << NAMEOF_TYPE(ptrvar) << std::endl; // SomeStruct
  std::cout << NAMEOF_TYPE(othervar.ll) << std::endl; // LL
  std::cout << NAMEOF_TYPE(othervar.ll.field) << std::endl; // int
  std::cout << NAMEOF_TYPE(Color::RED) << std::endl; // Color

  std::cout << NAMEOF_TYPE(SomeClass<int>{}) << std::endl; // SomeClass

  // Type full name.
  std::cout << NAMEOF_TYPE_RAW(othervar.ll) << std::endl; // Long::LL
  std::cout << NAMEOF_TYPE_RAW(std::declval<const SomeClass<int>>()) << std::endl; // const SomeClass<int>&&

  // Raw name.
  std::cout << NAMEOF_RAW(volatile const int) << std::endl; // volatile const int
  std::cout << NAMEOF_RAW(__LINE__) << std::endl; // __LINE__
  std::cout << NAMEOF_RAW(somevar.somefield) << std::endl; // somevar.somefield
  std::cout << NAMEOF_RAW(&SomeStruct::SomeMethod1) << std::endl; // &SomeStruct::SomeMethod1
  std::cout << NAMEOF_RAW(Long::LL) << std::endl; // Long::LL

  const auto div = [](int x, int y) -> int {
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

  /* Remarks */

  // Spaces and Tabs ignored.
  std::cout << NAMEOF(   somevar   ) << std::endl; // somevar
  std::cout << NAMEOF(	somevar	) << std::endl; // somevar

#if 0
  // This expression does not have a name.
  std::cout << NAMEOF("Bad case") << std::endl; // '"Bad case"'
  std::cout << NAMEOF("Bad case"_string) << std::endl; // '"Bad case"_string'
  std::cout << NAMEOF("somevar.somefield") << std::endl; // 'somefield"'
  std::cout << NAMEOF(42.0) << std::endl; // '0'
  std::cout << NAMEOF(42.f) << std::endl; // 'f'
  std::cout << NAMEOF(42) << std::endl; // '42'
  std::cout << NAMEOF(42.0_deg) << std::endl; // '0_deg'
  std::cout << NAMEOF(std::string()) << std::endl; // 'string()'
  std::cout << NAMEOF(std::string{}) << std::endl; // "string{}'
  std::cout << NAMEOF(std::string{"test"}) << std::endl; // 'string{"test"}'
  std::cout << NAMEOF(SomeMethod4<int>()) << std::endl; // '()'
  std::cout << NAMEOF(std::basic_string<char>) << std::endl; // ''
  std::cout << NAMEOF(ptrvar[0]) << std::endl; // 'ptrvar[0]'
  std::cout << NAMEOF(intvar + intvar) << std::endl; // ' intvar'
  std::cout << NAMEOF(NAMEOF(intvar)) << std::endl; // 'NAMEOF(intvar)'
  std::cout << NAMEOF(std::cout << intvar << std::endl) << std::endl; // 'endl'
  std::cout << NAMEOF(decltype(intvar)) << std::endl; // 'decltype(intvar)'
  std::cout << NAMEOF(typeid(intvar)) << std::endl; // 'typeid(intvar)'
  std::cout << NAMEOF((intvar)) << std::endl; // '(intvar)'
#endif

  return 0;
}
