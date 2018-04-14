// nameof c++ test
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

#include <cstring>
#include <string>
#include <iostream>

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

TEST_CASE("constexpr") {
  SomeStruct somevar;

  SECTION("NAMEOF") {
    // variable
    constexpr auto cx1 = NAMEOF((&somevar)->somefield);
    REQUIRE(std::strcmp(cx1, "somefield") == 0);
    // type
    constexpr auto cx2 = NAMEOF(std::string);
    REQUIRE(std::strcmp(cx2, "string") == 0);
    // function
    constexpr auto cx3 = NAMEOF(&SomeStruct::SomeMethod2);
    REQUIRE(std::strcmp(cx3, "SomeMethod2") == 0);
    // enum
    constexpr auto cx4 = NAMEOF(Color::RED);
    REQUIRE(std::strcmp(cx4, "RED") == 0);
    // macros
    constexpr auto cx5 = NAMEOF(__cplusplus);
    REQUIRE(std::strcmp(cx5, "__cplusplus") == 0);
  }

  SECTION("NAMEOF_FULL") {
    // variable
    constexpr auto cx1 = NAMEOF_FULL((&somevar)->somefield);
    REQUIRE(std::strcmp(cx1, "(&somevar)->somefield") == 0);
    // type
    constexpr auto cx2 = NAMEOF_FULL(std::string);
    REQUIRE(std::strcmp(cx2, "std::string") == 0);
    // function
    constexpr auto cx3 = NAMEOF_FULL(&SomeStruct::SomeMethod2);
    REQUIRE(std::strcmp(cx3, "&SomeStruct::SomeMethod2") == 0);
    // enum
    constexpr auto cx4 = NAMEOF_FULL(Color::RED);
    REQUIRE(std::strcmp(cx4, "Color::RED") == 0);
    // macros
    constexpr auto cx5 = NAMEOF_FULL(__cplusplus);
    REQUIRE(std::strcmp(cx5, "__cplusplus") == 0);
  }
}

TEST_CASE("NAMEOF") {
  SomeStruct somevar;
  Long othervar;
  int intvar;
  SomeStruct* ptrvar;
  SomeStruct** ptrptrvar;

  SECTION("variable") {
    REQUIRE(std::strcmp(NAMEOF(somevar), "somevar") == 0);
    REQUIRE(std::strcmp(NAMEOF(&somevar), "somevar") == 0);
    REQUIRE(std::strcmp(NAMEOF(::somevar), "somevar") == 0);

    REQUIRE(std::strcmp(NAMEOF(somevar.somefield), "somefield") == 0);
    REQUIRE(std::strcmp(NAMEOF((&somevar)->somefield), "somefield") == 0);

    REQUIRE(std::strcmp(NAMEOF(othervar.ll.field), "field") == 0);

    REQUIRE(std::strcmp(NAMEOF(ptrvar), "ptrvar") == 0);
    REQUIRE(std::strcmp(NAMEOF(*ptrvar), "ptrvar") == 0);

    REQUIRE(std::strcmp(NAMEOF(ptrptrvar), "ptrptrvar") == 0);
    REQUIRE(std::strcmp(NAMEOF(*ptrptrvar), "ptrptrvar") == 0);
    REQUIRE(std::strcmp(NAMEOF(**ptrptrvar), "ptrptrvar") == 0);

    REQUIRE(std::strcmp(NAMEOF(+intvar), "intvar") == 0);
    REQUIRE(std::strcmp(NAMEOF(-intvar), "intvar") == 0);
    REQUIRE(std::strcmp(NAMEOF(~intvar), "intvar") == 0);
    REQUIRE(std::strcmp(NAMEOF(!intvar), "intvar") == 0);
  }

  SECTION("type") {
    REQUIRE(std::strcmp(NAMEOF(int[]), "int[]") == 0);
    REQUIRE(std::strcmp(NAMEOF(int), "int") == 0);
    REQUIRE(std::strcmp(NAMEOF(const volatile int[]), "const volatile int[]") == 0);
    REQUIRE(std::strcmp(NAMEOF(std::string), "string") == 0);

    REQUIRE(std::strcmp(NAMEOF(SomeStruct), "SomeStruct") == 0);
    REQUIRE(std::strcmp(NAMEOF(Long::LL), "LL") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(Color), "Color") == 0);
  }

  SECTION("function") {
    REQUIRE(std::strcmp(NAMEOF(&SomeStruct::SomeMethod2), "SomeMethod2") == 0);
    REQUIRE(std::strcmp(NAMEOF(SomeMethod3), "SomeMethod3") == 0);
  }

  SECTION("enum") {
    REQUIRE(std::strcmp(NAMEOF(Color::RED), "RED") == 0);
    REQUIRE(std::strcmp(NAMEOF(Color::BLUE), "BLUE") == 0);
  }

  SECTION("macros") {
    REQUIRE(std::strcmp(NAMEOF(__cplusplus), "__cplusplus") == 0);
    REQUIRE(std::strcmp(NAMEOF(__LINE__), "__LINE__") == 0);
    REQUIRE(std::strcmp(NAMEOF(__FILE__), "__FILE__") == 0);
  }
}

TEST_CASE("NAMEOF_FULL") {
  SomeStruct somevar;
  Long othervar;
  int intvar;
  SomeStruct* ptrvar;
  SomeStruct** ptrptrvar;

  SECTION("variable") {
    REQUIRE(std::strcmp(NAMEOF_FULL(somevar), "somevar") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(&somevar), "&somevar") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(somevar.somefield), "somevar.somefield") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL((&somevar)->somefield), "(&somevar)->somefield") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(::somevar), "::somevar") == 0);

    REQUIRE(std::strcmp(NAMEOF_FULL(othervar.ll.field), "othervar.ll.field") == 0);

    REQUIRE(std::strcmp(NAMEOF_FULL(ptrvar), "ptrvar") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(*ptrvar), "*ptrvar") == 0);

    REQUIRE(std::strcmp(NAMEOF_FULL(ptrptrvar), "ptrptrvar") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(*ptrptrvar), "*ptrptrvar") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(**ptrptrvar), "**ptrptrvar") == 0);

    REQUIRE(std::strcmp(NAMEOF_FULL(+intvar), "+intvar") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(-intvar), "-intvar") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(~intvar), "~intvar") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(!intvar), "!intvar") == 0);

  }

  SECTION("type") {
    REQUIRE(std::strcmp(NAMEOF_FULL(int[]), "int[]") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(int), "int") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(const volatile int[]), "const volatile int[]") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(std::string), "std::string") == 0);

    REQUIRE(std::strcmp(NAMEOF_FULL(SomeStruct), "SomeStruct") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(Long::LL), "Long::LL") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(Color), "Color") == 0);
  }

  SECTION("function") {
    REQUIRE(std::strcmp(NAMEOF_FULL(&SomeStruct::SomeMethod2), "&SomeStruct::SomeMethod2") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(SomeMethod3), "SomeMethod3") == 0);
  }

  SECTION("enum") {
    REQUIRE(std::strcmp(NAMEOF_FULL(Color::RED), "Color::RED") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(Color::BLUE), "Color::BLUE") == 0);
  }

  SECTION("macros") {
    REQUIRE(std::strcmp(NAMEOF_FULL(__cplusplus), "__cplusplus") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(__LINE__), "__LINE__") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(__FILE__), "__FILE__") == 0);
  }
}
