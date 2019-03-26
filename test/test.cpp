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
    std::cout << NAMEOF_TYPE_T(T) << std::endl;
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

enum class Color { RED = -1, GREEN, BLUE };

enum Directions { Up, Down, Right, Left };

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
    REQUIRE(NAMEOF(struct_var.somefield++) == "somefield");
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
    REQUIRE(NAMEOF_FULL(struct_var.somefield++) == "somefield");
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

  SECTION("type") {
    REQUIRE(NAMEOF_RAW(const SomeStruct volatile *) == "const SomeStruct volatile *");
    REQUIRE(NAMEOF_RAW(SomeClass<int>) == "SomeClass<int>");
    REQUIRE(NAMEOF_RAW(decltype(othervar)) == "decltype(othervar)");
  }
}

TEST_CASE("NAMEOF_ENUM") {
  Color color_ = Color::BLUE;
  Color m[3] = {Color::RED, Color::GREEN, Color::BLUE};
#if defined(__clang__) || (defined(__GNUC__) && __GNUC__ >= 9) || defined(_MSC_VER)
  REQUIRE(NAMEOF_ENUM(Color::RED) == "RED");
  REQUIRE(NAMEOF_ENUM(color) == "RED");

  REQUIRE(NAMEOF_ENUM(Color::BLUE) == "BLUE");
  REQUIRE(NAMEOF_ENUM(color_) == "BLUE");

  REQUIRE(NAMEOF_ENUM(m[1]) == "GREEN");

  REQUIRE(NAMEOF_ENUM(Directions::Right) == "Right");
  REQUIRE(NAMEOF_ENUM(directions) == "Right");
#elif defined(__GNUC__)
  REQUIRE(NAMEOF_ENUM(Color::RED) == "(Color)-1");
  REQUIRE(NAMEOF_ENUM(color) == "(Color)-1");

  REQUIRE(NAMEOF_ENUM(Color::BLUE) == "(Color)1");
  REQUIRE(NAMEOF_ENUM(color_) == "(Color)1");

  REQUIRE(NAMEOF_ENUM(m[1]) == "(Color)0");

  REQUIRE(NAMEOF_ENUM(Directions::Right) == "(Directions)2");
  REQUIRE(NAMEOF_ENUM(directions) == "(Directions)2");
#endif
}

TEST_CASE("nameof::nameof_enum<T>(value)") {
  Color color_ = Color::BLUE;
  Color m[3] = {Color::RED, Color::GREEN, Color::BLUE};
#if defined(__clang__) || (defined(__GNUC__) && __GNUC__ >= 9) || defined(_MSC_VER)
  REQUIRE(nameof::nameof_enum(Color::RED) == "RED");
  REQUIRE(nameof::nameof_enum(color) == "RED");

  REQUIRE(nameof::nameof_enum(Color::BLUE) == "BLUE");
  REQUIRE(nameof::nameof_enum(color_) == "BLUE");

  REQUIRE(nameof::nameof_enum(m[1]) == "GREEN");

  REQUIRE(nameof::nameof_enum(Directions::Right) == "Right");
  REQUIRE(nameof::nameof_enum(directions) == "Right");
#elif defined(__GNUC__)
  REQUIRE(nameof::nameof_enum(Color::RED) == "(Color)-1");
  REQUIRE(nameof::nameof_enum(color) == "(Color)-1");

  REQUIRE(nameof::nameof_enum(Color::BLUE) == "(Color)1");
  REQUIRE(nameof::nameof_enum(color_) == "(Color)1");

  REQUIRE(nameof::nameof_enum(m[1]) == "(Color)0");

  REQUIRE(nameof::nameof_enum(Directions::Right) == "(Directions)2");
  REQUIRE(nameof::nameof_enum(directions) == "(Directions)2");
#endif
}

TEST_CASE("nameof::nameof_enum<value>()") {
  constexpr Color color_ = Color::BLUE;
  constexpr Color m[3] = {Color::RED, Color::GREEN, Color::BLUE};
#if defined(__clang__) || (defined(__GNUC__) && __GNUC__ >= 9) || defined(_MSC_VER)
  REQUIRE(nameof::nameof_enum<Color::RED>() == "RED");
  REQUIRE(nameof::nameof_enum<color>() == "RED");

  REQUIRE(nameof::nameof_enum<Color::BLUE>() == "BLUE");
  REQUIRE(nameof::nameof_enum<color_>() == "BLUE");

  REQUIRE(nameof::nameof_enum<m[1]>() == "GREEN");

  REQUIRE(nameof::nameof_enum<Directions::Right>() == "Right");
  REQUIRE(nameof::nameof_enum<directions>() == "Right");
#elif defined(__GNUC__)
  REQUIRE(nameof::nameof_enum<Color::RED>() == "(Color)-1");
  REQUIRE(nameof::nameof_enum<color>() == "(Color)-1");

  REQUIRE(nameof::nameof_enum<Color::BLUE>() == "(Color)1");
  REQUIRE(nameof::nameof_enum<color_>() == "(Color)1");

  REQUIRE(nameof::nameof_enum<m[1]>() == "(Color)0");

  REQUIRE(nameof::nameof_enum<Directions::Right>() == "(Directions)2");
  REQUIRE(nameof::nameof_enum<directions>() == "(Directions)2");
#endif
}

TEST_CASE("NAMEOF_TYPE") {
#if defined(__clang__)
  REQUIRE(NAMEOF_TYPE(struct_var) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE(ptr_s) == "SomeStruct *");
  REQUIRE(NAMEOF_TYPE(ref_s) == "SomeStruct &");

  REQUIRE(NAMEOF_TYPE(ptr_c) == "const volatile SomeClass<int> *");

  REQUIRE(NAMEOF_TYPE(othervar) == "Long");
  REQUIRE(NAMEOF_TYPE(othervar.ll) == "Long::LL");
  REQUIRE(NAMEOF_TYPE(othervar.ll.field) == "int");

  REQUIRE(NAMEOF_TYPE(Color::RED) == "Color");

  REQUIRE(NAMEOF_TYPE(std::declval<const SomeClass<int>>()) == "const SomeClass<int> &&");
#elif defined(_MSC_VER)
  REQUIRE(NAMEOF_TYPE(struct_var) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE(ptr_s) == "SomeStruct *");
  REQUIRE(NAMEOF_TYPE(ref_s) == "SomeStruct &");

  REQUIRE(NAMEOF_TYPE(ptr_c) == "SomeClass<int> const volatile *");

  REQUIRE(NAMEOF_TYPE(othervar) == "Long");
  REQUIRE(NAMEOF_TYPE(othervar.ll) == "Long::LL");
  REQUIRE(NAMEOF_TYPE(othervar.ll.field) == "int");

  REQUIRE(NAMEOF_TYPE(Color::RED) == "Color");

  REQUIRE(NAMEOF_TYPE(std::declval<const SomeClass<int>>()) == "SomeClass<int> const &&");
#elif defined(__GNUC__)
  REQUIRE(NAMEOF_TYPE(struct_var) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE(ptr_s) == "SomeStruct*");
  REQUIRE(NAMEOF_TYPE(ref_s) == "SomeStruct&");

  REQUIRE(NAMEOF_TYPE(ptr_c) == "const volatile SomeClass<int>*");

  REQUIRE(NAMEOF_TYPE(othervar) == "Long");
  REQUIRE(NAMEOF_TYPE(othervar.ll) == "Long::LL");
  REQUIRE(NAMEOF_TYPE(othervar.ll.field) == "int");

  REQUIRE(NAMEOF_TYPE(Color::RED) == "Color");

  REQUIRE(NAMEOF_TYPE(std::declval<const SomeClass<int>>()) == "const SomeClass<int>&&");
#endif
}

TEST_CASE("NAMEOF_TYPE_T") {
#if defined(__clang__)
  REQUIRE(NAMEOF_TYPE_T(decltype(struct_var)) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_T(decltype(ptr_s)) == "SomeStruct *");
  REQUIRE(NAMEOF_TYPE_T(decltype(ref_s)) == "SomeStruct &");
  REQUIRE(NAMEOF_TYPE_T(SomeStruct) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_T(SomeStruct *) == "SomeStruct *");
  REQUIRE(NAMEOF_TYPE_T(SomeStruct &) == "SomeStruct &");
  REQUIRE(NAMEOF_TYPE_T(const SomeStruct volatile *) == "const volatile SomeStruct *");

  REQUIRE(NAMEOF_TYPE_T(SomeClass<int>) == "SomeClass<int>");
  REQUIRE(NAMEOF_TYPE_T(const SomeClass<int> volatile *) == "const volatile SomeClass<int> *");

  REQUIRE(NAMEOF_TYPE_T(decltype(othervar)) == "Long");
  REQUIRE(NAMEOF_TYPE_T(Long) == "Long");
  REQUIRE(NAMEOF_TYPE_T(Long::LL) == "Long::LL");

  REQUIRE(NAMEOF_TYPE_T(Color) == "Color");
#elif defined(_MSC_VER)
  REQUIRE(NAMEOF_TYPE_T(decltype(struct_var)) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_T(decltype(ptr_s)) == "SomeStruct *");
  REQUIRE(NAMEOF_TYPE_T(decltype(ref_s)) == "SomeStruct &");
  REQUIRE(NAMEOF_TYPE_T(SomeStruct) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_T(SomeStruct *) == "SomeStruct *");
  REQUIRE(NAMEOF_TYPE_T(SomeStruct &) == "SomeStruct &");
  REQUIRE(NAMEOF_TYPE_T(const SomeStruct volatile *) == "SomeStruct const volatile *");

  REQUIRE(NAMEOF_TYPE_T(SomeClass<int>) == "SomeClass<int>");
  REQUIRE(NAMEOF_TYPE_T(const SomeClass<int> volatile *) == "SomeClass<int> const volatile *");

  REQUIRE(NAMEOF_TYPE_T(decltype(othervar)) == "Long");
  REQUIRE(NAMEOF_TYPE_T(Long) == "Long");
  REQUIRE(NAMEOF_TYPE_T(Long::LL) == "Long::LL");

  REQUIRE(NAMEOF_TYPE_T(Color) == "Color");
#elif defined(__GNUC__)
  REQUIRE(NAMEOF_TYPE_T(decltype(struct_var)) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_T(decltype(ptr_s)) == "SomeStruct*");
  REQUIRE(NAMEOF_TYPE_T(decltype(ref_s)) == "SomeStruct&");
  REQUIRE(NAMEOF_TYPE_T(SomeStruct) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_T(SomeStruct *) == "SomeStruct*");
  REQUIRE(NAMEOF_TYPE_T(SomeStruct &) == "SomeStruct&");
  REQUIRE(NAMEOF_TYPE_T(const SomeStruct volatile *) == "const volatile SomeStruct*");

  REQUIRE(NAMEOF_TYPE_T(SomeClass<int>) == "SomeClass<int>");
  REQUIRE(NAMEOF_TYPE_T(const SomeClass<int> volatile *) == "const volatile SomeClass<int>*");

  REQUIRE(NAMEOF_TYPE_T(decltype(othervar)) == "Long");
  REQUIRE(NAMEOF_TYPE_T(Long) == "Long");
  REQUIRE(NAMEOF_TYPE_T(Long::LL) == "Long::LL");

  REQUIRE(NAMEOF_TYPE_T(Color) == "Color");
#endif
}

TEST_CASE("nameof::nameof_type<T>()"){
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
  REQUIRE(nameof::nameof_type<decltype(struct_var)>() == "SomeStruct");
  REQUIRE(nameof::nameof_type<decltype(ptr_s)>() == "SomeStruct *");
  REQUIRE(nameof::nameof_type<decltype(ref_s)>() == "SomeStruct &");
  REQUIRE(nameof::nameof_type<SomeStruct>() == "SomeStruct");
  REQUIRE(nameof::nameof_type<SomeStruct *>() == "SomeStruct *");
  REQUIRE(nameof::nameof_type<SomeStruct &>() == "SomeStruct &");
  REQUIRE(nameof::nameof_type<const SomeStruct volatile *>() == "SomeStruct const volatile *");

  REQUIRE(nameof::nameof_type<SomeClass<int>>() == "SomeClass<int>");
  REQUIRE(nameof::nameof_type<const SomeClass<int> volatile *>() == "SomeClass<int> const volatile *");

  REQUIRE(nameof::nameof_type<decltype(othervar)>() == "Long");
  REQUIRE(nameof::nameof_type<Long>() == "Long");
  REQUIRE(nameof::nameof_type<Long::LL>() == "Long::LL");

  REQUIRE(nameof::nameof_type<Color>() == "Color");
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

TEST_CASE("Spaces and Tabs ignored") {
  SECTION("Spaces") {
    REQUIRE(NAMEOF(   struct_var   ) == "struct_var");
    REQUIRE(NAMEOF_FULL(   struct_var   ) == "struct_var");
    REQUIRE(NAMEOF_RAW(   struct_var   ) == "struct_var");
#if defined(__clang__) || (defined(__GNUC__) && __GNUC__ >= 9) || defined(_MSC_VER)
    REQUIRE(NAMEOF_ENUM(   color   ) == "RED");
#elif defined(__GNUC__)
    REQUIRE(NAMEOF_ENUM(   color   ) == "(Color)-1");
#endif
    REQUIRE(NAMEOF_TYPE(   struct_var   ) == "SomeStruct");
    REQUIRE(NAMEOF_TYPE_T(   decltype(struct_var)   ) == "SomeStruct");
  }

  SECTION("Tabs") {
    REQUIRE(NAMEOF(	struct_var	) == "struct_var");
    REQUIRE(NAMEOF_FULL(	struct_var	) == "struct_var");
    REQUIRE(NAMEOF_RAW(	struct_var	) == "struct_var");
#if defined(__clang__) || (defined(__GNUC__) && __GNUC__ >= 9) || defined(_MSC_VER)
    REQUIRE(NAMEOF_ENUM(	color	) == "RED");
#elif defined(__GNUC__)
    REQUIRE(NAMEOF_ENUM(	color	) == "(Color)-1");
#endif
    REQUIRE(NAMEOF_TYPE(	struct_var	) == "SomeStruct");
    REQUIRE(NAMEOF_TYPE_T(	decltype(struct_var)	) == "SomeStruct");
  }
}
