// nameof() c++11 test
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// Copyright (c) 2018 Terik23 <neargye@gmail.com>.
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

int someVar = 0;

enum class Color { RED, GREEN, BLUE };

TEST_CASE("NAMEOF") {
  SomeStruct someVar;
  Long otherVar;
  int intValue;
  SomeStruct* ptrVar;
  SomeStruct** ptrptrVar;

  SECTION("constexpr") {
    constexpr auto constexpr_work_fine = NAMEOF(intValue);
    REQUIRE(std::strcmp(constexpr_work_fine, "intValue") == 0);
  }

  SECTION("NAMEOF") {
    REQUIRE(std::strcmp(NAMEOF(someVar), "someVar") == 0);
    REQUIRE(std::strcmp(NAMEOF(Color::RED), "RED") == 0);
    REQUIRE(std::strcmp(NAMEOF(someVar.SomeField), "SomeField") == 0);
    REQUIRE(std::strcmp(NAMEOF((&someVar)->SomeField), "SomeField") == 0);
    REQUIRE(std::strcmp(NAMEOF(::someVar), "someVar") == 0);
    REQUIRE(std::strcmp(NAMEOF(otherVar.LLField.LLLField), "LLLField") == 0);
    REQUIRE(std::strcmp(NAMEOF(&someVar), "someVar") == 0);
    REQUIRE(std::strcmp(NAMEOF(ptrVar), "ptrVar") == 0);
    REQUIRE(std::strcmp(NAMEOF(*ptrVar), "ptrVar") == 0);
    REQUIRE(std::strcmp(NAMEOF(ptrptrVar), "ptrptrVar") == 0);
    REQUIRE(std::strcmp(NAMEOF(*ptrptrVar), "ptrptrVar") == 0);
    REQUIRE(std::strcmp(NAMEOF(**ptrptrVar), "ptrptrVar") == 0);
    REQUIRE(std::strcmp(NAMEOF(+intValue), "intValue") == 0);
    REQUIRE(std::strcmp(NAMEOF(-intValue), "intValue") == 0);
    REQUIRE(std::strcmp(NAMEOF(~intValue), "intValue") == 0);
    REQUIRE(std::strcmp(NAMEOF(!intValue), "intValue") == 0);
    REQUIRE(std::strcmp(NAMEOF(someVar.SomeMethod1()), "SomeMethod1()") == 0);
    REQUIRE(std::strcmp(NAMEOF(&SomeStruct::SomeMethod2), "SomeMethod2") == 0);
    REQUIRE(std::strcmp(NAMEOF(SomeMethod3), "SomeMethod3") == 0);
  }

  SECTION("NAMEOF_VARIABLE") {
    REQUIRE(std::strcmp(NAMEOF(someVar.SomeField), "SomeField") == 0);
    REQUIRE(std::strcmp(NAMEOF((&someVar)->SomeField), "SomeField") == 0);
    REQUIRE(std::strcmp(NAMEOF(::someVar), "someVar") == 0);
  }

  SECTION("NAMEOF_FUNCTION") {
    REQUIRE(std::strcmp(NAMEOF(someVar.SomeMethod1()), "SomeMethod1()") == 0);
    REQUIRE(std::strcmp(NAMEOF(&SomeStruct::SomeMethod2), "SomeMethod2") == 0);
    REQUIRE(std::strcmp(NAMEOF(SomeMethod3), "SomeMethod3") == 0);
  }
}

TEST_CASE("NAMEOF_TYPE") {
  REQUIRE(std::strcmp(NAMEOF_TYPE(int[]), "int[]") == 0);
  REQUIRE(std::strcmp(NAMEOF_TYPE(int), "int") == 0);
  REQUIRE(std::strcmp(NAMEOF_TYPE(std::string), "string") == 0);
  REQUIRE(std::strcmp(NAMEOF_TYPE(SomeStruct), "SomeStruct") == 0);
  REQUIRE(std::strcmp(NAMEOF_TYPE(Long::LL), "LL") == 0);
}

TEST_CASE("NAMEOF_FULL") {
  SomeStruct someVar;
  Long otherVar;
  int intValue;
  SomeStruct* ptrVar;
  SomeStruct** ptrptrVar;

  SECTION("NAMEOF_FULL") {
    REQUIRE(std::strcmp(NAMEOF_FULL(someVar), "someVar") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(someVar.SomeField), "someVar.SomeField") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL((&someVar)->SomeField), "(&someVar)->SomeField") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(::someVar), "::someVar") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(otherVar.LLField.LLLField), "otherVar.LLField.LLLField") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(&someVar), "&someVar") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(ptrVar), "ptrVar") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(*ptrVar), "*ptrVar") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(ptrptrVar), "ptrptrVar") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(*ptrptrVar), "*ptrptrVar") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(**ptrptrVar), "**ptrptrVar") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(+intValue), "+intValue") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(-intValue), "-intValue") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(~intValue), "~intValue") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(!intValue), "!intValue") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(someVar.SomeMethod1()), "someVar.SomeMethod1()") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(&SomeStruct::SomeMethod2), "&SomeStruct::SomeMethod2") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(SomeMethod3), "SomeMethod3") == 0);
  }

  SECTION("NAMEOF_VARIABLE_FULL") {
    REQUIRE(std::strcmp(NAMEOF_FULL(someVar.SomeField), "someVar.SomeField") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL((&someVar)->SomeField), "(&someVar)->SomeField") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(::someVar), "::someVar") == 0);
  }

  SECTION("NAMEOF_FUNCTION_FULL") {
    REQUIRE(std::strcmp(NAMEOF_FULL(someVar.SomeMethod1()), "someVar.SomeMethod1()") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(&SomeStruct::SomeMethod2), "&SomeStruct::SomeMethod2") == 0);
    REQUIRE(std::strcmp(NAMEOF_FULL(SomeMethod3), "SomeMethod3") == 0);
  }
}

TEST_CASE("NAMEOF_TYPE_FULL") {
  REQUIRE(std::strcmp(NAMEOF_TYPE_FULL(int[]), "int[]") == 0);
  REQUIRE(std::strcmp(NAMEOF_TYPE_FULL(int), "int") == 0);
  REQUIRE(std::strcmp(NAMEOF_TYPE_FULL(std::string), "std::string") == 0);
  REQUIRE(std::strcmp(NAMEOF_TYPE_FULL(SomeStruct), "SomeStruct") == 0);
  REQUIRE(std::strcmp(NAMEOF_TYPE_FULL(Long::LL), "Long::LL") == 0);
}