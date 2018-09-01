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

template <typename T, typename U>
std::string SomeMethod4(U value) {
  return NAMEOF(SomeMethod4<T, U>) + "<" + NAMEOF_TYPE_T(T) + ", " + NAMEOF_TYPE_T(U) + ">(" + NAMEOF_TYPE_T(U) + " " + NAMEOF(value) + ")";
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

enum Directions { Up, Down, Right, Left};

SomeStruct struct_var;
Long othervar;
SomeStruct * ptr_s = &struct_var;
SomeStruct & ref_s = struct_var;

SomeClass<int> class_var;
const SomeClass<int> volatile * ptr_c = nullptr;

const Color color = Color::RED;
const Directions directions = Directions::Right;

TEST_CASE("constexpr") {
  SECTION("NAMEOF") {
    // variable
    constexpr auto cx1 = NAMEOF(struct_var);
    static_assert(cx1 == "struct_var", "");
    REQUIRE(cx1 == "struct_var");
    // member
    constexpr auto cx2 = NAMEOF((&struct_var)->somefield);
    static_assert(cx2 == "somefield", "");
    REQUIRE(cx2 == "somefield");
    // function
    constexpr auto cx3 = NAMEOF(&SomeClass<int>::SomeMethod6<long int>);
    static_assert(cx3 == "SomeMethod6", "");
    REQUIRE(cx3 == "SomeMethod6");
    // enum
    constexpr auto cx4 = NAMEOF(Color::RED);
    static_assert(cx4 == "RED", "");
    REQUIRE(cx4 == "RED");
  }

  SECTION("NAMEOF_FULL") {
    // variable
    constexpr auto cx1 = NAMEOF_FULL(struct_var);
    static_assert(cx1 == "struct_var", "");
    REQUIRE(cx1 == "struct_var");
    // member
    constexpr auto cx2 = NAMEOF_FULL((&struct_var)->somefield);
    static_assert(cx2 == "somefield", "");
    REQUIRE(cx2 == "somefield");
    // function
    constexpr auto cx3 = NAMEOF_FULL(&SomeClass<int>::SomeMethod6<long int>);
    static_assert(cx3 == "SomeMethod6<long int>", "");
    REQUIRE(cx3 == "SomeMethod6<long int>");
    // enum
    constexpr auto cx4 = NAMEOF_FULL(Color::RED);
    static_assert(cx4 == "RED", "");
    REQUIRE(cx4 == "RED");
  }

  SECTION("NAMEOF_RAW") {
    // variable
    constexpr auto cx1 = NAMEOF_RAW(struct_var);
    static_assert(cx1 == "struct_var", "");
    REQUIRE(cx1 == "struct_var");
    // member
    constexpr auto cx2 = NAMEOF_RAW((&struct_var)->somefield);
    static_assert(cx2 == "(&struct_var)->somefield", "");
    REQUIRE(cx2 == "(&struct_var)->somefield");
    // function
    constexpr auto cx4 = NAMEOF_RAW(&SomeStruct::SomeMethod2);
    static_assert(cx4 == "&SomeStruct::SomeMethod2", "");
    REQUIRE(cx4 == "&SomeStruct::SomeMethod2");
    // enum
    constexpr auto cx5 = NAMEOF_RAW(Color::RED);
    static_assert(cx5 == "Color::RED", "");
    REQUIRE(cx5 == "Color::RED");
    // macros
    constexpr auto cx6 = NAMEOF_RAW(__cplusplus);
    static_assert(cx6 == "__cplusplus", "");
    REQUIRE(cx6 == "__cplusplus");
  }

  SECTION("NAMEOF_ENUM") {
    constexpr auto cx = NAMEOF_ENUM(color);
#if defined(__clang__) || defined(_MSC_VER)
    static_assert(cx == "RED", "");
    REQUIRE(cx == "RED");
#elif defined(__GNUC__)
    REQUIRE(cx == "(const Color)-1");
#endif
  }

#if defined(NAMEOF_TYPE_HAS_CONSTEXPR)
  SECTION("NAMEOF_TYPE") {
    constexpr auto cx = NAMEOF_TYPE(ptr_c);
#  if defined(__clang__)
    static_assert(cx == "const volatile SomeClass<int> *", "");
    REQUIRE(cx == "const volatile SomeClass<int> *");
#  elif defined(__GNUC__)
    REQUIRE(cx == "const volatile SomeClass<int>*");
#  elif defined(_MSC_VER)
    static_assert(cx == "SomeClass<int> const volatile *", "");
    REQUIRE(cx == "SomeClass<int> const volatile *");
#  endif
  }

  SECTION("NAMEOF_TYPE_T") {
    constexpr auto cx = NAMEOF_TYPE_T(const SomeClass<int> volatile *);
#  if defined(__clang__)
    static_assert(cx == "const volatile SomeClass<int> *", "");
    REQUIRE(cx == "const volatile SomeClass<int> *");
#  elif defined(__GNUC__)
    REQUIRE(cx == "const volatile SomeClass<int>*");
#  elif defined(_MSC_VER)
    static_assert(cx == "SomeClass<int> const volatile *", "");
    REQUIRE(cx == "SomeClass<int> const volatile *");
#  endif
  }
#endif
}

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

  SECTION("macros") {
    REQUIRE(NAMEOF_RAW(__cplusplus) == "__cplusplus");
    REQUIRE(NAMEOF_RAW(__LINE__) == "__LINE__");
    REQUIRE(NAMEOF_RAW(__FILE__) == "__FILE__");
  }
}

TEST_CASE("NAMEOF_ENUM") {
#  if defined(__clang__) || defined(_MSC_VER)
  REQUIRE(NAMEOF_ENUM(Color::RED) == "RED");
  REQUIRE(NAMEOF_ENUM(::color) == "RED");

  auto color_ = Color::BLUE;
  REQUIRE(NAMEOF_ENUM(Color::BLUE) == "BLUE");
  REQUIRE(NAMEOF_ENUM(color_) == "BLUE");

  REQUIRE(NAMEOF_ENUM(Directions::Right) == "Right");
  REQUIRE(NAMEOF_ENUM(directions) == "Right");
#  elif defined(__GNUC__)
  REQUIRE(NAMEOF_ENUM(Color::RED) == "(Color)-1");
  REQUIRE(NAMEOF_ENUM(color) == "(const Color)-1");

  REQUIRE(NAMEOF_ENUM(Directions::Right) == "(Directions)2");
  REQUIRE(NAMEOF_ENUM(directions) == "(const Directions)2");
#  endif
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

TEST_CASE("Spaces and Tabs ignored") {
  SECTION("Spaces") {
    REQUIRE(NAMEOF(   struct_var   ) == "struct_var");
    REQUIRE(NAMEOF_FULL(   struct_var   ) == "struct_var");
    REQUIRE(NAMEOF_RAW(   struct_var   ) == "struct_var");
#if defined(__clang__) || defined(_MSC_VER)
    REQUIRE(NAMEOF_ENUM(   color   ) == "RED");
#elif defined(__GNUC__)
    REQUIRE(NAMEOF_ENUM(   color   ) == "(const Color)-1");
#endif
    REQUIRE(NAMEOF_TYPE(   struct_var   ) == "SomeStruct");
    REQUIRE(NAMEOF_TYPE_T(   decltype(struct_var)   ) == "SomeStruct");
  }

  SECTION("Tabs") {
    REQUIRE(NAMEOF(	struct_var	) == "struct_var");
    REQUIRE(NAMEOF_FULL(	struct_var	) == "struct_var");
    REQUIRE(NAMEOF_RAW(	struct_var	) == "struct_var");
#if defined(__clang__) || defined(_MSC_VER)
    REQUIRE(NAMEOF_ENUM(	color	) == "RED");
#elif defined(__GNUC__)
    REQUIRE(NAMEOF_ENUM(	color	) == "(const Color)-1");
#endif
    REQUIRE(NAMEOF_TYPE(	struct_var	) == "SomeStruct");
    REQUIRE(NAMEOF_TYPE_T(	decltype(struct_var)	) == "SomeStruct");
  }
}

TEST_CASE("cstring") {}
