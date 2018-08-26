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

enum class Color { RED, GREEN, BLUE };

SomeStruct structvar;
Long othervar;
SomeStruct * ptrvar = &structvar;
SomeStruct & refvar = structvar;
const SomeClass<int> volatile * classvar;

TEST_CASE("constexpr") {
  SECTION("NAMEOF") {
    // variable
    constexpr auto cx1 = NAMEOF(structvar);
    static_assert(cx1 == "structvar", "");
    // member
    constexpr auto cx2 = NAMEOF((&structvar)->somefield);
    static_assert(cx2 == "somefield", "");
    // function
    constexpr auto cx3 = NAMEOF(&SomeClass<int>::SomeMethod6<long int>);
    static_assert(cx3 == "SomeMethod6", "");
    // enum
    constexpr auto cx4 = NAMEOF(Color::RED);
    static_assert(cx4 == "RED", "");
  }

  SECTION("NAMEOF_FULL") {
    // variable
    constexpr auto cx1 = NAMEOF_FULL(structvar);
    static_assert(cx1 == "structvar", "");
    // member
    constexpr auto cx2 = NAMEOF_FULL((&structvar)->somefield);
    static_assert(cx2 == "somefield", "");
    // function
    constexpr auto cx3 = NAMEOF_FULL(&SomeClass<int>::SomeMethod6<long int>);
    static_assert(cx3 == "SomeMethod6<long int>", "");
    // enum
    constexpr auto cx4 = NAMEOF_FULL(Color::RED);
    static_assert(cx4 == "RED", "");
  }

  SECTION("NAMEOF_RAW") {
    // variable
    constexpr auto cx1 = NAMEOF_RAW(structvar);
    static_assert(cx1 == "structvar", "");
    // member
    constexpr auto cx2 = NAMEOF_RAW((&structvar)->somefield);
    static_assert(cx2 == "(&structvar)->somefield", "");
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

  // constexpr in NAMEOF_TYPE not supported if GCC.
#if defined(_MSC_VER) || defined(__clang__)
  SECTION("NAMEOF_TYPE") {
    constexpr auto cx = NAMEOF_TYPE(classvar);
    static_assert(cx == "SomeClass", "");
  }

  SECTION("NAMEOF_TYPE") {
    constexpr auto cx = NAMEOF_TYPE_T(const SomeClass<int> volatile *);
    static_assert(cx == "SomeClass", "");
  }

  SECTION("NAMEOF_TYPE_FULL") {
    constexpr auto cx = NAMEOF_TYPE_FULL(classvar);
    static_assert(cx == "SomeClass<int>", "");
  }

  SECTION("NAMEOF_TYPE_FULL") {
    constexpr auto cx = NAMEOF_TYPE_FULL_T(const SomeClass<int> volatile *);
    static_assert(cx == "SomeClass<int>", "");
  }

  SECTION("NAMEOF_TYPE_RAW") {
    constexpr auto cx = NAMEOF_TYPE_RAW(classvar);
#if defined(__clang__)
    static_assert(cx == "const volatile SomeClass<int> *", "");
#elif defined(_MSC_VER)
    static_assert(cx == "SomeClass<int> const volatile *", "");
#endif
  }
  SECTION("NAMEOF_TYPE_RAW") {
    constexpr auto cx = NAMEOF_TYPE_RAW_T(const SomeClass<int> volatile *);
#if defined(__clang__)
    static_assert(cx == "const volatile SomeClass<int> *", "");
#elif defined(_MSC_VER)
    static_assert(cx == "SomeClass<int> const volatile *", "");
#endif
  }
#endif
}

TEST_CASE("NAMEOF") {
  SECTION("variable") {
    REQUIRE(NAMEOF(structvar) == "structvar");
    REQUIRE(NAMEOF(::structvar) == "structvar");
    REQUIRE(NAMEOF(ptrvar) == "ptrvar");
  }

  SECTION("member") {
    REQUIRE(NAMEOF(structvar.somefield) == "somefield");
    REQUIRE(NAMEOF((&structvar)->somefield) == "somefield");
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

TEST_CASE("NAMEOF_FULL") {
  SECTION("variable") {
    REQUIRE(NAMEOF_FULL(structvar) == "structvar");
    REQUIRE(NAMEOF_FULL(::structvar) == "structvar");
    REQUIRE(NAMEOF_FULL(ptrvar) == "ptrvar");
  }

  SECTION("member") {
    REQUIRE(NAMEOF_FULL(structvar.somefield) == "somefield");
    REQUIRE(NAMEOF_FULL((&structvar)->somefield) == "somefield");
    REQUIRE(NAMEOF_FULL(othervar.ll.field) == "field");
  }

  SECTION("function") {
    REQUIRE(NAMEOF_FULL(&SomeStruct::SomeMethod1) == "SomeMethod1");
    REQUIRE(NAMEOF_FULL(&SomeStruct::SomeMethod2) == "SomeMethod2");
    REQUIRE(NAMEOF_FULL(SomeMethod3) == "SomeMethod3");
    REQUIRE(NAMEOF_FULL(SomeMethod4<int>) == "SomeMethod4<int>");
    REQUIRE(NAMEOF_FULL(&SomeClass<int>::SomeMethod5) == "SomeMethod5");
    REQUIRE(NAMEOF_FULL(&SomeClass<int>::SomeMethod6<long int>) == "SomeMethod6<long int>");
  }

  SECTION("enum") {
    REQUIRE(NAMEOF_FULL(Color::RED) == "RED");
    REQUIRE(NAMEOF_FULL(Color::BLUE) == "BLUE");
  }
}

TEST_CASE("NAMEOF_RAW") {
  SECTION("variable") {
    REQUIRE(NAMEOF_RAW(structvar) == "structvar");
    REQUIRE(NAMEOF_RAW(&structvar) == "&structvar");
    REQUIRE(NAMEOF_RAW(::structvar) == "::structvar");
    REQUIRE(NAMEOF_RAW(ptrvar) == "ptrvar");
    REQUIRE(NAMEOF_RAW(*ptrvar) == "*ptrvar");
    REQUIRE(NAMEOF_RAW(+structvar.somefield) == "+structvar.somefield");
    REQUIRE(NAMEOF_RAW(-structvar.somefield) == "-structvar.somefield");
    REQUIRE(NAMEOF_RAW(~structvar.somefield) == "~structvar.somefield");
    REQUIRE(NAMEOF_RAW(!structvar.somefield) == "!structvar.somefield");
    REQUIRE(NAMEOF_RAW(structvar.somefield + refvar.somefield) == "structvar.somefield + refvar.somefield");
  }

  SECTION("member") {
    REQUIRE(NAMEOF_RAW(structvar.somefield) == "structvar.somefield");
    REQUIRE(NAMEOF_RAW((&structvar)->somefield) == "(&structvar)->somefield");
    REQUIRE(NAMEOF_RAW(othervar.ll.field) == "othervar.ll.field");
  }

  SECTION("type") {
    REQUIRE(NAMEOF_RAW(int[]) == "int[]");
    REQUIRE(NAMEOF_RAW(int) == "int");
    REQUIRE(NAMEOF_RAW(const volatile int[]) == "const volatile int[]");
    REQUIRE(NAMEOF_RAW(std::string) == "std::string");
    REQUIRE(NAMEOF_RAW(SomeStruct) == "SomeStruct");
    REQUIRE(NAMEOF_RAW(Long::LL) == "Long::LL");
    REQUIRE(NAMEOF_RAW(Color) == "Color");
  }

  SECTION("function") {
    REQUIRE(NAMEOF_RAW(&SomeStruct::SomeMethod1) == "&SomeStruct::SomeMethod1");
    REQUIRE(NAMEOF_RAW(&SomeStruct::SomeMethod2) == "&SomeStruct::SomeMethod2");
    REQUIRE(NAMEOF_RAW(SomeMethod3) == "SomeMethod3");
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

TEST_CASE("NAMEOF_TYPE") {
  REQUIRE(NAMEOF_TYPE(structvar) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE(ptrvar) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE(refvar) == "SomeStruct");

  REQUIRE(NAMEOF_TYPE(classvar) == "SomeClass");
  REQUIRE(NAMEOF_TYPE(std::declval<const SomeClass<int> volatile *>()) == "SomeClass");

  REQUIRE(NAMEOF_TYPE(othervar) == "Long");
  REQUIRE(NAMEOF_TYPE(othervar.ll) == "LL");
  REQUIRE(NAMEOF_TYPE(othervar.ll.field) == "int");

  REQUIRE(NAMEOF_TYPE(std::string{}) == "basic_string");

  REQUIRE(NAMEOF_TYPE(Color::RED) == "Color");
}

TEST_CASE("NAMEOF_TYPE_T") {
  REQUIRE(NAMEOF_TYPE_T(decltype(structvar)) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_T(decltype(ptrvar)) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_T(decltype(refvar)) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_T(SomeStruct) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_T(SomeStruct *) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_T(SomeStruct &) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_T(const SomeStruct volatile *) == "SomeStruct");

  REQUIRE(NAMEOF_TYPE_T(SomeClass<int>) == "SomeClass");
  REQUIRE(NAMEOF_TYPE_T(const SomeClass<int> volatile *) == "SomeClass");

  REQUIRE(NAMEOF_TYPE_T(decltype(othervar)) == "Long");
  REQUIRE(NAMEOF_TYPE_T(Long) == "Long");
  REQUIRE(NAMEOF_TYPE_T(Long::LL) == "LL");

  REQUIRE(NAMEOF_TYPE_T(std::string) == "basic_string");

  REQUIRE(NAMEOF_TYPE_T(Color) == "Color");
}

TEST_CASE("NAMEOF_TYPE_FULL") {
  REQUIRE(NAMEOF_TYPE_FULL(structvar) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_FULL(ptrvar) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_FULL(refvar) == "SomeStruct");

  REQUIRE(NAMEOF_TYPE_FULL(classvar) == "SomeClass<int>");
  REQUIRE(NAMEOF_TYPE_FULL(std::declval<const SomeClass<int> volatile *>()) == "SomeClass<int>");

  REQUIRE(NAMEOF_TYPE_FULL(othervar) == "Long");
  REQUIRE(NAMEOF_TYPE_FULL(othervar.ll) == "LL");
  REQUIRE(NAMEOF_TYPE_FULL(othervar.ll.field) == "int");

  REQUIRE(NAMEOF_TYPE_FULL(Color::RED) == "Color");
}

TEST_CASE("NAMEOF_TYPE_FULL_T") {
  REQUIRE(NAMEOF_TYPE_FULL_T(decltype(structvar)) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_FULL_T(decltype(ptrvar)) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_FULL_T(decltype(refvar)) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_FULL_T(SomeStruct) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_FULL_T(SomeStruct *) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_FULL_T(SomeStruct &) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_FULL_T(const SomeStruct volatile *) == "SomeStruct");

  REQUIRE(NAMEOF_TYPE_FULL_T(SomeClass<int>) == "SomeClass<int>");
  REQUIRE(NAMEOF_TYPE_FULL_T(const SomeClass<int> volatile *) == "SomeClass<int>");

  REQUIRE(NAMEOF_TYPE_FULL_T(decltype(othervar)) == "Long");
  REQUIRE(NAMEOF_TYPE_FULL_T(Long) == "Long");
  REQUIRE(NAMEOF_TYPE_FULL_T(Long::LL) == "LL");

  REQUIRE(NAMEOF_TYPE_FULL_T(Color) == "Color");
}

TEST_CASE("NAMEOF_TYPE_RAW") {
#if defined(__clang__)
  REQUIRE(NAMEOF_TYPE_RAW(structvar) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_RAW(ptrvar) == "SomeStruct *");
  REQUIRE(NAMEOF_TYPE_RAW(refvar) == "SomeStruct &");

  REQUIRE(NAMEOF_TYPE_RAW(classvar) == "const volatile SomeClass<int> *");

  REQUIRE(NAMEOF_TYPE_RAW(othervar) == "Long");
  REQUIRE(NAMEOF_TYPE_RAW(othervar.ll) == "Long::LL");
  REQUIRE(NAMEOF_TYPE_RAW(othervar.ll.field) == "int");

  REQUIRE(NAMEOF_TYPE_RAW(Color::RED) == "Color");

  REQUIRE(NAMEOF_TYPE_RAW(std::declval<const SomeClass<int>>()) == "const SomeClass<int> &&");
#elif defined(_MSC_VER)
  REQUIRE(NAMEOF_TYPE_RAW(structvar) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_RAW(ptrvar) == "SomeStruct *");
  REQUIRE(NAMEOF_TYPE_RAW(refvar) == "SomeStruct &");

  REQUIRE(NAMEOF_TYPE_RAW(classvar) == "SomeClass<int> const volatile *");

  REQUIRE(NAMEOF_TYPE_RAW(othervar) == "Long");
  REQUIRE(NAMEOF_TYPE_RAW(othervar.ll) == "Long::LL");
  REQUIRE(NAMEOF_TYPE_RAW(othervar.ll.field) == "int");

  REQUIRE(NAMEOF_TYPE_RAW(Color::RED) == "Color");

  REQUIRE(NAMEOF_TYPE_RAW(std::declval<const SomeClass<int>>()) == "SomeClass<int> const &&");
#elif defined(__GNUC__)
  REQUIRE(NAMEOF_TYPE_RAW(structvar) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_RAW(ptrvar) == "SomeStruct*");
  REQUIRE(NAMEOF_TYPE_RAW(refvar) == "SomeStruct&");

  REQUIRE(NAMEOF_TYPE_RAW(classvar) == "const volatile SomeClass<int>*");

  REQUIRE(NAMEOF_TYPE_RAW(othervar) == "Long");
  REQUIRE(NAMEOF_TYPE_RAW(othervar.ll) == "Long::LL");
  REQUIRE(NAMEOF_TYPE_RAW(othervar.ll.field) == "int");

  REQUIRE(NAMEOF_TYPE_RAW(Color::RED) == "Color");

  REQUIRE(NAMEOF_TYPE_RAW(std::declval<const SomeClass<int>>()) == "const SomeClass<int>&&");
#endif
}

TEST_CASE("NAMEOF_TYPE_RAW_T") {
#if defined(__clang__)
  REQUIRE(NAMEOF_TYPE_RAW_T(decltype(structvar)) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_RAW_T(decltype(ptrvar)) == "SomeStruct *");
  REQUIRE(NAMEOF_TYPE_RAW_T(decltype(refvar)) == "SomeStruct &");
  REQUIRE(NAMEOF_TYPE_RAW_T(SomeStruct) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_RAW_T(SomeStruct *) == "SomeStruct *");
  REQUIRE(NAMEOF_TYPE_RAW_T(SomeStruct &) == "SomeStruct &");
  REQUIRE(NAMEOF_TYPE_RAW_T(const SomeStruct volatile *) == "const volatile SomeStruct *");

  REQUIRE(NAMEOF_TYPE_RAW_T(SomeClass<int>) == "SomeClass<int>");
  REQUIRE(NAMEOF_TYPE_RAW_T(const SomeClass<int> volatile *) == "const volatile SomeClass<int> *");

  REQUIRE(NAMEOF_TYPE_RAW_T(decltype(othervar)) == "Long");
  REQUIRE(NAMEOF_TYPE_RAW_T(Long) == "Long");
  REQUIRE(NAMEOF_TYPE_RAW_T(Long::LL) == "Long::LL");

  REQUIRE(NAMEOF_TYPE_RAW_T(Color) == "Color");
#elif defined(_MSC_VER)
  REQUIRE(NAMEOF_TYPE_RAW_T(decltype(structvar)) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_RAW_T(decltype(ptrvar)) == "SomeStruct *");
  REQUIRE(NAMEOF_TYPE_RAW_T(decltype(refvar)) == "SomeStruct &");
  REQUIRE(NAMEOF_TYPE_RAW_T(SomeStruct) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_RAW_T(SomeStruct *) == "SomeStruct *");
  REQUIRE(NAMEOF_TYPE_RAW_T(SomeStruct &) == "SomeStruct &");
  REQUIRE(NAMEOF_TYPE_RAW_T(const SomeStruct volatile *) == "SomeStruct const volatile *");

  REQUIRE(NAMEOF_TYPE_RAW_T(SomeClass<int>) == "SomeClass<int>");
  REQUIRE(NAMEOF_TYPE_RAW_T(const SomeClass<int> volatile *) == "SomeClass<int> const volatile *");

  REQUIRE(NAMEOF_TYPE_RAW_T(decltype(othervar)) == "Long");
  REQUIRE(NAMEOF_TYPE_RAW_T(Long) == "Long");
  REQUIRE(NAMEOF_TYPE_RAW_T(Long::LL) == "Long::LL");

  REQUIRE(NAMEOF_TYPE_RAW_T(Color) == "Color");
#elif defined(__GNUC__)
  REQUIRE(NAMEOF_TYPE_RAW_T(decltype(structvar)) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_RAW_T(decltype(ptrvar)) == "SomeStruct*");
  REQUIRE(NAMEOF_TYPE_RAW_T(decltype(refvar)) == "SomeStruct&");
  REQUIRE(NAMEOF_TYPE_RAW_T(SomeStruct) == "SomeStruct");
  REQUIRE(NAMEOF_TYPE_RAW_T(SomeStruct *) == "SomeStruct*");
  REQUIRE(NAMEOF_TYPE_RAW_T(SomeStruct &) == "SomeStruct&");
  REQUIRE(NAMEOF_TYPE_RAW_T(const SomeStruct volatile *) == "const volatile SomeStruct*");

  REQUIRE(NAMEOF_TYPE_RAW_T(SomeClass<int>) == "SomeClass<int>");
  REQUIRE(NAMEOF_TYPE_RAW_T(const SomeClass<int> volatile *) == "const volatile SomeClass<int>*");

  REQUIRE(NAMEOF_TYPE_RAW_T(decltype(othervar)) == "Long");
  REQUIRE(NAMEOF_TYPE_RAW_T(Long) == "Long");
  REQUIRE(NAMEOF_TYPE_RAW_T(Long::LL) == "Long::LL");

  REQUIRE(NAMEOF_TYPE_RAW_T(Color) == "Color");
#endif
}

TEST_CASE("Spaces and Tabs ignored") {
  SECTION("Spaces") {
    REQUIRE(NAMEOF(   structvar   ) == "structvar");
    REQUIRE(NAMEOF_FULL(   structvar   ) == "structvar");
    REQUIRE(NAMEOF_RAW(   structvar   ) == "structvar");

    REQUIRE(NAMEOF_TYPE(   structvar   ) == "SomeStruct");
    REQUIRE(NAMEOF_TYPE_T(   decltype(structvar)   ) == "SomeStruct");
    REQUIRE(NAMEOF_TYPE_FULL(   structvar   ) == "SomeStruct");
    REQUIRE(NAMEOF_TYPE_FULL_T(   decltype(structvar)   ) == "SomeStruct");
    REQUIRE(NAMEOF_TYPE_RAW(   structvar   ) == "SomeStruct");
    REQUIRE(NAMEOF_TYPE_RAW_T(   decltype(structvar)   ) == "SomeStruct");
  }

  SECTION("Tabs") {
    REQUIRE(NAMEOF(	structvar	) == "structvar");
    REQUIRE(NAMEOF_FULL(	structvar	) == "structvar");
    REQUIRE(NAMEOF_RAW(	structvar	) == "structvar");

    REQUIRE(NAMEOF_TYPE(	structvar	) == "SomeStruct");
    REQUIRE(NAMEOF_TYPE_T(	decltype(structvar)	) == "SomeStruct");
    REQUIRE(NAMEOF_TYPE_FULL(	structvar	) == "SomeStruct");
    REQUIRE(NAMEOF_TYPE_FULL_T(	decltype(structvar)	) == "SomeStruct");
    REQUIRE(NAMEOF_TYPE_RAW(	structvar	) == "SomeStruct");
    REQUIRE(NAMEOF_TYPE_RAW_T(	decltype(structvar)	) == "SomeStruct");
  }
}
