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
#include <catch2/catch.hpp>

#define NAMEOF_ENUM_RANGE_MIN -120
#define NAMEOF_ENUM_RANGE_MAX 120
#include <nameof.hpp>

#include <string>
#include <stdexcept>

struct SomeStruct {
  int somefield = 0;

  void SomeMethod1(int) {
    throw std::runtime_error{"should not be called!"};
   }

  int SomeMethod2() const {
    throw std::runtime_error{"should not be called!"};
  }
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
    throw std::runtime_error{"should not be called!"};
  }

  template <typename C>
  C SomeMethod6() const {
    throw std::runtime_error{"should not be called!"};
  }
};

struct Long {
  struct LL {
    int field = 0;
  };
  LL ll;
};

enum class Color { RED = -12, GREEN = 7, BLUE = 15 };

enum class Numbers : char { one = 10, two = 20, three = 30, many = 127 };

enum Directions { Up = 85, Down = -42, Right = 120, Left = -120 };

enum number : unsigned long { one = 100, two = 200, three = 300, four = 400 };

namespace nameof {
template <>
struct enum_range<number> {
  static_assert(std::is_enum_v<number>, "nameof::enum_range<number> requires enum type.");
  static constexpr int min = 100;
  static constexpr int max = 300;
  static_assert(max > min, "nameof::enum_range<number> requires max > min.");
};
}

SomeStruct struct_var;
Long othervar;
SomeStruct * ptr_s = &struct_var;
SomeStruct & ref_s = struct_var;

SomeClass<int> class_var;
const SomeClass<int> volatile * ptr_c = nullptr;

const Color color = Color::RED;

TEST_CASE("NAMEOF") {
  SECTION("variable") {
    constexpr auto name = NAMEOF(othervar);
    REQUIRE(name == "othervar");
    REQUIRE(NAMEOF(struct_var) == "struct_var");
    REQUIRE(NAMEOF(::struct_var) == "struct_var");
    REQUIRE(NAMEOF(ptr_s) == "ptr_s");
    REQUIRE(NAMEOF(color) == "color");
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
    constexpr auto full_name = NAMEOF_FULL(othervar);
    REQUIRE(full_name == "othervar");
    REQUIRE(NAMEOF_FULL(struct_var) == "struct_var");
    REQUIRE(NAMEOF_FULL(::struct_var) == "struct_var");
    REQUIRE(NAMEOF_FULL(ptr_s) == "ptr_s");
    REQUIRE(NAMEOF_FULL(color) == "color");
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
    constexpr auto raw_name = NAMEOF_RAW(othervar);
    REQUIRE(raw_name == "othervar");
    REQUIRE(NAMEOF_RAW(struct_var) == "struct_var");
    REQUIRE(NAMEOF_RAW(&struct_var) == "&struct_var");
    REQUIRE(NAMEOF_RAW(::struct_var) == "::struct_var");
    REQUIRE(NAMEOF_RAW(ptr_s) == "ptr_s");
    REQUIRE(NAMEOF_RAW(*ptr_s) == "*ptr_s");
    REQUIRE(NAMEOF_RAW(ptr_s[0]) == "ptr_s[0]");
    REQUIRE(NAMEOF_RAW(color) == "color");
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

#if defined(__clang__) || (defined(__GNUC__) && __GNUC__ >= 9) || defined(_MSC_VER)
TEST_CASE("NAMEOF_ENUM") {
  constexpr Color cr = Color::RED;
  constexpr auto cr_name = NAMEOF_ENUM(cr);
  Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
  REQUIRE(cr_name == "RED");
  REQUIRE(NAMEOF_ENUM(Color::BLUE) == "BLUE");
  REQUIRE(NAMEOF_ENUM(cm[1]) == "GREEN");
  REQUIRE(NAMEOF_ENUM(static_cast<Color>(0)).empty());

  constexpr Numbers no = Numbers::one;
  constexpr auto no_name = NAMEOF_ENUM(no);
  REQUIRE(no_name == "one");
  REQUIRE(NAMEOF_ENUM(Numbers::two) == "two");
  REQUIRE(NAMEOF_ENUM(Numbers::three) == "three");
  REQUIRE(NAMEOF_ENUM(Numbers::many).empty());
  REQUIRE(NAMEOF_ENUM(static_cast<Numbers>(0)).empty());

  constexpr Directions dr = Directions::Right;
  constexpr auto dr_name = NAMEOF_ENUM(dr);
  REQUIRE(NAMEOF_ENUM(Directions::Up) == "Up");
  REQUIRE(NAMEOF_ENUM(Directions::Down) == "Down");
  REQUIRE(dr_name == "Right");
  REQUIRE(NAMEOF_ENUM(Directions::Left) == "Left");
  REQUIRE(NAMEOF_ENUM(static_cast<Directions>(0)).empty());

  constexpr number nt = number::three;
  constexpr auto nt_name = NAMEOF_ENUM(nt);
  REQUIRE(NAMEOF_ENUM(number::one) == "one");
  REQUIRE(NAMEOF_ENUM(number::two) == "two");
  REQUIRE(nt_name == "three");
  REQUIRE(NAMEOF_ENUM(number::four).empty());
  REQUIRE(NAMEOF_ENUM(static_cast<number>(0)).empty());
}

TEST_CASE("NAMEOF_CONST_ENUM") {
  constexpr Color cr = Color::RED;
  constexpr auto cr_name = NAMEOF_CONST_ENUM(cr);
  constexpr Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
  REQUIRE(cr_name == "RED");
  REQUIRE(NAMEOF_CONST_ENUM(Color::BLUE) == "BLUE");
  REQUIRE(NAMEOF_CONST_ENUM(cm[1]) == "GREEN");
  REQUIRE(NAMEOF_CONST_ENUM(static_cast<Color>(0)).empty());

  constexpr Numbers no = Numbers::one;
  constexpr auto no_name = NAMEOF_CONST_ENUM(no);
  REQUIRE(no_name == "one");
  REQUIRE(NAMEOF_CONST_ENUM(Numbers::two) == "two");
  REQUIRE(NAMEOF_CONST_ENUM(Numbers::three) == "three");
  REQUIRE(NAMEOF_CONST_ENUM(Numbers::many) == "many");
  REQUIRE(NAMEOF_CONST_ENUM(static_cast<Numbers>(0)).empty());

  constexpr Directions dr = Directions::Right;
  constexpr auto dr_name = NAMEOF_CONST_ENUM(dr);
  REQUIRE(NAMEOF_CONST_ENUM(Directions::Up) == "Up");
  REQUIRE(NAMEOF_CONST_ENUM(Directions::Down) == "Down");
  REQUIRE(dr_name == "Right");
  REQUIRE(NAMEOF_CONST_ENUM(Directions::Left) == "Left");
  REQUIRE(NAMEOF_CONST_ENUM(static_cast<Directions>(0)).empty());

  constexpr number nt = number::three;
  constexpr auto nt_name = NAMEOF_CONST_ENUM(nt);
  REQUIRE(NAMEOF_CONST_ENUM(number::one) == "one");
  REQUIRE(NAMEOF_CONST_ENUM(number::two) == "two");
  REQUIRE(nt_name == "three");
  REQUIRE(NAMEOF_CONST_ENUM(number::four) == "four");
  REQUIRE(NAMEOF_CONST_ENUM(static_cast<number>(0)).empty());
}

TEST_CASE("nameof_enum") {
  SECTION("automatic storage") {
    constexpr Color cr = Color::RED;
    constexpr auto cr_name = nameof::nameof_enum(cr);
    Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
    REQUIRE(cr_name == "RED");
    REQUIRE(nameof::nameof_enum(Color::BLUE) == "BLUE");
    REQUIRE(nameof::nameof_enum(cm[1]) == "GREEN");
    REQUIRE(nameof::nameof_enum(static_cast<Color>(0)).empty());

    constexpr Numbers no = Numbers::one;
    constexpr auto no_name = nameof::nameof_enum(no);
    REQUIRE(no_name == "one");
    REQUIRE(nameof::nameof_enum(Numbers::two) == "two");
    REQUIRE(nameof::nameof_enum(Numbers::three) == "three");
    REQUIRE(nameof::nameof_enum(Numbers::many).empty());
    REQUIRE(nameof::nameof_enum(static_cast<Numbers>(0)).empty());

    constexpr Directions dr = Directions::Right;
    constexpr auto dr_name = nameof::nameof_enum(dr);
    REQUIRE(nameof::nameof_enum(Directions::Up) == "Up");
    REQUIRE(nameof::nameof_enum(Directions::Down) == "Down");
    REQUIRE(dr_name == "Right");
    REQUIRE(nameof::nameof_enum(Directions::Left) == "Left");
    REQUIRE(nameof::nameof_enum(static_cast<Directions>(0)).empty());

    constexpr number nt = number::three;
    constexpr auto nt_name = nameof::nameof_enum(nt);
    REQUIRE(nameof::nameof_enum(number::one) == "one");
    REQUIRE(nameof::nameof_enum(number::two) == "two");
    REQUIRE(nt_name == "three");
    REQUIRE(nameof::nameof_enum(number::four).empty());
    REQUIRE(nameof::nameof_enum(static_cast<number>(0)).empty());
  }

  SECTION("static storage") {
    constexpr Color cr = Color::RED;
    constexpr auto cr_name = nameof::nameof_enum<cr>();
    constexpr Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
    REQUIRE(cr_name == "RED");
    REQUIRE(nameof::nameof_enum<Color::BLUE>() == "BLUE");
    REQUIRE(nameof::nameof_enum<cm[1]>() == "GREEN");
    REQUIRE(nameof::nameof_enum<static_cast<Color>(0)>().empty());

    constexpr Numbers no = Numbers::one;
    constexpr auto no_name = nameof::nameof_enum<no>();
    REQUIRE(no_name == "one");
    REQUIRE(nameof::nameof_enum<Numbers::two>() == "two");
    REQUIRE(nameof::nameof_enum<Numbers::three>() == "three");
    REQUIRE(nameof::nameof_enum<Numbers::many>() == "many");
    REQUIRE(nameof::nameof_enum<static_cast<Numbers>(0)>().empty());

    constexpr Directions dr = Directions::Right;
    constexpr auto dr_name = nameof::nameof_enum<dr>();
    REQUIRE(nameof::nameof_enum<Directions::Up>() == "Up");
    REQUIRE(nameof::nameof_enum<Directions::Down>() == "Down");
    REQUIRE(dr_name == "Right");
    REQUIRE(nameof::nameof_enum<Directions::Left>() == "Left");
    REQUIRE(nameof::nameof_enum<static_cast<Directions>(0)>().empty());

    constexpr number nt = number::three;
    constexpr auto nt_name = nameof::nameof_enum<nt>();
    REQUIRE(nameof::nameof_enum<number::one>() == "one");
    REQUIRE(nameof::nameof_enum<number::two>() == "two");
    REQUIRE(nt_name == "three");
    REQUIRE(nameof::nameof_enum<number::four>() == "four");
    REQUIRE(nameof::nameof_enum<static_cast<number>(0)>().empty());
  }
}
#endif

TEST_CASE("NAMEOF_FULL_TYPE_EXPR") {
  constexpr auto type_name = NAMEOF_FULL_TYPE_EXPR(struct_var);
#if defined(__clang__)
  REQUIRE(type_name == "SomeStruct");
  REQUIRE(NAMEOF_FULL_TYPE_EXPR(ptr_s) == "SomeStruct *");
  REQUIRE(NAMEOF_FULL_TYPE_EXPR(ref_s) == "SomeStruct &");

  REQUIRE(NAMEOF_FULL_TYPE_EXPR(ptr_c) == "const volatile SomeClass<int> *");

  REQUIRE(NAMEOF_FULL_TYPE_EXPR(othervar) == "Long");
  REQUIRE(NAMEOF_FULL_TYPE_EXPR(othervar.ll) == "Long::LL");
  REQUIRE(NAMEOF_FULL_TYPE_EXPR(othervar.ll.field) == "int");

  REQUIRE(NAMEOF_FULL_TYPE_EXPR(Color::RED) == "Color");

  REQUIRE(NAMEOF_FULL_TYPE_EXPR(std::declval<const SomeClass<int>>()) == "const SomeClass<int> &&");
#elif defined(_MSC_VER)
  REQUIRE(type_name == "struct SomeStruct");
  REQUIRE(NAMEOF_FULL_TYPE_EXPR(ptr_s) == "struct SomeStruct *");
  REQUIRE(NAMEOF_FULL_TYPE_EXPR(ref_s) == "struct SomeStruct &");

  REQUIRE(NAMEOF_FULL_TYPE_EXPR(ptr_c) == "class SomeClass<int> const volatile *");

  REQUIRE(NAMEOF_FULL_TYPE_EXPR(othervar) == "struct Long");
  REQUIRE(NAMEOF_FULL_TYPE_EXPR(othervar.ll) == "struct Long::LL");
  REQUIRE(NAMEOF_FULL_TYPE_EXPR(othervar.ll.field) == "int");

  REQUIRE(NAMEOF_FULL_TYPE_EXPR(Color::RED) == "enum Color");

  REQUIRE(NAMEOF_FULL_TYPE_EXPR(std::declval<const SomeClass<int>>()) == "class SomeClass<int> const &&");
#elif defined(__GNUC__)
  REQUIRE(type_name == "SomeStruct");
  REQUIRE(NAMEOF_FULL_TYPE_EXPR(ptr_s) == "SomeStruct*");
  REQUIRE(NAMEOF_FULL_TYPE_EXPR(ref_s) == "SomeStruct&");

  REQUIRE(NAMEOF_FULL_TYPE_EXPR(ptr_c) == "const volatile SomeClass<int>*");

  REQUIRE(NAMEOF_FULL_TYPE_EXPR(othervar) == "Long");
  REQUIRE(NAMEOF_FULL_TYPE_EXPR(othervar.ll) == "Long::LL");
  REQUIRE(NAMEOF_FULL_TYPE_EXPR(othervar.ll.field) == "int");

  REQUIRE(NAMEOF_FULL_TYPE_EXPR(Color::RED) == "Color");

  REQUIRE(NAMEOF_FULL_TYPE_EXPR(std::declval<const SomeClass<int>>()) == "const SomeClass<int>&&");
#endif
}

TEST_CASE("NAMEOF_FULL_TYPE") {
  constexpr auto type_name = NAMEOF_FULL_TYPE(decltype(struct_var));
#if defined(__clang__)
  REQUIRE(type_name == "SomeStruct");
  REQUIRE(NAMEOF_FULL_TYPE(decltype(ptr_s)) == "SomeStruct *");
  REQUIRE(NAMEOF_FULL_TYPE(decltype(ref_s)) == "SomeStruct &");
  REQUIRE(NAMEOF_FULL_TYPE(SomeStruct) == "SomeStruct");
  REQUIRE(NAMEOF_FULL_TYPE(SomeStruct *) == "SomeStruct *");
  REQUIRE(NAMEOF_FULL_TYPE(SomeStruct &) == "SomeStruct &");
  REQUIRE(NAMEOF_FULL_TYPE(const SomeStruct volatile *) == "const volatile SomeStruct *");

  REQUIRE(NAMEOF_FULL_TYPE(SomeClass<int>) == "SomeClass<int>");
  REQUIRE(NAMEOF_FULL_TYPE(const SomeClass<int> volatile *) == "const volatile SomeClass<int> *");

  REQUIRE(NAMEOF_FULL_TYPE(decltype(othervar)) == "Long");
  REQUIRE(NAMEOF_FULL_TYPE(Long) == "Long");
  REQUIRE(NAMEOF_FULL_TYPE(Long::LL) == "Long::LL");

  REQUIRE(NAMEOF_FULL_TYPE(Color) == "Color");
#elif defined(_MSC_VER)
  REQUIRE(type_name == "struct SomeStruct");
  REQUIRE(NAMEOF_FULL_TYPE(decltype(ptr_s)) == "struct SomeStruct *");
  REQUIRE(NAMEOF_FULL_TYPE(decltype(ref_s)) == "struct SomeStruct &");
  REQUIRE(NAMEOF_FULL_TYPE(SomeStruct) == "struct SomeStruct");
  REQUIRE(NAMEOF_FULL_TYPE(SomeStruct *) == "struct SomeStruct *");
  REQUIRE(NAMEOF_FULL_TYPE(SomeStruct &) == "struct SomeStruct &");
  REQUIRE(NAMEOF_FULL_TYPE(const SomeStruct volatile *) == "struct SomeStruct const volatile *");

  REQUIRE(NAMEOF_FULL_TYPE(SomeClass<int>) == "class SomeClass<int>");
  REQUIRE(NAMEOF_FULL_TYPE(const SomeClass<int> volatile *) == "class SomeClass<int> const volatile *");

  REQUIRE(NAMEOF_FULL_TYPE(decltype(othervar)) == "struct Long");
  REQUIRE(NAMEOF_FULL_TYPE(Long) == "struct Long");
  REQUIRE(NAMEOF_FULL_TYPE(Long::LL) == "struct Long::LL");

  REQUIRE(NAMEOF_FULL_TYPE(Color) == "enum Color");
#elif defined(__GNUC__)
  REQUIRE(type_name == "SomeStruct");
  REQUIRE(NAMEOF_FULL_TYPE(decltype(ptr_s)) == "SomeStruct*");
  REQUIRE(NAMEOF_FULL_TYPE(decltype(ref_s)) == "SomeStruct&");
  REQUIRE(NAMEOF_FULL_TYPE(SomeStruct) == "SomeStruct");
  REQUIRE(NAMEOF_FULL_TYPE(SomeStruct *) == "SomeStruct*");
  REQUIRE(NAMEOF_FULL_TYPE(SomeStruct &) == "SomeStruct&");
  REQUIRE(NAMEOF_FULL_TYPE(const SomeStruct volatile *) == "const volatile SomeStruct*");

  REQUIRE(NAMEOF_FULL_TYPE(SomeClass<int>) == "SomeClass<int>");
  REQUIRE(NAMEOF_FULL_TYPE(const SomeClass<int> volatile *) == "const volatile SomeClass<int>*");

  REQUIRE(NAMEOF_FULL_TYPE(decltype(othervar)) == "Long");
  REQUIRE(NAMEOF_FULL_TYPE(Long) == "Long");
  REQUIRE(NAMEOF_FULL_TYPE(Long::LL) == "Long::LL");

  REQUIRE(NAMEOF_FULL_TYPE(Color) == "Color");
#endif
}

TEST_CASE("nameof::nameof_full_type") {
  constexpr auto type_name = nameof::nameof_full_type<decltype(struct_var)>();
#if defined(__clang__)
  REQUIRE(type_name == "SomeStruct");
  REQUIRE(nameof::nameof_full_type<decltype(ptr_s)>() == "SomeStruct *");
  REQUIRE(nameof::nameof_full_type<decltype(ref_s)>() == "SomeStruct &");
  REQUIRE(nameof::nameof_full_type<SomeStruct>() == "SomeStruct");
  REQUIRE(nameof::nameof_full_type<SomeStruct *>() == "SomeStruct *");
  REQUIRE(nameof::nameof_full_type<SomeStruct &>() == "SomeStruct &");
  REQUIRE(nameof::nameof_full_type<const SomeStruct volatile *>() == "const volatile SomeStruct *");

  REQUIRE(nameof::nameof_full_type<SomeClass<int>>() == "SomeClass<int>");
  REQUIRE(nameof::nameof_full_type<const SomeClass<int> volatile *>() == "const volatile SomeClass<int> *");

  REQUIRE(nameof::nameof_full_type<decltype(othervar)>() == "Long");
  REQUIRE(nameof::nameof_full_type<Long>() == "Long");
  REQUIRE(nameof::nameof_full_type<Long::LL>() == "Long::LL");

  REQUIRE(nameof::nameof_full_type<Color>() == "Color");
#elif defined(_MSC_VER)
  REQUIRE(type_name == "struct SomeStruct");
  REQUIRE(nameof::nameof_full_type<decltype(ptr_s)>() == "struct SomeStruct *");
  REQUIRE(nameof::nameof_full_type<decltype(ref_s)>() == "struct SomeStruct &");
  REQUIRE(nameof::nameof_full_type<SomeStruct>() == "struct SomeStruct");
  REQUIRE(nameof::nameof_full_type<SomeStruct *>() == "struct SomeStruct *");
  REQUIRE(nameof::nameof_full_type<SomeStruct &>() == "struct SomeStruct &");
  REQUIRE(nameof::nameof_full_type<const SomeStruct volatile *>() == "struct SomeStruct const volatile *");

  REQUIRE(nameof::nameof_full_type<SomeClass<int>>() == "class SomeClass<int>");
  REQUIRE(nameof::nameof_full_type<const SomeClass<int> volatile *>() == "class SomeClass<int> const volatile *");

  REQUIRE(nameof::nameof_full_type<decltype(othervar)>() == "struct Long");
  REQUIRE(nameof::nameof_full_type<Long>() == "struct Long");
  REQUIRE(nameof::nameof_full_type<Long::LL>() == "struct Long::LL");

  REQUIRE(nameof::nameof_full_type<Color>() == "enum Color");
#elif defined(__GNUC__)
  REQUIRE(type_name == "SomeStruct");
  REQUIRE(nameof::nameof_full_type<decltype(ptr_s)>() == "SomeStruct*");
  REQUIRE(nameof::nameof_full_type<decltype(ref_s)>() == "SomeStruct&");
  REQUIRE(nameof::nameof_full_type<SomeStruct>() == "SomeStruct");
  REQUIRE(nameof::nameof_full_type<SomeStruct *>() == "SomeStruct*");
  REQUIRE(nameof::nameof_full_type<SomeStruct &>() == "SomeStruct&");
  REQUIRE(nameof::nameof_full_type<const SomeStruct volatile *>() == "const volatile SomeStruct*");

  REQUIRE(nameof::nameof_full_type<SomeClass<int>>() == "SomeClass<int>");
  REQUIRE(nameof::nameof_full_type<const SomeClass<int> volatile *>() == "const volatile SomeClass<int>*");

  REQUIRE(nameof::nameof_full_type<decltype(othervar)>() == "Long");
  REQUIRE(nameof::nameof_full_type<Long>() == "Long");
  REQUIRE(nameof::nameof_full_type<Long::LL>() == "Long::LL");

  REQUIRE(nameof::nameof_full_type<Color>() == "Color");
#endif
}

TEST_CASE("NAMEOF_TYPE_EXPR") {
  constexpr auto type_name = NAMEOF_TYPE_EXPR(struct_var);
#if defined(__clang__)
  REQUIRE(type_name == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_EXPR(ptr_s) == "SomeStruct *");
  REQUIRE(NAMEOF_TYPE_EXPR(ref_s) == "SomeStruct");

  REQUIRE(NAMEOF_TYPE_EXPR(ptr_c) == "const volatile SomeClass<int> *");

  REQUIRE(NAMEOF_TYPE_EXPR(othervar) == "Long");
  REQUIRE(NAMEOF_TYPE_EXPR(othervar.ll) == "Long::LL");
  REQUIRE(NAMEOF_TYPE_EXPR(othervar.ll.field) == "int");

  REQUIRE(NAMEOF_TYPE_EXPR(Color::RED) == "Color");

  REQUIRE(NAMEOF_TYPE_EXPR(std::declval<const SomeClass<int>>()) == "SomeClass<int>");
#elif defined(_MSC_VER)
  REQUIRE(type_name == "struct SomeStruct");
  REQUIRE(NAMEOF_TYPE_EXPR(ptr_s) == "struct SomeStruct *");
  REQUIRE(NAMEOF_TYPE_EXPR(ref_s) == "struct SomeStruct");

  REQUIRE(NAMEOF_TYPE_EXPR(ptr_c) == "class SomeClass<int> const volatile *");

  REQUIRE(NAMEOF_TYPE_EXPR(othervar) == "struct Long");
  REQUIRE(NAMEOF_TYPE_EXPR(othervar.ll) == "struct Long::LL");
  REQUIRE(NAMEOF_TYPE_EXPR(othervar.ll.field) == "int");

  REQUIRE(NAMEOF_TYPE_EXPR(Color::RED) == "enum Color");

  REQUIRE(NAMEOF_TYPE_EXPR(std::declval<const SomeClass<int>>()) == "class SomeClass<int>");
#elif defined(__GNUC__)
  REQUIRE(type_name == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_EXPR(ptr_s) == "SomeStruct*");
  REQUIRE(NAMEOF_TYPE_EXPR(ref_s) == "SomeStruct");

  REQUIRE(NAMEOF_TYPE_EXPR(ptr_c) == "const volatile SomeClass<int>*");

  REQUIRE(NAMEOF_TYPE_EXPR(othervar) == "Long");
  REQUIRE(NAMEOF_TYPE_EXPR(othervar.ll) == "Long::LL");
  REQUIRE(NAMEOF_TYPE_EXPR(othervar.ll.field) == "int");

  REQUIRE(NAMEOF_TYPE_EXPR(Color::RED) == "Color");

  REQUIRE(NAMEOF_TYPE_EXPR(std::declval<const SomeClass<int>>()) == "SomeClass<int>");
#endif
}

TEST_CASE("NAMEOF_TYPE") {
  constexpr auto type_name = NAMEOF_TYPE(decltype(struct_var));
#if defined(__clang__)
  REQUIRE(type_name == "SomeStruct");
  REQUIRE(NAMEOF_TYPE(decltype(ptr_s)) == "SomeStruct *");
  REQUIRE(NAMEOF_TYPE(decltype(ref_s)) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE(SomeStruct) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE(SomeStruct *) == "SomeStruct *");
  REQUIRE(NAMEOF_TYPE(const SomeStruct &) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE(const SomeStruct volatile *) == "const volatile SomeStruct *");

  REQUIRE(NAMEOF_TYPE(SomeClass<int>) == "SomeClass<int>");
  REQUIRE(NAMEOF_TYPE(const SomeClass<int> volatile *) == "const volatile SomeClass<int> *");

  REQUIRE(NAMEOF_TYPE(decltype(othervar)) == "Long");
  REQUIRE(NAMEOF_TYPE(Long) == "Long");
  REQUIRE(NAMEOF_TYPE(Long::LL) == "Long::LL");

  REQUIRE(NAMEOF_TYPE(Color) == "Color");
#elif defined(_MSC_VER)
  REQUIRE(type_name == "struct SomeStruct");
  REQUIRE(NAMEOF_TYPE(decltype(ptr_s)) == "struct SomeStruct *");
  REQUIRE(NAMEOF_TYPE(decltype(ref_s)) == "struct SomeStruct");
  REQUIRE(NAMEOF_TYPE(SomeStruct) == "struct SomeStruct");
  REQUIRE(NAMEOF_TYPE(SomeStruct *) == "struct SomeStruct *");
  REQUIRE(NAMEOF_TYPE(const SomeStruct &) == "struct SomeStruct");
  REQUIRE(NAMEOF_TYPE(const SomeStruct volatile *) == "struct SomeStruct const volatile *");

  REQUIRE(NAMEOF_TYPE(SomeClass<int>) == "class SomeClass<int>");
  REQUIRE(NAMEOF_TYPE(const SomeClass<int> volatile *) == "class SomeClass<int> const volatile *");

  REQUIRE(NAMEOF_TYPE(decltype(othervar)) == "struct Long");
  REQUIRE(NAMEOF_TYPE(Long) == "struct Long");
  REQUIRE(NAMEOF_TYPE(Long::LL) == "struct Long::LL");

  REQUIRE(NAMEOF_TYPE(Color) == "enum Color");
#elif defined(__GNUC__)
  REQUIRE(type_name == "SomeStruct");
  REQUIRE(NAMEOF_TYPE(decltype(ptr_s)) == "SomeStruct*");
  REQUIRE(NAMEOF_TYPE(decltype(ref_s)) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE(SomeStruct) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE(SomeStruct *) == "SomeStruct*");
  REQUIRE(NAMEOF_TYPE(const SomeStruct &) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE(const SomeStruct volatile *) == "const volatile SomeStruct*");

  REQUIRE(NAMEOF_TYPE(SomeClass<int>) == "SomeClass<int>");
  REQUIRE(NAMEOF_TYPE(const SomeClass<int> volatile *) == "const volatile SomeClass<int>*");

  REQUIRE(NAMEOF_TYPE(decltype(othervar)) == "Long");
  REQUIRE(NAMEOF_TYPE(Long) == "Long");
  REQUIRE(NAMEOF_TYPE(Long::LL) == "Long::LL");

  REQUIRE(NAMEOF_TYPE(Color) == "Color");
#endif
}

TEST_CASE("nameof::nameof_type") {
  constexpr auto type_name = nameof::nameof_type<decltype(struct_var)>();
#if defined(__clang__)
  REQUIRE(type_name == "SomeStruct");
  REQUIRE(nameof::nameof_type<decltype(ptr_s)>() == "SomeStruct *");
  REQUIRE(nameof::nameof_type<decltype(ref_s)>() == "SomeStruct");
  REQUIRE(nameof::nameof_type<SomeStruct>() == "SomeStruct");
  REQUIRE(nameof::nameof_type<SomeStruct *>() == "SomeStruct *");
  REQUIRE(nameof::nameof_type<const SomeStruct &>() == "SomeStruct");
  REQUIRE(nameof::nameof_type<const SomeStruct volatile *>() == "const volatile SomeStruct *");

  REQUIRE(nameof::nameof_type<SomeClass<int>>() == "SomeClass<int>");
  REQUIRE(nameof::nameof_type<const SomeClass<int> volatile *>() == "const volatile SomeClass<int> *");

  REQUIRE(nameof::nameof_type<decltype(othervar)>() == "Long");
  REQUIRE(nameof::nameof_type<Long>() == "Long");
  REQUIRE(nameof::nameof_type<Long::LL>() == "Long::LL");

  REQUIRE(nameof::nameof_type<Color>() == "Color");
#elif defined(_MSC_VER)
  REQUIRE(type_name == "struct SomeStruct");
  REQUIRE(nameof::nameof_type<decltype(ptr_s)>() == "struct SomeStruct *");
  REQUIRE(nameof::nameof_type<decltype(ref_s)>() == "struct SomeStruct");
  REQUIRE(nameof::nameof_type<SomeStruct>() == "struct SomeStruct");
  REQUIRE(nameof::nameof_type<SomeStruct *>() == "struct SomeStruct *");
  REQUIRE(nameof::nameof_type<const SomeStruct &>() == "struct SomeStruct");
  REQUIRE(nameof::nameof_type<const SomeStruct volatile *>() == "struct SomeStruct const volatile *");

  REQUIRE(nameof::nameof_type<SomeClass<int>>() == "class SomeClass<int>");
  REQUIRE(nameof::nameof_type<const SomeClass<int> volatile *>() == "class SomeClass<int> const volatile *");

  REQUIRE(nameof::nameof_type<decltype(othervar)>() == "struct Long");
  REQUIRE(nameof::nameof_type<Long>() == "struct Long");
  REQUIRE(nameof::nameof_type<Long::LL>() == "struct Long::LL");

  REQUIRE(nameof::nameof_type<Color>() == "enum Color");
#elif defined(__GNUC__)
  REQUIRE(type_name == "SomeStruct");
  REQUIRE(nameof::nameof_type<decltype(ptr_s)>() == "SomeStruct*");
  REQUIRE(nameof::nameof_type<decltype(ref_s)>() == "SomeStruct");
  REQUIRE(nameof::nameof_type<SomeStruct>() == "SomeStruct");
  REQUIRE(nameof::nameof_type<SomeStruct *>() == "SomeStruct*");
  REQUIRE(nameof::nameof_type<const SomeStruct &>() == "SomeStruct");
  REQUIRE(nameof::nameof_type<const SomeStruct volatile *>() == "const volatile SomeStruct*");

  REQUIRE(nameof::nameof_type<SomeClass<int>>() == "SomeClass<int>");
  REQUIRE(nameof::nameof_type<const SomeClass<int> volatile *>() == "const volatile SomeClass<int>*");

  REQUIRE(nameof::nameof_type<decltype(othervar)>() == "Long");
  REQUIRE(nameof::nameof_type<Long>() == "Long");
  REQUIRE(nameof::nameof_type<Long::LL>() == "Long::LL");

  REQUIRE(nameof::nameof_type<Color>() == "Color");
#endif
}
