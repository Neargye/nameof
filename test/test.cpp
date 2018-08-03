// nameof test
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

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <nameof.hpp>

#include <string>
#include <stdexcept>

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
    std::cout << nameof::NameofType<T>(false) << std::endl;
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

#if (__cplusplus >= 201402L || (defined(_MSVC_LANG ) && _MSVC_LANG  >= 201402L))
// Compile-time supported by C++14.
TEST_CASE("constexpr") {
  SECTION("NAMEOF") {
    // variable
    constexpr auto cx1 = NAMEOF(somevar);
    static_assert(cx1 == "somevar", "");
    // member
    constexpr auto cx2 = NAMEOF((&somevar)->somefield);
    static_assert(cx2 == "somefield", "");
    // function
    constexpr auto cx3 = NAMEOF(&SomeStruct::SomeMethod2);
    static_assert(cx3 == "SomeMethod2", "");
    // enum
    constexpr auto cx4 = NAMEOF(Color::RED);
    static_assert(cx4 ==  "RED", "");
  }

  SECTION("NAMEOF_RAW") {
    // variable
    constexpr auto cx1 = NAMEOF_RAW(somevar);
    static_assert(cx1 == "somevar", "");
    // member
    constexpr auto cx2 = NAMEOF_RAW((&somevar)->somefield);
    static_assert(cx2 == "(&somevar)->somefield", "");
    // type
    constexpr auto cx3 = NAMEOF_RAW(std::string);
    static_assert(cx3 == "std::string", "");
    // function
    constexpr auto cx4 = NAMEOF_RAW(&SomeStruct::SomeMethod2);
    static_assert(cx4 == "&SomeStruct::SomeMethod2", "");
    // enum
    constexpr auto cx5 = NAMEOF_RAW(Color::RED);
    static_assert(cx5 == "Color::RED", "");
    // macros
    constexpr auto cx6 = NAMEOF_RAW(__cplusplus);
    static_assert(cx6 == "__cplusplus", "");
  }

  SECTION("NAMEOF_TYPE") {
    SomeClass<int> a;

    constexpr auto cx1 = NAMEOF_TYPE(a);
    static_assert(cx1 == "SomeClass", "");

    constexpr auto cx2 = nameof::NameofType<SomeClass<int>>(false);
    static_assert(cx2 == "SomeClass", "");

    constexpr auto cx3 = nameof::NameofType<decltype(a)>(false);
    static_assert(cx3 == "SomeClass", "");
  }

  SECTION("NAMEOF_TYPE_RAW") {
    SomeClass<int> a;

    constexpr auto cx1 = NAMEOF_TYPE_RAW(a);
    constexpr auto cx2 = nameof::NameofType<SomeClass<int>>(true);
    constexpr auto cx3 = nameof::NameofType<decltype(a)>(true);

#if defined(_MSC_VER)
    static_assert(cx1 == "class SomeClass<int>", "");
    static_assert(cx2 == "class SomeClass<int>", "");
    static_assert(cx3 == "class SomeClass<int>", "");
#else
    static_assert(cx1 == "SomeClass<int>", "");
    static_assert(cx2 == "SomeClass<int>", "");
    static_assert(cx3 == "SomeClass<int>", "");
#endif
  }
}
#endif

TEST_CASE("simple name") {
  SECTION("variable") {
    REQUIRE(NAMEOF(somevar) == "somevar");
    REQUIRE(NAMEOF(::somevar) == "somevar");
    REQUIRE(NAMEOF(ptrvar) == "ptrvar");
  }

  SECTION("member") {
    REQUIRE(NAMEOF(somevar.somefield) == "somefield");
    REQUIRE(NAMEOF((&somevar)->somefield) == "somefield");
    REQUIRE(NAMEOF(othervar.ll.field) == "field");
  }

  SECTION("function") {
    REQUIRE(NAMEOF(&SomeStruct::SomeMethod1) == "SomeMethod1");
    REQUIRE(NAMEOF(&SomeStruct::SomeMethod2) == "SomeMethod2");
    REQUIRE(NAMEOF(SomeMethod3) == "SomeMethod3");
    REQUIRE(NAMEOF(SomeMethod4<int>) == "SomeMethod4");
    REQUIRE(NAMEOF(&SomeClass<int>::SomeMethod5) == "SomeMethod5");
    REQUIRE(NAMEOF(&SomeClass<int>::SomeMethod6<long int>) == "SomeMethod6");
  }

  SECTION("enum") {
    REQUIRE(NAMEOF(Color::RED) == "RED");
    REQUIRE(NAMEOF(Color::BLUE) == "BLUE");
  }
}

TEST_CASE("raw name") {
  SECTION("variable") {
    REQUIRE(NAMEOF_RAW(somevar) == "somevar");
    REQUIRE(NAMEOF_RAW(&somevar) ==  "&somevar");
    REQUIRE(NAMEOF_RAW(::somevar) ==  "::somevar");
    REQUIRE(NAMEOF_RAW(ptrvar) ==  "ptrvar");
    REQUIRE(NAMEOF_RAW(*ptrvar) ==  "*ptrvar");
    REQUIRE(NAMEOF_RAW(+somevar.somefield) ==  "+somevar.somefield");
    REQUIRE(NAMEOF_RAW(-somevar.somefield) ==  "-somevar.somefield");
    REQUIRE(NAMEOF_RAW(~somevar.somefield) ==  "~somevar.somefield");
    REQUIRE(NAMEOF_RAW(!somevar.somefield) ==  "!somevar.somefield");
  }

  SECTION("member") {
    REQUIRE(NAMEOF_RAW(somevar.somefield) ==  "somevar.somefield");
    REQUIRE(NAMEOF_RAW((&somevar)->somefield) ==  "(&somevar)->somefield");
    REQUIRE(NAMEOF_RAW(othervar.ll.field) ==  "othervar.ll.field");
  }

  SECTION("type") {
    REQUIRE(NAMEOF_RAW(int[]) == "int[]");
    REQUIRE(NAMEOF_RAW(int) ==  "int");
    REQUIRE(NAMEOF_RAW(const volatile int[]) ==  "const volatile int[]");
    REQUIRE(NAMEOF_RAW(std::string) ==  "std::string");
    REQUIRE(NAMEOF_RAW(SomeStruct) ==  "SomeStruct");
    REQUIRE(NAMEOF_RAW(Long::LL) ==  "Long::LL");
    REQUIRE(NAMEOF_RAW(Color) ==  "Color");
  }

  SECTION("function") {
    REQUIRE(NAMEOF_RAW(&SomeStruct::SomeMethod1) ==  "&SomeStruct::SomeMethod1");
    REQUIRE(NAMEOF_RAW(&SomeStruct::SomeMethod2) ==  "&SomeStruct::SomeMethod2");
    REQUIRE(NAMEOF_RAW(SomeMethod3) ==  "SomeMethod3");
    REQUIRE(NAMEOF_RAW(SomeMethod4<int>) ==  "SomeMethod4<int>");
    REQUIRE(NAMEOF_RAW(&SomeClass<int>::SomeMethod5) ==  "&SomeClass<int>::SomeMethod5");
    REQUIRE(NAMEOF_RAW(&SomeClass<int>::SomeMethod6<int>) ==  "&SomeClass<int>::SomeMethod6<int>");
  }

  SECTION("enum") {
    REQUIRE(NAMEOF_RAW(Color::RED) ==  "Color::RED");
    REQUIRE(NAMEOF_RAW(Color::BLUE) ==  "Color::BLUE");
  }

  SECTION("macros") {
    REQUIRE(NAMEOF_RAW(__cplusplus) ==  "__cplusplus");
    REQUIRE(NAMEOF_RAW(__LINE__) ==  "__LINE__");
    REQUIRE(NAMEOF_RAW(__FILE__) ==  "__FILE__");
  }
}

TEST_CASE("type name") {
  REQUIRE(NAMEOF_TYPE(somevar) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE(ptrvar) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE(refvar) == "SomeStruct");

  REQUIRE(NAMEOF_TYPE(othervar) == "Long");
  REQUIRE(NAMEOF_TYPE(othervar.ll) == "LL");
  REQUIRE(NAMEOF_TYPE(othervar.ll.field) == "int");

  REQUIRE(NAMEOF_TYPE(Color::RED) == "Color");

  REQUIRE(NAMEOF_TYPE(std::string{}) == "basic_string");

  REQUIRE(NAMEOF_TYPE(std::declval<const SomeClass<int>>()) == "SomeClass");
}

TEST_CASE("type raw name") {
#if defined(_MSC_VER)
  REQUIRE(NAMEOF_TYPE_RAW(somevar) == "struct SomeStruct");
  REQUIRE(NAMEOF_TYPE_RAW(ptrvar) == "structSomeStruct*");
  REQUIRE(NAMEOF_TYPE_RAW(refvar) == "structSomeStruct&");

  REQUIRE(NAMEOF_TYPE_RAW(othervar) == "struct Long");
  REQUIRE(NAMEOF_TYPE_RAW(othervar.ll) == "struct Long::LL");
  REQUIRE(NAMEOF_TYPE_RAW(othervar.ll.field) == "int");

  REQUIRE(NAMEOF_TYPE_RAW(Color::RED) == "enum Color");

  REQUIRE(NAMEOF_TYPE_RAW(std::declval<const SomeClass<int>>()) == "const classSomeClass<int>&&");
#else
  REQUIRE(NAMEOF_TYPE_RAW(somevar) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_RAW(ptrvar) == "SomeStruct*");
  REQUIRE(NAMEOF_TYPE_RAW(refvar) == "SomeStruct&");

  REQUIRE(NAMEOF_TYPE_RAW(othervar) == "Long");
  REQUIRE(NAMEOF_TYPE_RAW(othervar.ll) == "Long::LL");
  REQUIRE(NAMEOF_TYPE_RAW(othervar.ll.field) == "int");

  REQUIRE(NAMEOF_TYPE_RAW(Color::RED) == "Color");

  REQUIRE(NAMEOF_TYPE_RAW(std::declval<const SomeClass<int>>()) == "const SomeClass<int>&&");
#endif

}

TEST_CASE("Spaces and Tabs ignored") {
  SECTION("Spaces") {
    REQUIRE(NAMEOF(   somevar   ) ==  "somevar");
    REQUIRE(NAMEOF_RAW(   somevar   ) ==  "somevar");
    REQUIRE(NAMEOF_TYPE(   int{}   ) ==  "int");
    REQUIRE(NAMEOF_TYPE_RAW(   int{}   ) ==  "int");
  }

  SECTION("Tabs") {
    REQUIRE(NAMEOF(	somevar	) ==  "somevar");
    REQUIRE(NAMEOF_RAW(	somevar	) ==  "somevar");
    REQUIRE(NAMEOF_TYPE(	int{}	) ==  "int");
    REQUIRE(NAMEOF_TYPE_RAW(	int{}	) ==  "int");
  }
}
