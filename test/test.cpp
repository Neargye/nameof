// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2019 Daniil Goncharov <neargye@gmail.com>.
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
  throw std::runtime_error{"should not be called!"};
}

template <typename T, typename U>
std::string SomeMethod4(U) {
  throw std::runtime_error{"should not be called!"};
}

template <typename T>
class SomeClass {
public:
  void SomeMethod5() const {
    std::cout << NAMEOF_TYPE(T) << std::endl;
  }

  template <typename C>
  C SomeMethod6() const {
    C t{};
    std::cout << NAMEOF_VAR_TYPE(t) << std::endl;
    return t;
  }
};

struct Long {
  struct LL {
    int field = 0;
  };
  LL ll;
};

enum class Color : int { RED = -10, GREEN = 0, BLUE = 10 };

enum Directions : unsigned int { Up, Down, Right, Left };

SomeStruct struct_var;
Long othervar;
SomeStruct * ptr_s = &struct_var;
SomeStruct & ref_s = struct_var;

SomeClass<int> class_var;
const SomeClass<int> volatile * ptr_c = nullptr;

const Color color = Color::RED;
const Directions directions = Directions::Right;

TEST_CASE("NAMEOF") {
  SECTION("variable") {
    REQUIRE(NAMEOF(othervar) == "othervar");
    REQUIRE(NAMEOF(struct_var) == "struct_var");
    REQUIRE(NAMEOF(::struct_var) == "struct_var");
    REQUIRE(NAMEOF(ptr_s) == "ptr_s");
  }

  SECTION("member") {
    REQUIRE(NAMEOF(struct_var.somefield) == "somefield");
    REQUIRE(NAMEOF((&struct_var)->somefield) == "somefield");
    REQUIRE(NAMEOF(othervar.ll.field) == "field");
  }

  SECTION("function") {
    REQUIRE(NAMEOF(&SomeStruct::SomeMethod1) == "SomeMethod1");
    REQUIRE(NAMEOF(struct_var.SomeMethod1(1)) == "SomeMethod1");
    REQUIRE(NAMEOF(&SomeStruct::SomeMethod2) == "SomeMethod2");
    REQUIRE(NAMEOF(struct_var.SomeMethod2()) == "SomeMethod2");
    REQUIRE(NAMEOF(SomeMethod3) == "SomeMethod3");
    REQUIRE(NAMEOF(SomeMethod3()) == "SomeMethod3");
    REQUIRE(NAMEOF(SomeMethod4<int, float>) == "SomeMethod4");
    REQUIRE(NAMEOF(SomeMethod4<int, float>(1.0f)) == "SomeMethod4");
    REQUIRE(NAMEOF(&SomeClass<int>::SomeMethod5) == "SomeMethod5");
    REQUIRE(NAMEOF(class_var.SomeMethod5()) == "SomeMethod5");
    REQUIRE(NAMEOF(&SomeClass<int>::SomeMethod6<long int>) == "SomeMethod6");
    REQUIRE(NAMEOF(class_var.SomeMethod6<long int>()) == "SomeMethod6");
  }

  SECTION("enum") {
    REQUIRE(NAMEOF(Color::RED) == "RED");
    REQUIRE(NAMEOF(Color::BLUE) == "BLUE");
  }
}

TEST_CASE("NAMEOF_FULL") {
  SECTION("variable") {
    REQUIRE(NAMEOF_FULL(othervar) == "othervar");
    REQUIRE(NAMEOF_FULL(struct_var) == "struct_var");
    REQUIRE(NAMEOF_FULL(::struct_var) == "struct_var");
    REQUIRE(NAMEOF_FULL(ptr_s) == "ptr_s");
  }

  SECTION("member") {
    REQUIRE(NAMEOF_FULL(struct_var.somefield) == "somefield");
    REQUIRE(NAMEOF_FULL((&struct_var)->somefield) == "somefield");
    REQUIRE(NAMEOF_FULL(othervar.ll.field) == "field");
  }

  SECTION("function") {
    REQUIRE(NAMEOF_FULL(&SomeStruct::SomeMethod1) == "SomeMethod1");
    REQUIRE(NAMEOF_FULL(struct_var.SomeMethod1(1)) == "SomeMethod1");
    REQUIRE(NAMEOF_FULL(&SomeStruct::SomeMethod2) == "SomeMethod2");
    REQUIRE(NAMEOF_FULL(struct_var.SomeMethod2()) == "SomeMethod2");
    REQUIRE(NAMEOF_FULL(SomeMethod3) == "SomeMethod3");
    REQUIRE(NAMEOF_FULL(SomeMethod3()) == "SomeMethod3");
    REQUIRE(NAMEOF_FULL(SomeMethod4<int, float>) == "SomeMethod4<int, float>");
    REQUIRE(NAMEOF_FULL(SomeMethod4<int, float>(1.0f)) == "SomeMethod4<int, float>");
    REQUIRE(NAMEOF_FULL(&SomeClass<int>::SomeMethod5) == "SomeMethod5");
    REQUIRE(NAMEOF_FULL(class_var.SomeMethod5()) == "SomeMethod5");
    REQUIRE(NAMEOF_FULL(&SomeClass<int>::SomeMethod6<long int>) == "SomeMethod6<long int>");
    REQUIRE(NAMEOF_FULL(class_var.SomeMethod6<long int>()) == "SomeMethod6<long int>");
  }

  SECTION("enum") {
    REQUIRE(NAMEOF_FULL(Color::RED) == "RED");
    REQUIRE(NAMEOF_FULL(Color::BLUE) == "BLUE");
  }
}

TEST_CASE("NAMEOF_RAW") {
  SECTION("variable") {
    REQUIRE(NAMEOF_RAW(othervar) == "othervar");
    REQUIRE(NAMEOF_RAW(struct_var) == "struct_var");
    REQUIRE(NAMEOF_RAW(&struct_var) == "&struct_var");
    REQUIRE(NAMEOF_RAW(::struct_var) == "::struct_var");
    REQUIRE(NAMEOF_RAW(ptr_s) == "ptr_s");
    REQUIRE(NAMEOF_RAW(*ptr_s) == "*ptr_s");
    REQUIRE(NAMEOF_RAW(ptr_s[0]) == "ptr_s[0]");
  }

  SECTION("member") {
    REQUIRE(NAMEOF_RAW(struct_var.somefield) == "struct_var.somefield");
    REQUIRE(NAMEOF_RAW(struct_var.somefield++) == "struct_var.somefield++");
    REQUIRE(NAMEOF_RAW((&struct_var)->somefield) == "(&struct_var)->somefield");
    REQUIRE(NAMEOF_RAW(othervar.ll.field) == "othervar.ll.field");
    REQUIRE(NAMEOF_RAW(+struct_var.somefield) == "+struct_var.somefield");
    REQUIRE(NAMEOF_RAW(-struct_var.somefield) == "-struct_var.somefield");
    REQUIRE(NAMEOF_RAW(~struct_var.somefield) == "~struct_var.somefield");
    REQUIRE(NAMEOF_RAW(!struct_var.somefield) == "!struct_var.somefield");
    REQUIRE(NAMEOF_RAW(struct_var.somefield + ref_s.somefield) == "struct_var.somefield + ref_s.somefield");
  }

  SECTION("function") {
    REQUIRE(NAMEOF_RAW(&SomeStruct::SomeMethod1) == "&SomeStruct::SomeMethod1");
    REQUIRE(NAMEOF_RAW(struct_var.SomeMethod1(1)) == "struct_var.SomeMethod1(1)");
    REQUIRE(NAMEOF_RAW(&SomeStruct::SomeMethod2) == "&SomeStruct::SomeMethod2");
    REQUIRE(NAMEOF_RAW(struct_var.SomeMethod2()) == "struct_var.SomeMethod2()");
    REQUIRE(NAMEOF_RAW(SomeMethod3) == "SomeMethod3");
    REQUIRE(NAMEOF_RAW(SomeMethod3()) == "SomeMethod3()");
    REQUIRE(NAMEOF_RAW(SomeMethod4<int, float>) == "SomeMethod4<int, float>");
    REQUIRE(NAMEOF_RAW(SomeMethod4<int, float>(1.0f)) == "SomeMethod4<int, float>(1.0f)");
    REQUIRE(NAMEOF_RAW(&SomeClass<int>::SomeMethod5) == "&SomeClass<int>::SomeMethod5");
    REQUIRE(NAMEOF_RAW(class_var.SomeMethod5()) == "class_var.SomeMethod5()");
    REQUIRE(NAMEOF_RAW(&SomeClass<int>::SomeMethod6<long int>) == "&SomeClass<int>::SomeMethod6<long int>");
    REQUIRE(NAMEOF_RAW(class_var.SomeMethod6<long int>()) == "class_var.SomeMethod6<long int>()");
  }

  SECTION("enum") {
    REQUIRE(NAMEOF_RAW(Color::RED) == "Color::RED");
    REQUIRE(NAMEOF_RAW(Color::BLUE) == "Color::BLUE");
  }

  SECTION("macro") {
    REQUIRE(NAMEOF_RAW(__cplusplus) == "__cplusplus");
    REQUIRE(NAMEOF_RAW(__LINE__) == "__LINE__");
    REQUIRE(NAMEOF_RAW(__FILE__) == "__FILE__");
  }
}

TEST_CASE("NAMEOF_ENUM") {
#if defined(__clang__) || (defined(__GNUC__) && __GNUC__ >= 9) || defined(_MSC_VER)
  constexpr Color color_ = Color::BLUE;
  Color m[3] = {Color::RED, Color::GREEN, Color::BLUE};

  REQUIRE(NAMEOF_ENUM(Color::RED) == "RED");
  REQUIRE(NAMEOF_ENUM(color) == "RED");

  constexpr auto color_name = NAMEOF_ENUM(color_);
  REQUIRE(color_name == "BLUE");
  REQUIRE(NAMEOF_ENUM(Color::BLUE) == "BLUE");
  REQUIRE(NAMEOF_ENUM(color_) == "BLUE");

  REQUIRE(NAMEOF_ENUM(m[1]) == "GREEN");

  REQUIRE(NAMEOF_ENUM(Directions::Right) == "Right");
  REQUIRE(NAMEOF_ENUM(directions) == "Right");

  REQUIRE(NAMEOF_ENUM((Color)-100).empty());
  REQUIRE(NAMEOF_ENUM((Color)100).empty());
  REQUIRE(NAMEOF_ENUM((Directions)100).empty());
  REQUIRE(NAMEOF_ENUM((Directions)100).empty());
#endif
}

TEST_CASE("nameof::nameof_enum") {
#if defined(__clang__) || (defined(__GNUC__) && __GNUC__ >= 9) || defined(_MSC_VER)
  constexpr Color color_ = Color::BLUE;
  Color m[3] = {Color::RED, Color::GREEN, Color::BLUE};

  REQUIRE(nameof::nameof_enum(Color::RED) == "RED");
  REQUIRE(nameof::nameof_enum(color) == "RED");

  constexpr auto color_name = nameof::nameof_enum(color_);
  REQUIRE(color_name == "BLUE");
  REQUIRE(nameof::nameof_enum(Color::BLUE) == "BLUE");
  REQUIRE(nameof::nameof_enum(color_) == "BLUE");

  REQUIRE(nameof::nameof_enum(m[1]) == "GREEN");

  REQUIRE(nameof::nameof_enum(Directions::Right) == "Right");
  REQUIRE(nameof::nameof_enum(directions) == "Right");

  REQUIRE(nameof::nameof_enum((Color)-100).empty());
  REQUIRE(nameof::nameof_enum((Color)100).empty());
  REQUIRE(nameof::nameof_enum((Directions)100).empty());
  REQUIRE(nameof::nameof_enum((Directions)100).empty());
#endif
}

TEST_CASE("NAMEOF_VAR_TYPE") {
#if defined(__clang__)
  REQUIRE(NAMEOF_VAR_TYPE(struct_var) == "SomeStruct");
  REQUIRE(NAMEOF_VAR_TYPE(ptr_s) == "SomeStruct *");
  REQUIRE(NAMEOF_VAR_TYPE(ref_s) == "SomeStruct &");

  REQUIRE(NAMEOF_VAR_TYPE(ptr_c) == "const volatile SomeClass<int> *");

  REQUIRE(NAMEOF_VAR_TYPE(othervar) == "Long");
  REQUIRE(NAMEOF_VAR_TYPE(othervar.ll) == "Long::LL");
  REQUIRE(NAMEOF_VAR_TYPE(othervar.ll.field) == "int");

  REQUIRE(NAMEOF_VAR_TYPE(Color::RED) == "Color");

  REQUIRE(NAMEOF_VAR_TYPE(std::declval<const SomeClass<int>>()) == "const SomeClass<int> &&");
#elif defined(_MSC_VER)
  REQUIRE(NAMEOF_VAR_TYPE(struct_var) == "struct SomeStruct");
  REQUIRE(NAMEOF_VAR_TYPE(ptr_s) == "struct SomeStruct *");
  REQUIRE(NAMEOF_VAR_TYPE(ref_s) == "struct SomeStruct &");

  REQUIRE(NAMEOF_VAR_TYPE(ptr_c) == "class SomeClass<int> const volatile *");

  REQUIRE(NAMEOF_VAR_TYPE(othervar) == "struct Long");
  REQUIRE(NAMEOF_VAR_TYPE(othervar.ll) == "struct Long::LL");
  REQUIRE(NAMEOF_VAR_TYPE(othervar.ll.field) == "int");

  REQUIRE(NAMEOF_VAR_TYPE(Color::RED) == "enum Color");

  REQUIRE(NAMEOF_VAR_TYPE(std::declval<const SomeClass<int>>()) == "class SomeClass<int> const &&");
#elif defined(__GNUC__)
  REQUIRE(NAMEOF_VAR_TYPE(struct_var) == "SomeStruct");
  REQUIRE(NAMEOF_VAR_TYPE(ptr_s) == "SomeStruct*");
  REQUIRE(NAMEOF_VAR_TYPE(ref_s) == "SomeStruct&");

  REQUIRE(NAMEOF_VAR_TYPE(ptr_c) == "const volatile SomeClass<int>*");

  REQUIRE(NAMEOF_VAR_TYPE(othervar) == "Long");
  REQUIRE(NAMEOF_VAR_TYPE(othervar.ll) == "Long::LL");
  REQUIRE(NAMEOF_VAR_TYPE(othervar.ll.field) == "int");

  REQUIRE(NAMEOF_VAR_TYPE(Color::RED) == "Color");

  REQUIRE(NAMEOF_VAR_TYPE(std::declval<const SomeClass<int>>()) == "const SomeClass<int>&&");
#endif
}

TEST_CASE("NAMEOF_TYPE") {
#if defined(__clang__)
  REQUIRE(NAMEOF_TYPE(decltype(struct_var)) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE(decltype(ptr_s)) == "SomeStruct *");
  REQUIRE(NAMEOF_TYPE(decltype(ref_s)) == "SomeStruct &");
  REQUIRE(NAMEOF_TYPE(SomeStruct) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE(SomeStruct *) == "SomeStruct *");
  REQUIRE(NAMEOF_TYPE(SomeStruct &) == "SomeStruct &");
  REQUIRE(NAMEOF_TYPE(const SomeStruct volatile *) == "const volatile SomeStruct *");

  REQUIRE(NAMEOF_TYPE(SomeClass<int>) == "SomeClass<int>");
  REQUIRE(NAMEOF_TYPE(const SomeClass<int> volatile *) == "const volatile SomeClass<int> *");

  REQUIRE(NAMEOF_TYPE(decltype(othervar)) == "Long");
  REQUIRE(NAMEOF_TYPE(Long) == "Long");
  REQUIRE(NAMEOF_TYPE(Long::LL) == "Long::LL");

  REQUIRE(NAMEOF_TYPE(Color) == "Color");
#elif defined(_MSC_VER)
  REQUIRE(NAMEOF_TYPE(decltype(struct_var)) == "struct SomeStruct");
  REQUIRE(NAMEOF_TYPE(decltype(ptr_s)) == "struct SomeStruct *");
  REQUIRE(NAMEOF_TYPE(decltype(ref_s)) == "struct SomeStruct &");
  REQUIRE(NAMEOF_TYPE(SomeStruct) == "struct SomeStruct");
  REQUIRE(NAMEOF_TYPE(SomeStruct *) == "struct SomeStruct *");
  REQUIRE(NAMEOF_TYPE(SomeStruct &) == "struct SomeStruct &");
  REQUIRE(NAMEOF_TYPE(const SomeStruct volatile *) == "struct SomeStruct const volatile *");

  REQUIRE(NAMEOF_TYPE(SomeClass<int>) == "class SomeClass<int>");
  REQUIRE(NAMEOF_TYPE(const SomeClass<int> volatile *) == "class SomeClass<int> const volatile *");

  REQUIRE(NAMEOF_TYPE(decltype(othervar)) == "struct Long");
  REQUIRE(NAMEOF_TYPE(Long) == "struct Long");
  REQUIRE(NAMEOF_TYPE(Long::LL) == "struct Long::LL");

  REQUIRE(NAMEOF_TYPE(Color) == "enum Color");
#elif defined(__GNUC__)
  REQUIRE(NAMEOF_TYPE(decltype(struct_var)) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE(decltype(ptr_s)) == "SomeStruct*");
  REQUIRE(NAMEOF_TYPE(decltype(ref_s)) == "SomeStruct&");
  REQUIRE(NAMEOF_TYPE(SomeStruct) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE(SomeStruct *) == "SomeStruct*");
  REQUIRE(NAMEOF_TYPE(SomeStruct &) == "SomeStruct&");
  REQUIRE(NAMEOF_TYPE(const SomeStruct volatile *) == "const volatile SomeStruct*");

  REQUIRE(NAMEOF_TYPE(SomeClass<int>) == "SomeClass<int>");
  REQUIRE(NAMEOF_TYPE(const SomeClass<int> volatile *) == "const volatile SomeClass<int>*");

  REQUIRE(NAMEOF_TYPE(decltype(othervar)) == "Long");
  REQUIRE(NAMEOF_TYPE(Long) == "Long");
  REQUIRE(NAMEOF_TYPE(Long::LL) == "Long::LL");

  REQUIRE(NAMEOF_TYPE(Color) == "Color");
#endif
}

TEST_CASE("nameof::nameof_type"){
#if defined(__clang__)
  REQUIRE(nameof::nameof_type<decltype(struct_var)>() == "SomeStruct");
  REQUIRE(nameof::nameof_type<decltype(ptr_s)>() == "SomeStruct *");
  REQUIRE(nameof::nameof_type<decltype(ref_s)>() == "SomeStruct &");
  REQUIRE(nameof::nameof_type<SomeStruct>() == "SomeStruct");
  REQUIRE(nameof::nameof_type<SomeStruct *>() == "SomeStruct *");
  REQUIRE(nameof::nameof_type<SomeStruct &>() == "SomeStruct &");
  REQUIRE(nameof::nameof_type<const SomeStruct volatile *>() == "const volatile SomeStruct *");

  REQUIRE(nameof::nameof_type<SomeClass<int>>() == "SomeClass<int>");
  REQUIRE(nameof::nameof_type<const SomeClass<int> volatile *>() == "const volatile SomeClass<int> *");

  REQUIRE(nameof::nameof_type<decltype(othervar)>() == "Long");
  REQUIRE(nameof::nameof_type<Long>() == "Long");
  REQUIRE(nameof::nameof_type<Long::LL>() == "Long::LL");

  REQUIRE(nameof::nameof_type<Color>() == "Color");
#elif defined(_MSC_VER)
  REQUIRE(nameof::nameof_type<decltype(struct_var)>() == "struct SomeStruct");
  REQUIRE(nameof::nameof_type<decltype(ptr_s)>() == "struct SomeStruct *");
  REQUIRE(nameof::nameof_type<decltype(ref_s)>() == "struct SomeStruct &");
  REQUIRE(nameof::nameof_type<SomeStruct>() == "struct SomeStruct");
  REQUIRE(nameof::nameof_type<SomeStruct *>() == "struct SomeStruct *");
  REQUIRE(nameof::nameof_type<SomeStruct &>() == "struct SomeStruct &");
  REQUIRE(nameof::nameof_type<const SomeStruct volatile *>() == "struct SomeStruct const volatile *");

  REQUIRE(nameof::nameof_type<SomeClass<int>>() == "class SomeClass<int>");
  REQUIRE(nameof::nameof_type<const SomeClass<int> volatile *>() == "class SomeClass<int> const volatile *");

  REQUIRE(nameof::nameof_type<decltype(othervar)>() == "struct Long");
  REQUIRE(nameof::nameof_type<Long>() == "struct Long");
  REQUIRE(nameof::nameof_type<Long::LL>() == "struct Long::LL");

  REQUIRE(nameof::nameof_type<Color>() == "enum Color");
#elif defined(__GNUC__)
  REQUIRE(nameof::nameof_type<decltype(struct_var)>() == "SomeStruct");
  REQUIRE(nameof::nameof_type<decltype(ptr_s)>() == "SomeStruct*");
  REQUIRE(nameof::nameof_type<decltype(ref_s)>() == "SomeStruct&");
  REQUIRE(nameof::nameof_type<SomeStruct>() == "SomeStruct");
  REQUIRE(nameof::nameof_type<SomeStruct *>() == "SomeStruct*");
  REQUIRE(nameof::nameof_type<SomeStruct &>() == "SomeStruct&");
  REQUIRE(nameof::nameof_type<const SomeStruct volatile *>() == "const volatile SomeStruct*");

  REQUIRE(nameof::nameof_type<SomeClass<int>>() == "SomeClass<int>");
  REQUIRE(nameof::nameof_type<const SomeClass<int> volatile *>() == "const volatile SomeClass<int>*");

  REQUIRE(nameof::nameof_type<decltype(othervar)>() == "Long");
  REQUIRE(nameof::nameof_type<Long>() == "Long");
  REQUIRE(nameof::nameof_type<Long::LL>() == "Long::LL");

  REQUIRE(nameof::nameof_type<Color>() == "Color");
#endif
}
