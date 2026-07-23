// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2018 - 2026 Daniil Goncharov <neargye@gmail.com>.
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

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <string>
#include <string_view>
#include <sstream>
#include <stdexcept>
#include <type_traits>

#if __has_include(<fmt/format.h>)
#  include <fmt/format.h>
#  if defined(FMT_VERSION)
#    define NAMEOF_TEST_HAS_FMT_FORMAT 1
#  endif
#endif

#define NAMEOF_ENUM_RANGE_MIN -120
#define NAMEOF_ENUM_RANGE_MAX 120
#include <nameof.hpp>

#if defined(__cpp_lib_format) && __cpp_lib_format >= 201907L
#  define NAMEOF_TEST_HAS_STD_FORMAT 1
#endif

#ifdef NDEBUG
#  define NAMEOF_DEBUG_REQUIRE(...) REQUIRE(__VA_ARGS__)
#else
#  define NAMEOF_DEBUG_REQUIRE(...)
#endif

struct SomeStruct {
  int somefield = 0;

  void SomeMethod1(int) {
    throw std::runtime_error{"should not be called!"};
   }

  int SomeMethod2() const {
    throw std::runtime_error{"should not be called!"};
  }

  static int somestaticfield;
  constexpr static int someotherstaticfield = 21;
};

struct NameOfTest {
  int some_member;
};

int SomeStruct::somestaticfield;

int someglobalvariable = 0;
const int someglobalconstvariable = 42;

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

enum class Numbers : int { one = 1, two, three, many = 127 };

enum Directions { Up = 85, Down = -42, Right = 120, Left = -120 };

enum number : unsigned long { one = 100, two = 200, three = 300, four = 400 };

enum AnimalFlags {
  HasClaws = 1,
  CanFly = 2,
  EatsFish = 4,
  Endangered = 8,
};

enum class BigFlags : std::uint64_t {
  A = 1,
  B = (static_cast<std::uint64_t>(0x1) << 20),
  C = (static_cast<std::uint64_t>(0x1) << 40),
  D = (static_cast<std::uint64_t>(0x1) << 63),
};

template <>
struct nameof::customize::enum_range<number> {
  static_assert(std::is_enum_v<number>, "nameof::enum_range<number> requires enum type.");
  static constexpr int min = 100;
  static constexpr int max = 300;
  static_assert(max > min, "nameof::enum_range<number> requires max > min.");
};

enum class OutOfRange {
  too_low = NAMEOF_ENUM_RANGE_MIN - 1,
  required_to_work = 0,
  too_high = NAMEOF_ENUM_RANGE_MAX + 1
};

enum class EntirelyOutOfRange {
  too_low = NAMEOF_ENUM_RANGE_MIN - 1,
  too_high = NAMEOF_ENUM_RANGE_MAX + 1
};

enum class CustomEnum { default_name = 1, custom_name = 2 };

namespace {
enum class HiddenEnum { A = 1 };
namespace {
enum class NestedHiddenEnum { A = 1 };
}
}

struct CustomNameTarget {
  int member = 0;
};

inline int custom_pointer_value = 0;

template <>
constexpr nameof::string_view nameof::customize::enum_name<CustomEnum>(CustomEnum value) noexcept {
  switch (value) {
    case CustomEnum::custom_name:
      return "custom enum";
    default:
      return {};
  }
}

#if defined(NAMEOF_ENUM_SUPPORTED)
static_assert(nameof::detail::is_valid<Color, -12>(), "nameof::detail::is_valid requires valid enum values.");
static_assert(!nameof::detail::is_valid<Color, 0>(), "nameof::detail::is_valid requires invalid enum values.");
static_assert(nameof::detail::is_valid<Numbers, 127>(), "nameof::detail::is_valid requires valid enum values.");
static_assert(!nameof::detail::is_valid<Numbers, 0>(), "nameof::detail::is_valid requires invalid enum values.");
static_assert(nameof::detail::is_valid<AnimalFlags, 1>(), "nameof::detail::is_valid requires valid flag values.");
static_assert(!nameof::detail::is_valid<AnimalFlags, 3>(), "nameof::detail::is_valid requires invalid flag values.");
static_assert(nameof::detail::is_valid<CustomEnum, 2>(), "nameof::detail::is_valid requires custom enum names.");
static_assert(!nameof::detail::is_valid<CustomEnum, 0>(), "nameof::detail::is_valid requires invalid custom enum values.");
static_assert(nameof::detail::is_valid<HiddenEnum, 1>(), "nameof::detail::is_valid requires anonymous namespace enum values.");
static_assert(!nameof::detail::is_valid<HiddenEnum, 0>(), "nameof::detail::is_valid requires invalid anonymous namespace enum values.");
static_assert(nameof::nameof_enum(HiddenEnum::A) == "A", "nameof::nameof_enum requires anonymous namespace enum values.");
static_assert(NAMEOF_ENUM_CONST(HiddenEnum::A) == "A", "NAMEOF_ENUM_CONST requires anonymous namespace enum values.");
static_assert(nameof::detail::is_valid<NestedHiddenEnum, 1>(), "nameof::detail::is_valid requires nested anonymous namespace enum values.");
static_assert(!nameof::detail::is_valid<NestedHiddenEnum, 0>(), "nameof::detail::is_valid requires invalid nested anonymous namespace enum values.");
static_assert(nameof::nameof_enum(NestedHiddenEnum::A) == "A", "nameof::nameof_enum requires nested anonymous namespace enum values.");
static_assert(NAMEOF_ENUM_CONST(NestedHiddenEnum::A) == "A", "NAMEOF_ENUM_CONST requires nested anonymous namespace enum values.");
#endif

template <>
constexpr nameof::string_view nameof::customize::type_name<CustomNameTarget>() noexcept {
  return "CustomType";
}

template <>
constexpr nameof::string_view nameof::customize::member_name<&CustomNameTarget::member>() noexcept {
  return "custom_member";
}

template <>
constexpr nameof::string_view nameof::customize::pointer_name<&custom_pointer_value>() noexcept {
  return "custom_pointer";
}

struct TestRtti {
  struct Base { virtual ~Base() = default; };
  struct Derived : Base {};
};

SomeStruct struct_var;
Long othervar;
SomeStruct* ptr_s = &struct_var;
SomeStruct& ref_s = struct_var;

SomeClass<int> class_var;
const SomeClass<int> volatile * ptr_c = nullptr;

const Color color = Color::RED;

template <typename Name>
void require_cstring_contract(const Name& name, ::nameof::string_view expected) {
  const auto view = static_cast<::nameof::string_view>(name);

  REQUIRE(view == expected);
  REQUIRE(view.data() == name.data());
  REQUIRE(name.data() == name.c_str());
  REQUIRE(name.c_str() != nullptr);
  REQUIRE(name.c_str()[name.size()] == '\0');
  REQUIRE(std::char_traits<char>::length(name.c_str()) == name.size());
}

template <typename Name>
void require_expression_cstring_contract(const Name& name, ::nameof::string_view expected) {
  static_assert(!std::is_convertible_v<Name, ::nameof::string_view>);
  static_assert(std::is_convertible_v<const Name&, ::nameof::string_view>);
  static_assert(std::is_constructible_v<::nameof::string, Name>);
  require_cstring_contract(name, expected);
}

template <typename Name>
void require_string_view_uses_cstring_storage(::nameof::string_view view, const Name& storage, ::nameof::string_view expected) {
  require_cstring_contract(storage, expected);
  REQUIRE(view == expected);
  REQUIRE(view.data() == storage.data());
}

void require_string_view_contract(::nameof::string_view view, ::nameof::string_view expected) {
  REQUIRE(view == expected);
  REQUIRE(view.data() != nullptr);
  REQUIRE(view.data()[view.size()] == '\0');
}

void require_string_contract(const ::nameof::string& value, ::nameof::string_view expected) {
  REQUIRE(value == expected);
  REQUIRE(value.c_str() != nullptr);
  REQUIRE(value.c_str()[value.size()] == '\0');
}

template <typename Name>
void require_static_cstring_api_contract(const Name& name, ::nameof::string_view expected) {
  static_assert(std::is_convertible_v<const Name&, ::nameof::string_view>);
  static_assert(!std::is_convertible_v<Name, ::nameof::string_view>);
  static_assert(std::is_constructible_v<::nameof::string, const Name&>);
  require_cstring_contract(name, expected);
  require_string_contract(::nameof::string{name}, expected);
}

TEST_CASE("support constants") {
#if defined(NAMEOF_ENUM_SUPPORTED)
  static_assert(nameof::is_nameof_enum_supported);
#else
  static_assert(!nameof::is_nameof_enum_supported);
#endif

#if defined(NAMEOF_TYPE_SUPPORTED)
  static_assert(nameof::is_nameof_type_supported);
#else
  static_assert(!nameof::is_nameof_type_supported);
#endif

#if defined(NAMEOF_TYPE_RTTI_SUPPORTED)
  static_assert(nameof::is_nameof_type_rtti_supported);
#else
  static_assert(!nameof::is_nameof_type_rtti_supported);
#endif

#if defined(NAMEOF_MEMBER_SUPPORTED)
  static_assert(nameof::is_nameof_member_supported);
#else
  static_assert(!nameof::is_nameof_member_supported);
#endif

#if defined(NAMEOF_POINTER_SUPPORTED)
  static_assert(nameof::is_nameof_pointer_supported);
#else
  static_assert(!nameof::is_nameof_pointer_supported);
#endif
}

TEST_CASE("NAMEOF") {
  SUBCASE("variable") {
    constexpr auto name = NAMEOF(othervar);
    REQUIRE(name == "othervar");
    REQUIRE(NAMEOF(struct_var) == "struct_var");
    REQUIRE(NAMEOF(::struct_var) == "struct_var");
    REQUIRE(NAMEOF(ptr_s) == "ptr_s");
    REQUIRE(NAMEOF(color) == "color");
  }

  SUBCASE("member") {
    REQUIRE(NAMEOF(struct_var.somefield) == "somefield");
    REQUIRE(NAMEOF((&struct_var)->somefield) == "somefield");
    REQUIRE(NAMEOF(othervar.ll.field) == "field");
  }

  SUBCASE("function") {
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

  SUBCASE("enum") {
    REQUIRE(NAMEOF(Color::RED) == "RED");
    REQUIRE(NAMEOF(Color::BLUE) == "BLUE");
  }

  SUBCASE("cstring") {
    constexpr auto name = NAMEOF(othervar);
    require_expression_cstring_contract(name, "othervar");
    require_string_contract(::nameof::string{NAMEOF(othervar)}, "othervar");
  }
}

TEST_CASE("CSTRING") {
    constexpr auto content = ::nameof::string_view("content");
    constexpr auto cstring_N = ::nameof::cstring<content.size()>(content);

    static_assert(std::is_convertible_v<const decltype(cstring_N)&, ::nameof::string_view>);
    static_assert(!std::is_convertible_v<decltype(cstring_N), ::nameof::string_view>);
    static_assert(std::is_constructible_v<::nameof::string, const decltype(cstring_N)&>);
    static_assert(!std::is_convertible_v<const decltype(cstring_N)&, ::nameof::string>);

    SUBCASE("construction") {
        REQUIRE(cstring_N == content);

        constexpr auto copy = ::nameof::cstring<cstring_N.size()>(cstring_N);
        REQUIRE(copy == content);
    }
    SUBCASE("size_checking") {
        REQUIRE(cstring_N.empty() == false);
        REQUIRE(cstring_N.size() == content.size());
        REQUIRE(cstring_N.length() == content.size());
    }
    SUBCASE("compare") {
        REQUIRE(cstring_N.compare("b") > 0);
        REQUIRE(cstring_N.compare(content) == 0);
        REQUIRE(cstring_N.compare("d") < 0);

        REQUIRE((cstring_N > "b") == true);
        REQUIRE(("b" < cstring_N) == true);
        REQUIRE((cstring_N > "content") == false);
        REQUIRE((cstring_N > "d") == false);
        REQUIRE((cstring_N < "b") == false);
        REQUIRE((cstring_N < "content") == false);
        REQUIRE((cstring_N < "d") == true);
        REQUIRE((cstring_N >= "b") == true);
        REQUIRE((cstring_N >= "content") == true);
        REQUIRE((cstring_N >= "d") == false);
        REQUIRE((cstring_N <= "b") == false);
        REQUIRE((cstring_N <= "content") == true);
        REQUIRE((cstring_N <= "d") == true);
        REQUIRE(("d" > cstring_N) == true);
        REQUIRE(("d" >= cstring_N) == true);
        REQUIRE(("b" <= cstring_N) == true);
        REQUIRE(("content" >= cstring_N) == true);
        REQUIRE(("content" <= cstring_N) == true);
        REQUIRE((cstring_N == "content") == true);
        REQUIRE(("content" == cstring_N) == true);
        REQUIRE((cstring_N == "different") == false);
        REQUIRE((cstring_N != "content") == false);
        REQUIRE((cstring_N != "different") == true);
        REQUIRE(("different" != cstring_N) == true);
    }
    SUBCASE("iterators") {
        REQUIRE(::nameof::string{cstring_N.begin(), cstring_N.end()} == content);
        REQUIRE(::nameof::string{cstring_N.cbegin(), cstring_N.cend()} == content);
        REQUIRE(::nameof::string{cstring_N.rbegin(), cstring_N.rend()} == "tnetnoc");
        REQUIRE(::nameof::string{cstring_N.crbegin(), cstring_N.crend()} == "tnetnoc");
    }
    SUBCASE("retrieval") {
        REQUIRE(cstring_N.str() == content);
        REQUIRE(::nameof::string{cstring_N} == content);
        REQUIRE(static_cast<::nameof::string>(cstring_N) == content);
        REQUIRE(static_cast<::nameof::string_view>(cstring_N) == content);
        REQUIRE(content.compare(cstring_N.data()) == 0);
        REQUIRE(content.compare(cstring_N.c_str()) == 0);
        REQUIRE(content.compare(static_cast<const char *>(cstring_N)) == 0);
        require_cstring_contract(cstring_N, content);

        REQUIRE(cstring_N[4] == 'e');

        REQUIRE(cstring_N.front() == 'c');
        REQUIRE(cstring_N.back() == 't');
    }
    SUBCASE("stream") {
        std::ostringstream os;
        os << cstring_N;
        REQUIRE(os.str() == content);
    }
}

TEST_CASE("CSTRING_0") {
    constexpr auto empty = ::nameof::string_view("");
    constexpr auto cstring_0 = ::nameof::cstring<empty.size()>(empty);

    static_assert(std::is_convertible_v<const decltype(cstring_0)&, ::nameof::string_view>);
    static_assert(!std::is_convertible_v<decltype(cstring_0), ::nameof::string_view>);
    static_assert(std::is_constructible_v<::nameof::string, const decltype(cstring_0)&>);
    static_assert(!std::is_convertible_v<const decltype(cstring_0)&, ::nameof::string>);

    SUBCASE("construction") {
        REQUIRE(cstring_0 == empty);

        constexpr auto copy = ::nameof::cstring<cstring_0.size()>(cstring_0);
        REQUIRE(copy == empty);
    }
    SUBCASE("size_checking") {
        REQUIRE(cstring_0.empty() == true);
        REQUIRE(cstring_0.size() == empty.size());
        REQUIRE(cstring_0.length() == empty.size());
    }
    SUBCASE("compare") {
        REQUIRE(cstring_0.compare(empty) == 0);
        REQUIRE(cstring_0.compare("different") < 0);

        REQUIRE((cstring_0 > "different") == false);
        REQUIRE(("different" > cstring_0) == true);
        REQUIRE((cstring_0 < "different") == true);
        REQUIRE((cstring_0 >= "") == true);
        REQUIRE(("" >= cstring_0) == true);
        REQUIRE((cstring_0 <= "") == true);
        REQUIRE(("" <= cstring_0) == true);
        REQUIRE((cstring_0 >= "different") == false);
        REQUIRE(("different" <= cstring_0) == false);
        REQUIRE((cstring_0 <= "different") == true);
        REQUIRE(("different" >= cstring_0) == true);
        REQUIRE((cstring_0 == "different") == false);
        REQUIRE((cstring_0 != "different") == true);
    }
    SUBCASE("iterators") {
        REQUIRE(cstring_0.begin() == cstring_0.end());
        REQUIRE(cstring_0.cbegin() == cstring_0.cend());
        REQUIRE(cstring_0.rbegin() == cstring_0.rend());
        REQUIRE(cstring_0.crbegin() == cstring_0.crend());
        REQUIRE(::nameof::string{cstring_0.rbegin(), cstring_0.rend()}.empty());
    }
    SUBCASE("retrieval") {
        REQUIRE(cstring_0.str() == empty);
        REQUIRE(::nameof::string{cstring_0} == empty);
        REQUIRE(static_cast<::nameof::string>(cstring_0) == empty);
        REQUIRE(static_cast<::nameof::string_view>(cstring_0) == empty);
        REQUIRE(cstring_0.data() != nullptr);
        REQUIRE(cstring_0.c_str() != nullptr);
        REQUIRE(static_cast<::nameof::string_view>(cstring_0).data() == cstring_0.data());
        require_cstring_contract(cstring_0, empty);
        REQUIRE(static_cast<const char *>(cstring_0) != nullptr);
        REQUIRE(cstring_0.begin() == cstring_0.end());
        REQUIRE(cstring_0.cbegin() == cstring_0.cend());
    }
    SUBCASE("stream") {
        std::ostringstream os;
        os << cstring_0;
        REQUIRE(os.str().empty());
    }
}

TEST_CASE("CSTRING_FORMAT") {
#if defined(NAMEOF_TEST_HAS_STD_FORMAT)
  SUBCASE("std::format") {
    using short_type_t = std::remove_cv_t<std::remove_reference_t<decltype(NAMEOF_SHORT_TYPE(SomeStruct))>>;
    constexpr auto some_struct = NAMEOF_SHORT_TYPE(SomeStruct);
    constexpr auto empty = ::nameof::cstring<0>{};
    REQUIRE(std::is_default_constructible_v<std::formatter<short_type_t, char>>);
    REQUIRE(std::is_default_constructible_v<std::formatter<::nameof::cstring<0>, char>>);
    REQUIRE(std::format("{}", some_struct) == "SomeStruct");
    REQUIRE(std::format("{:^12}", some_struct) == " SomeStruct ");
    REQUIRE(std::format("{:.4}", some_struct) == "Some");
    REQUIRE(std::format("{}", empty).empty());
    REQUIRE(std::format("{:>3}", empty) == "   ");
  }
#endif

#if defined(NAMEOF_TEST_HAS_FMT_FORMAT)
  SUBCASE("fmt::format") {
    constexpr auto some_struct = NAMEOF_SHORT_TYPE(SomeStruct);
    constexpr auto empty = ::nameof::cstring<0>{};
    REQUIRE(fmt::format("{}", some_struct) == "SomeStruct");
    REQUIRE(fmt::format("{:^12}", some_struct) == " SomeStruct ");
    REQUIRE(fmt::format("{:.4}", some_struct) == "Some");
    REQUIRE(fmt::format("{}", empty).empty());
    REQUIRE(fmt::format("{:>3}", empty) == "   ");
  }
#endif
}

TEST_CASE("NAMEOF_FULL") {
  SUBCASE("variable") {
    constexpr auto full_name = NAMEOF_FULL(othervar);
    REQUIRE(full_name == "othervar");
    REQUIRE(NAMEOF_FULL(struct_var) == "struct_var");
    REQUIRE(NAMEOF_FULL(::struct_var) == "struct_var");
    REQUIRE(NAMEOF_FULL(ptr_s) == "ptr_s");
    REQUIRE(NAMEOF_FULL(color) == "color");
  }

  SUBCASE("member") {
    REQUIRE(NAMEOF_FULL(struct_var.somefield) == "somefield");
    REQUIRE(NAMEOF_FULL((&struct_var)->somefield) == "somefield");
    REQUIRE(NAMEOF_FULL(othervar.ll.field) == "field");
  }

  SUBCASE("function") {
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

  SUBCASE("enum") {
    REQUIRE(NAMEOF_FULL(Color::RED) == "RED");
    REQUIRE(NAMEOF_FULL(Color::BLUE) == "BLUE");
  }

  SUBCASE("cstring") {
    constexpr auto name = NAMEOF_FULL(SomeMethod4<int, float>);
    require_expression_cstring_contract(name, "SomeMethod4<int, float>");
    require_string_contract(::nameof::string{NAMEOF_FULL(SomeMethod4<int, float>)}, "SomeMethod4<int, float>");
  }
}

TEST_CASE("NAMEOF_RAW") {
  SUBCASE("variable") {
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

  SUBCASE("member") {
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

  SUBCASE("function") {
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

  SUBCASE("enum") {
    REQUIRE(NAMEOF_RAW(Color::RED) == "Color::RED");
    REQUIRE(NAMEOF_RAW(Color::BLUE) == "Color::BLUE");
  }

  SUBCASE("macro") {
    REQUIRE(NAMEOF_RAW(__cplusplus) == "__cplusplus");
    REQUIRE(NAMEOF_RAW(__LINE__) == "__LINE__");
    REQUIRE(NAMEOF_RAW(__FILE__) == "__FILE__");
  }

  SUBCASE("cstring") {
    constexpr auto name = NAMEOF_RAW(&struct_var);
    require_expression_cstring_contract(name, "&struct_var");
    require_string_contract(::nameof::string{NAMEOF_RAW(&struct_var)}, "&struct_var");
  }
}

#if defined(NAMEOF_ENUM_SUPPORTED)

static_assert(nameof::is_nameof_enum_supported, "nameof::nameof_enum: Unsupported compiler (https://github.com/Neargye/nameof#compiler-compatibility).");

TEST_CASE("nameof_enum") {
  SUBCASE("automatic storage") {
    constexpr Color cr = Color::RED;
    constexpr auto cr_name = nameof::nameof_enum(cr);
    Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
    const auto blue_name = nameof::nameof_enum(Color::BLUE);
    REQUIRE(cr_name == "RED");
    REQUIRE(blue_name == "BLUE");
    require_string_view_contract(blue_name, "BLUE");
    REQUIRE(nameof::nameof_enum(cm[1]) == "GREEN");
    NAMEOF_DEBUG_REQUIRE(nameof::nameof_enum(static_cast<Color>(0)).empty());

    constexpr Numbers no = Numbers::one;
    constexpr auto no_name = nameof::nameof_enum(no);
    REQUIRE(no_name == "one");
    REQUIRE(nameof::nameof_enum(Numbers::two) == "two");
    REQUIRE(nameof::nameof_enum(Numbers::three) == "three");
    NAMEOF_DEBUG_REQUIRE(nameof::nameof_enum(Numbers::many).empty());
    NAMEOF_DEBUG_REQUIRE(nameof::nameof_enum(static_cast<Numbers>(0)).empty());

    constexpr Directions dr = Directions::Right;
    constexpr auto dr_name = nameof::nameof_enum(dr);
    REQUIRE(nameof::nameof_enum(Directions::Up) == "Up");
    REQUIRE(nameof::nameof_enum(Directions::Down) == "Down");
    REQUIRE(dr_name == "Right");
    REQUIRE(nameof::nameof_enum(Directions::Left) == "Left");
    NAMEOF_DEBUG_REQUIRE(nameof::nameof_enum(static_cast<Directions>(0)).empty());

    constexpr number nt = number::three;
    constexpr auto nt_name = nameof::nameof_enum(nt);
    REQUIRE(nameof::nameof_enum(number::one) == "one");
    REQUIRE(nameof::nameof_enum(number::two) == "two");
    REQUIRE(nt_name == "three");
    NAMEOF_DEBUG_REQUIRE(nameof::nameof_enum(number::four).empty());
    NAMEOF_DEBUG_REQUIRE(nameof::nameof_enum(static_cast<number>(0)).empty());
  }

  SUBCASE("static storage") {
    constexpr Color cr = Color::RED;
    constexpr auto cr_name = nameof::nameof_enum<cr>();
    constexpr Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
    REQUIRE(cr_name == "RED");
    REQUIRE(nameof::nameof_enum<Color::BLUE>() == "BLUE");
    require_cstring_contract(nameof::nameof_enum<Color::BLUE>(), "BLUE");
    REQUIRE(nameof::nameof_enum<cm[1]>() == "GREEN");

    constexpr Numbers no = Numbers::one;
    constexpr auto no_name = nameof::nameof_enum<no>();
    REQUIRE(no_name == "one");
    REQUIRE(nameof::nameof_enum<Numbers::two>() == "two");
    REQUIRE(nameof::nameof_enum<Numbers::three>() == "three");
    REQUIRE(nameof::nameof_enum<Numbers::many>() == "many");

    constexpr Directions dr = Directions::Right;
    constexpr auto dr_name = nameof::nameof_enum<dr>();
    REQUIRE(nameof::nameof_enum<Directions::Up>() == "Up");
    REQUIRE(nameof::nameof_enum<Directions::Down>() == "Down");
    REQUIRE(dr_name == "Right");
    REQUIRE(nameof::nameof_enum<Directions::Left>() == "Left");

    constexpr number nt = number::three;
    constexpr auto nt_name = nameof::nameof_enum<nt>();
    REQUIRE(nameof::nameof_enum<number::one>() == "one");
    REQUIRE(nameof::nameof_enum<number::two>() == "two");
    REQUIRE(nt_name == "three");
    REQUIRE(nameof::nameof_enum<number::four>() == "four");

    auto blue_view = std::string_view{};
    blue_view = nameof::nameof_enum<Color::BLUE>();
    require_string_view_uses_cstring_storage(blue_view, nameof::detail::enum_name_v<Color, Color::BLUE>, "BLUE");

    constexpr auto invalid_color = static_cast<Color>(0);
    auto empty_view = std::string_view{};
    empty_view = nameof::nameof_enum<invalid_color>();
    require_string_view_uses_cstring_storage(empty_view, nameof::detail::enum_name_v<Color, invalid_color>, "");
  }

  SUBCASE("empty result") {
    const auto empty_name = nameof::nameof_enum(static_cast<Color>(0));
    REQUIRE(empty_name.empty());
    require_string_view_contract(empty_name, "");
  }
}

TEST_CASE("nameof_enum_flag") {
  constexpr AnimalFlags af = AnimalFlags::HasClaws;
  auto af_name = nameof::nameof_enum_flag(af);
  const auto empty_af_name = nameof::nameof_enum_flag(static_cast<AnimalFlags>(0));
  AnimalFlags afm[3] = {AnimalFlags::HasClaws, AnimalFlags::CanFly, AnimalFlags::EatsFish};
  REQUIRE(af_name == "HasClaws");
  require_string_contract(af_name, "HasClaws");
  REQUIRE(empty_af_name.empty());
  require_string_contract(empty_af_name, "");
  REQUIRE(nameof::nameof_enum_flag(AnimalFlags::EatsFish) == "EatsFish");
  REQUIRE(nameof::nameof_enum_flag(afm[1]) == "CanFly");
  NAMEOF_DEBUG_REQUIRE(nameof::nameof_enum_flag(static_cast<AnimalFlags>(0)).empty());
  REQUIRE(nameof::nameof_enum_flag(static_cast<AnimalFlags>(1 | 2)) == "HasClaws|CanFly");
  REQUIRE(nameof::nameof_enum_flag(static_cast<AnimalFlags>(1 | 2 | 4)) == "HasClaws|CanFly|EatsFish");
  REQUIRE(nameof::nameof_enum_flag(static_cast<AnimalFlags>(1 | 0 | 8)) == "HasClaws|Endangered");
  NAMEOF_DEBUG_REQUIRE(nameof::nameof_enum_flag(static_cast<AnimalFlags>(0)).empty());

  constexpr BigFlags bf = BigFlags::A;
  auto bf_name = nameof::nameof_enum_flag(bf);
  BigFlags bfm[3] = {BigFlags::A, BigFlags::B, BigFlags::C};
  REQUIRE(bf_name == "A");
  REQUIRE(nameof::nameof_enum_flag(BigFlags::C) == "C");
  REQUIRE(nameof::nameof_enum_flag(bfm[1]) == "B");
  NAMEOF_DEBUG_REQUIRE(nameof::nameof_enum_flag(static_cast<BigFlags>(0)).empty());
  NAMEOF_DEBUG_REQUIRE(nameof::nameof_enum_flag(static_cast<BigFlags>(1 | 2)).empty());
  REQUIRE(nameof::nameof_enum_flag(static_cast<BigFlags>(1 | (static_cast<std::uint64_t>(0x1) << 20))) == "A|B");
  REQUIRE(nameof::nameof_enum_flag(static_cast<BigFlags>(1 | (static_cast<std::uint64_t>(0x1) << 20) | (static_cast<std::uint64_t>(0x1) << 63))) == "A|B|D");
  REQUIRE(nameof::nameof_enum_flag(static_cast<BigFlags>(1 | 0 | (static_cast<std::uint64_t>(0x1) << 40))) == "A|C");
  REQUIRE(nameof::nameof_enum_flag(static_cast<BigFlags>(1 | 0 | (static_cast<std::uint64_t>(0x1) << 40))) == "A|C");
  REQUIRE(nameof::nameof_enum_flag(static_cast<BigFlags>((static_cast<std::uint64_t>(0x1) << 63) | 1)) == "A|D");
  NAMEOF_DEBUG_REQUIRE(nameof::nameof_enum_flag(static_cast<BigFlags>(2)).empty());
  NAMEOF_DEBUG_REQUIRE(nameof::nameof_enum_flag(static_cast<BigFlags>((static_cast<std::uint64_t>(0x1) << 63) | 2)).empty());
}

TEST_CASE("NAMEOF_ENUM") {
  constexpr Color cr = Color::RED;
  constexpr auto cr_name = NAMEOF_ENUM(cr);
  Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
  const auto blue_name = NAMEOF_ENUM(Color::BLUE);
  REQUIRE(cr_name == "RED");
  REQUIRE(blue_name == "BLUE");
  require_string_view_contract(blue_name, "BLUE");
  REQUIRE(NAMEOF_ENUM(cm[1]) == "GREEN");
  NAMEOF_DEBUG_REQUIRE(NAMEOF_ENUM(static_cast<Color>(0)).empty());

  constexpr Numbers no = Numbers::one;
  constexpr auto no_name = NAMEOF_ENUM(no);
  REQUIRE(no_name == "one");
  REQUIRE(NAMEOF_ENUM(Numbers::two) == "two");
  REQUIRE(NAMEOF_ENUM(Numbers::three) == "three");
  NAMEOF_DEBUG_REQUIRE(NAMEOF_ENUM(Numbers::many).empty());
  NAMEOF_DEBUG_REQUIRE(NAMEOF_ENUM(static_cast<Numbers>(0)).empty());

  constexpr Directions dr = Directions::Right;
  constexpr auto dr_name = NAMEOF_ENUM(dr);
  REQUIRE(NAMEOF_ENUM(Directions::Up) == "Up");
  REQUIRE(NAMEOF_ENUM(Directions::Down) == "Down");
  REQUIRE(dr_name == "Right");
  REQUIRE(NAMEOF_ENUM(Directions::Left) == "Left");
  NAMEOF_DEBUG_REQUIRE(NAMEOF_ENUM(static_cast<Directions>(0)).empty());

  constexpr number nt = number::three;
  constexpr auto nt_name = NAMEOF_ENUM(nt);
  REQUIRE(NAMEOF_ENUM(number::one) == "one");
  REQUIRE(NAMEOF_ENUM(number::two) == "two");
  REQUIRE(nt_name == "three");
  NAMEOF_DEBUG_REQUIRE(NAMEOF_ENUM(number::four).empty());
  NAMEOF_DEBUG_REQUIRE(NAMEOF_ENUM(static_cast<number>(0)).empty());
}

TEST_CASE("NAMEOF_ENUM_CONST") {
  constexpr Color cr = Color::RED;
  constexpr auto cr_name = NAMEOF_ENUM_CONST(cr);
  constexpr Color cm[3] = {Color::RED, Color::GREEN, Color::BLUE};
  REQUIRE(cr_name == "RED");
  REQUIRE(NAMEOF_ENUM_CONST(Color::BLUE) == "BLUE");
  REQUIRE(NAMEOF_ENUM_CONST(cm[1]) == "GREEN");

  constexpr Numbers no = Numbers::one;
  constexpr auto no_name = NAMEOF_ENUM_CONST(no);
  REQUIRE(no_name == "one");
  REQUIRE(NAMEOF_ENUM_CONST(Numbers::two) == "two");
  REQUIRE(NAMEOF_ENUM_CONST(Numbers::three) == "three");
  REQUIRE(NAMEOF_ENUM_CONST(Numbers::many) == "many");

  constexpr Directions dr = Directions::Right;
  constexpr auto dr_name = NAMEOF_ENUM_CONST(dr);
  REQUIRE(NAMEOF_ENUM_CONST(Directions::Up) == "Up");
  REQUIRE(NAMEOF_ENUM_CONST(Directions::Down) == "Down");
  REQUIRE(dr_name == "Right");
  REQUIRE(NAMEOF_ENUM_CONST(Directions::Left) == "Left");

  constexpr number nt = number::three;
  constexpr auto nt_name = NAMEOF_ENUM_CONST(nt);
  REQUIRE(NAMEOF_ENUM_CONST(number::one) == "one");
  REQUIRE(NAMEOF_ENUM_CONST(number::two) == "two");
  REQUIRE(nt_name == "three");
  REQUIRE(NAMEOF_ENUM_CONST(number::four) == "four");

  auto green_view = std::string_view{};
  green_view = NAMEOF_ENUM_CONST(Color::GREEN);
  require_string_view_uses_cstring_storage(green_view, nameof::detail::enum_name_v<Color, Color::GREEN>, "GREEN");
}

TEST_CASE("NAMEOF_ENUM_FLAG") {
  constexpr AnimalFlags af = AnimalFlags::HasClaws;
  auto af_name = NAMEOF_ENUM_FLAG(af);
  const auto empty_af_name = NAMEOF_ENUM_FLAG(static_cast<AnimalFlags>(0));
  AnimalFlags afm[3] = {AnimalFlags::HasClaws, AnimalFlags::CanFly, AnimalFlags::EatsFish};
  REQUIRE(af_name == "HasClaws");
  require_string_contract(af_name, "HasClaws");
  REQUIRE(empty_af_name.empty());
  require_string_contract(empty_af_name, "");
  REQUIRE(NAMEOF_ENUM_FLAG(afm[1]) == "CanFly");
  REQUIRE(NAMEOF_ENUM_FLAG(AnimalFlags::EatsFish) == "EatsFish");
  REQUIRE(NAMEOF_ENUM_FLAG(AnimalFlags::Endangered) == "Endangered");
  NAMEOF_DEBUG_REQUIRE(NAMEOF_ENUM_FLAG(static_cast<AnimalFlags>(0)).empty());
  REQUIRE(NAMEOF_ENUM_FLAG(static_cast<AnimalFlags>(1 | 2)) == "HasClaws|CanFly");
  REQUIRE(NAMEOF_ENUM_FLAG(static_cast<AnimalFlags>(1 | 2 | 4)) == "HasClaws|CanFly|EatsFish");
  REQUIRE(NAMEOF_ENUM_FLAG(static_cast<AnimalFlags>(1 | 0 | 8)) == "HasClaws|Endangered");
  NAMEOF_DEBUG_REQUIRE(NAMEOF_ENUM_FLAG(static_cast<AnimalFlags>(0)).empty());

  constexpr BigFlags bf = BigFlags::A;
  auto bf_name = NAMEOF_ENUM_FLAG(bf);
  BigFlags bfm[3] = {BigFlags::A, BigFlags::B, BigFlags::C};
  REQUIRE(bf_name == "A");
  REQUIRE(NAMEOF_ENUM_FLAG(bfm[1]) == "B");
  REQUIRE(NAMEOF_ENUM_FLAG(BigFlags::C) == "C");
  REQUIRE(NAMEOF_ENUM_FLAG(BigFlags::D) == "D");
  NAMEOF_DEBUG_REQUIRE(NAMEOF_ENUM_FLAG(static_cast<BigFlags>(0)).empty());
  NAMEOF_DEBUG_REQUIRE(NAMEOF_ENUM_FLAG(static_cast<BigFlags>(1 | 2)).empty());
  REQUIRE(NAMEOF_ENUM_FLAG(static_cast<BigFlags>(1 | (static_cast<std::uint64_t>(0x1) << 20))) == "A|B");
  REQUIRE(NAMEOF_ENUM_FLAG(static_cast<BigFlags>(1 | (static_cast<std::uint64_t>(0x1) << 20) | (static_cast<std::uint64_t>(0x1) << 63))) == "A|B|D");
  REQUIRE(NAMEOF_ENUM_FLAG(static_cast<BigFlags>((static_cast<std::uint64_t>(0x1) << 63) | 1)) == "A|D");
  NAMEOF_DEBUG_REQUIRE(NAMEOF_ENUM_FLAG(static_cast<BigFlags>(2)).empty());
  NAMEOF_DEBUG_REQUIRE(NAMEOF_ENUM_FLAG(static_cast<BigFlags>((static_cast<std::uint64_t>(0x1) << 63) | 2)).empty());
}

TEST_CASE("nameof_enum_or") {
  OutOfRange low = OutOfRange::too_low;
  OutOfRange high = OutOfRange::too_high;
  auto low_name = nameof::nameof_enum_or(low, "-121");
  auto high_name = nameof::nameof_enum_or(high, "121");
  auto entirely_out_of_range_name = nameof::nameof_enum_or(EntirelyOutOfRange::too_low, "fallback");
  constexpr OutOfRange oor[] = {OutOfRange::too_high, OutOfRange::too_low};
  REQUIRE(low_name == "-121");
  REQUIRE(high_name == "121");
  REQUIRE(entirely_out_of_range_name == "fallback");
  require_string_contract(low_name, "-121");
  require_string_contract(high_name, "121");
  require_string_contract(entirely_out_of_range_name, "fallback");
  REQUIRE(nameof::nameof_enum_or(oor[0], "121") == "121");

  constexpr auto fallback = ::nameof::cstring<8>{"fallback"};
  constexpr auto empty_fallback = ::nameof::cstring<0>{};
  const auto fallback_name = nameof::nameof_enum_or(OutOfRange::too_low, fallback);
  const auto empty_fallback_name = nameof::nameof_enum_or(OutOfRange::too_high, empty_fallback);
  REQUIRE(fallback_name == "fallback");
  REQUIRE(empty_fallback_name.empty());
  require_string_contract(fallback_name, "fallback");
  require_string_contract(empty_fallback_name, "");
}

TEST_CASE("NAMEOF_ENUM_OR") {
  OutOfRange low = OutOfRange::too_low;
  OutOfRange high = OutOfRange::too_high;
  auto low_name = NAMEOF_ENUM_OR(low, "-121");
  auto high_name = NAMEOF_ENUM_OR(high, "121");
  constexpr OutOfRange oor[] = {OutOfRange::too_high, OutOfRange::too_low};
  REQUIRE(low_name == "-121");
  REQUIRE(high_name == "121");
  REQUIRE(NAMEOF_ENUM_OR(oor[0], "121") == "121");
  require_string_contract(low_name, "-121");
  require_string_contract(high_name, "121");
}

#endif

TEST_CASE("customize") {
#if defined(NAMEOF_ENUM_SUPPORTED)
  REQUIRE(nameof::nameof_enum(CustomEnum::custom_name) == "custom enum");
  REQUIRE(nameof::nameof_enum(CustomEnum::default_name) == "default_name");
  require_string_view_contract(nameof::nameof_enum(CustomEnum::custom_name), "custom enum");
  require_cstring_contract(nameof::nameof_enum<CustomEnum::custom_name>(), "custom enum");
  require_cstring_contract(NAMEOF_ENUM_CONST(CustomEnum::custom_name), "custom enum");
#endif

#if defined(NAMEOF_TYPE_SUPPORTED)
  require_cstring_contract(nameof::nameof_type<CustomNameTarget>(), "CustomType");
  require_cstring_contract(nameof::nameof_full_type<CustomNameTarget>(), "CustomType");
  require_cstring_contract(nameof::nameof_short_type<CustomNameTarget>(), "CustomType");
  require_cstring_contract(NAMEOF_TYPE(CustomNameTarget), "CustomType");
#endif

#if defined(NAMEOF_MEMBER_SUPPORTED)
  require_cstring_contract(nameof::nameof_member<&CustomNameTarget::member>(), "custom_member");
  require_cstring_contract(NAMEOF_MEMBER(&CustomNameTarget::member), "custom_member");
#endif

#if defined(NAMEOF_POINTER_SUPPORTED)
  require_cstring_contract(nameof::nameof_pointer<&custom_pointer_value>(), "custom_pointer");
  require_cstring_contract(NAMEOF_POINTER(&custom_pointer_value), "custom_pointer");
#endif
}

TEST_CASE("static cstring api contract") {
#if defined(NAMEOF_ENUM_SUPPORTED)
  static_assert(std::is_lvalue_reference_v<decltype(nameof::nameof_enum<Color::BLUE>())>);
  static_assert(std::is_lvalue_reference_v<decltype(NAMEOF_ENUM_CONST(Color::BLUE))>);
  require_static_cstring_api_contract(nameof::nameof_enum<Color::BLUE>(), "BLUE");
  require_static_cstring_api_contract(NAMEOF_ENUM_CONST(Color::BLUE), "BLUE");
#endif

#if defined(NAMEOF_TYPE_SUPPORTED)
  static_assert(std::is_lvalue_reference_v<decltype(nameof::nameof_type<SomeStruct>())>);
  static_assert(std::is_lvalue_reference_v<decltype(nameof::nameof_full_type<SomeStruct&>())>);
  static_assert(std::is_lvalue_reference_v<decltype(nameof::nameof_short_type<SomeStruct>())>);
  static_assert(std::is_lvalue_reference_v<decltype(NAMEOF_TYPE(SomeStruct))>);
  static_assert(std::is_lvalue_reference_v<decltype(NAMEOF_FULL_TYPE(SomeStruct&))>);
  static_assert(std::is_lvalue_reference_v<decltype(NAMEOF_SHORT_TYPE(SomeStruct))>);
  static_assert(std::is_lvalue_reference_v<decltype(NAMEOF_TYPE_EXPR(struct_var))>);
  static_assert(std::is_lvalue_reference_v<decltype(NAMEOF_FULL_TYPE_EXPR(ref_s))>);
  static_assert(std::is_lvalue_reference_v<decltype(NAMEOF_SHORT_TYPE_EXPR(struct_var))>);

  require_static_cstring_api_contract(nameof::nameof_type<SomeStruct>(), NAMEOF_TYPE(SomeStruct));
  require_static_cstring_api_contract(nameof::nameof_full_type<SomeStruct&>(), NAMEOF_FULL_TYPE(SomeStruct&));
  require_static_cstring_api_contract(nameof::nameof_short_type<SomeStruct>(), "SomeStruct");
  require_static_cstring_api_contract(NAMEOF_TYPE(SomeStruct), NAMEOF_TYPE(SomeStruct));
  require_static_cstring_api_contract(NAMEOF_FULL_TYPE(SomeStruct&), NAMEOF_FULL_TYPE(SomeStruct&));
  require_static_cstring_api_contract(NAMEOF_SHORT_TYPE(SomeStruct), "SomeStruct");
  require_static_cstring_api_contract(NAMEOF_TYPE_EXPR(struct_var), NAMEOF_TYPE(SomeStruct));
  require_static_cstring_api_contract(NAMEOF_FULL_TYPE_EXPR(ref_s), NAMEOF_FULL_TYPE(SomeStruct&));
  require_static_cstring_api_contract(NAMEOF_SHORT_TYPE_EXPR(struct_var), "SomeStruct");
#endif

#if defined(NAMEOF_MEMBER_SUPPORTED)
  static_assert(std::is_lvalue_reference_v<decltype(nameof::nameof_member<&SomeStruct::somefield>())>);
  static_assert(std::is_lvalue_reference_v<decltype(NAMEOF_MEMBER(&SomeStruct::somefield))>);
  require_static_cstring_api_contract(nameof::nameof_member<&SomeStruct::somefield>(), "somefield");
  require_static_cstring_api_contract(NAMEOF_MEMBER(&SomeStruct::somefield), "somefield");
#endif

#if defined(NAMEOF_POINTER_SUPPORTED)
  static_assert(std::is_lvalue_reference_v<decltype(nameof::nameof_pointer<&someglobalconstvariable>())>);
  static_assert(std::is_lvalue_reference_v<decltype(NAMEOF_POINTER(&someglobalconstvariable))>);
  require_static_cstring_api_contract(nameof::nameof_pointer<&someglobalconstvariable>(), "someglobalconstvariable");
  require_static_cstring_api_contract(NAMEOF_POINTER(&someglobalconstvariable), "someglobalconstvariable");
#endif
}

#if defined(NAMEOF_TYPE_SUPPORTED)

static_assert(nameof::is_nameof_type_supported, "nameof::nameof_type: Unsupported compiler (https://github.com/Neargye/nameof#compiler-compatibility).");

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

  using SomeStructName = nameof::detail::identity<SomeStruct>;
  const auto storage_view = static_cast<::nameof::string_view>(nameof::detail::type_name_v<SomeStructName>);
  auto type_view = std::string_view{};
  type_view = nameof::nameof_type<SomeStruct>();
  REQUIRE(type_view == storage_view);
  REQUIRE(type_view.data() == storage_view.data());
  require_cstring_contract(nameof::nameof_type<SomeStruct>(), storage_view);
  require_cstring_contract(nameof::detail::type_name_v<SomeStructName>, storage_view);
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

  using SomeStructReferenceName = nameof::detail::identity<SomeStruct&>;
  const auto storage_view = static_cast<::nameof::string_view>(nameof::detail::type_name_v<SomeStructReferenceName>);
  auto type_view = std::string_view{};
  type_view = nameof::nameof_full_type<SomeStruct&>();
  REQUIRE(type_view == storage_view);
  REQUIRE(type_view.data() == storage_view.data());
  require_cstring_contract(nameof::nameof_full_type<SomeStruct&>(), storage_view);
  require_cstring_contract(nameof::detail::type_name_v<SomeStructReferenceName>, storage_view);
}

TEST_CASE("nameof::nameof_short_type") {
  constexpr auto type_name = nameof::nameof_short_type<decltype(struct_var)>();
  REQUIRE(type_name == "SomeStruct");
  REQUIRE(nameof::nameof_short_type<decltype(ref_s)>() == "SomeStruct");
  REQUIRE(nameof::nameof_short_type<SomeStruct>() == "SomeStruct");
  REQUIRE(nameof::nameof_short_type<SomeStruct &>() == "SomeStruct");
  REQUIRE(nameof::nameof_short_type<const SomeStruct volatile>() == "SomeStruct");

  REQUIRE(nameof::nameof_short_type<SomeClass<int>>() == "SomeClass");
  REQUIRE(nameof::nameof_short_type<const SomeClass<int> volatile>() == "SomeClass");

  REQUIRE(nameof::nameof_short_type<decltype(othervar)>() == "Long");
  REQUIRE(nameof::nameof_short_type<Long>() == "Long");
  REQUIRE(nameof::nameof_short_type<Long::LL>() == "LL");

  REQUIRE(nameof::nameof_short_type<Color>() == "Color");

  using SomeStructName = nameof::detail::identity<SomeStruct>;
  auto short_type_view = std::string_view{};
  short_type_view = nameof::nameof_short_type<SomeStruct>();
  require_string_view_uses_cstring_storage(short_type_view, nameof::detail::short_type_name_v<SomeStructName>, "SomeStruct");
  require_cstring_contract(nameof::nameof_short_type<Long::LL>(), "LL");
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

  using SomeStructName = nameof::detail::identity<SomeStruct>;
  const auto storage_view = static_cast<::nameof::string_view>(nameof::detail::type_name_v<SomeStructName>);
  std::string_view type_view = NAMEOF_TYPE(SomeStruct);
  require_string_view_uses_cstring_storage(type_view, NAMEOF_TYPE(SomeStruct), storage_view);
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

  using SomeStructName = nameof::detail::identity<SomeStruct>;
  const auto storage_view = static_cast<::nameof::string_view>(nameof::detail::type_name_v<SomeStructName>);
  std::string_view type_view = NAMEOF_TYPE_EXPR(struct_var);
  require_string_view_uses_cstring_storage(type_view, NAMEOF_TYPE_EXPR(struct_var), storage_view);
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

  using SomeStructReferenceName = nameof::detail::identity<SomeStruct&>;
  const auto storage_view = static_cast<::nameof::string_view>(nameof::detail::type_name_v<SomeStructReferenceName>);
  std::string_view type_view = NAMEOF_FULL_TYPE(SomeStruct&);
  require_string_view_uses_cstring_storage(type_view, NAMEOF_FULL_TYPE(SomeStruct&), storage_view);
}

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

  using SomeStructReferenceName = nameof::detail::identity<SomeStruct&>;
  const auto storage_view = static_cast<::nameof::string_view>(nameof::detail::type_name_v<SomeStructReferenceName>);
  std::string_view type_view = NAMEOF_FULL_TYPE_EXPR(ref_s);
  require_string_view_uses_cstring_storage(type_view, NAMEOF_FULL_TYPE_EXPR(ref_s), storage_view);
}

TEST_CASE("NAMEOF_SHORT_TYPE") {
  constexpr auto type_name = NAMEOF_SHORT_TYPE(decltype(struct_var));
  REQUIRE(type_name == "SomeStruct");
  REQUIRE(NAMEOF_SHORT_TYPE(decltype(ref_s)) == "SomeStruct");
  REQUIRE(NAMEOF_SHORT_TYPE(SomeStruct) == "SomeStruct");
  REQUIRE(NAMEOF_SHORT_TYPE(SomeStruct &) == "SomeStruct");
  REQUIRE(NAMEOF_SHORT_TYPE(const SomeStruct volatile) == "SomeStruct");

  REQUIRE(NAMEOF_SHORT_TYPE(SomeClass<int>) == "SomeClass");
  REQUIRE(NAMEOF_SHORT_TYPE(const SomeClass<int> volatile) == "SomeClass");

  REQUIRE(NAMEOF_SHORT_TYPE(decltype(othervar)) == "Long");
  REQUIRE(NAMEOF_SHORT_TYPE(Long) == "Long");
  REQUIRE(NAMEOF_SHORT_TYPE(Long::LL) == "LL");

  REQUIRE(NAMEOF_SHORT_TYPE(Color) == "Color");

  using SomeStructName = nameof::detail::identity<SomeStruct>;
  auto short_type_view = std::string_view{};
  short_type_view = NAMEOF_SHORT_TYPE(SomeStruct);
  require_string_view_uses_cstring_storage(short_type_view, nameof::detail::short_type_name_v<SomeStructName>, "SomeStruct");
}

TEST_CASE("NAMEOF_SHORT_TYPE_EXPR") {
  constexpr auto type_name = NAMEOF_SHORT_TYPE_EXPR(struct_var);
  REQUIRE(type_name == "SomeStruct");
  REQUIRE(NAMEOF_SHORT_TYPE_EXPR(ref_s) == "SomeStruct");

  REQUIRE(NAMEOF_SHORT_TYPE_EXPR(othervar) == "Long");
  REQUIRE(NAMEOF_SHORT_TYPE_EXPR(othervar.ll) == "LL");
  REQUIRE(NAMEOF_SHORT_TYPE_EXPR(othervar.ll.field) == "int");

  REQUIRE(NAMEOF_SHORT_TYPE_EXPR(Color::RED) == "Color");

  REQUIRE(NAMEOF_SHORT_TYPE_EXPR(std::declval<const SomeClass<int>>()) == "SomeClass");
}

#endif

#if defined(NAMEOF_TYPE_RTTI_SUPPORTED) && NAMEOF_TYPE_RTTI_SUPPORTED

TEST_CASE("NAMEOF_TYPE_RTTI") {
  TestRtti::Derived object;
  TestRtti::Base* ptr = &object;
  const TestRtti::Base& const_ref = *ptr;
  volatile TestRtti::Base& volatile_ref = *ptr;
  volatile const TestRtti::Base& cv_ref = *ptr;
  const auto ptr_name = NAMEOF_TYPE_RTTI(*ptr);
  const auto const_ref_name = NAMEOF_TYPE_RTTI(const_ref);
  const auto volatile_ref_name = NAMEOF_TYPE_RTTI(volatile_ref);
  const auto cv_ref_name = NAMEOF_TYPE_RTTI(cv_ref);
#if defined(__clang__) && !defined(_MSC_VER)
  require_string_contract(ptr_name, "TestRtti::Derived");
  require_string_contract(const_ref_name, "TestRtti::Derived");
  require_string_contract(volatile_ref_name, "TestRtti::Derived");
  require_string_contract(cv_ref_name, "TestRtti::Derived");
#elif defined(_MSC_VER)
  require_string_contract(ptr_name, "struct TestRtti::Derived");
  require_string_contract(const_ref_name, "struct TestRtti::Derived");
  require_string_contract(volatile_ref_name, "struct TestRtti::Derived");
  require_string_contract(cv_ref_name, "struct TestRtti::Derived");
#elif defined(__GNUC__)
  require_string_contract(ptr_name, "TestRtti::Derived");
  require_string_contract(const_ref_name, "TestRtti::Derived");
  require_string_contract(volatile_ref_name, "TestRtti::Derived");
  require_string_contract(cv_ref_name, "TestRtti::Derived");
#endif
}

TEST_CASE("NAMEOF_FULL_TYPE_RTTI") {
  TestRtti::Derived object;
  TestRtti::Base* ptr = &object;
  const TestRtti::Base& const_ref = *ptr;
  volatile TestRtti::Base& volatile_ref = *ptr;
  volatile const TestRtti::Base& cv_ref = *ptr;
  const auto ptr_name = NAMEOF_FULL_TYPE_RTTI(*ptr);
  const auto const_ref_name = NAMEOF_FULL_TYPE_RTTI(const_ref);
  const auto volatile_ref_name = NAMEOF_FULL_TYPE_RTTI(volatile_ref);
  const auto cv_ref_name = NAMEOF_FULL_TYPE_RTTI(cv_ref);
#if defined(__clang__) && !defined(_MSC_VER)
  require_string_contract(ptr_name, "TestRtti::Derived&");
  require_string_contract(const_ref_name, "const TestRtti::Derived&");
  require_string_contract(volatile_ref_name, "volatile TestRtti::Derived&");
  require_string_contract(cv_ref_name, "volatile const TestRtti::Derived&");
#elif defined(_MSC_VER)
  require_string_contract(ptr_name, "struct TestRtti::Derived&");
  require_string_contract(const_ref_name, "const struct TestRtti::Derived&");
  require_string_contract(volatile_ref_name, "volatile struct TestRtti::Derived&");
  require_string_contract(cv_ref_name, "volatile const struct TestRtti::Derived&");
#elif defined(__GNUC__)
  require_string_contract(ptr_name, "TestRtti::Derived&");
  require_string_contract(const_ref_name, "const TestRtti::Derived&");
  require_string_contract(volatile_ref_name, "volatile TestRtti::Derived&");
  require_string_contract(cv_ref_name, "volatile const TestRtti::Derived&");
#endif
}

TEST_CASE("NAMEOF_SHORT_TYPE_RTTI") {
  TestRtti::Derived object;
  TestRtti::Base* ptr = &object;
  const TestRtti::Base& const_ref = *ptr;
  volatile TestRtti::Base& volatile_ref = *ptr;
  volatile const TestRtti::Base& cv_ref = *ptr;

  require_string_contract(NAMEOF_SHORT_TYPE_RTTI(*ptr), "Derived");
  require_string_contract(NAMEOF_SHORT_TYPE_RTTI(const_ref), "Derived");
  require_string_contract(NAMEOF_SHORT_TYPE_RTTI(volatile_ref), "Derived");
  require_string_contract(NAMEOF_SHORT_TYPE_RTTI(cv_ref), "Derived");
}

#if __has_include(<cxxabi.h>)
TEST_CASE("NAMEOF_TYPE_RTTI demangle success") {
  const auto name = typeid(TestRtti::Derived).name();
  require_string_contract(nameof::detail::nameof_type_rtti<TestRtti::Derived>(name), "TestRtti::Derived");
  require_string_contract(nameof::detail::nameof_full_type_rtti<const TestRtti::Derived&>(name), "const TestRtti::Derived&");
  require_string_contract(nameof::detail::nameof_short_type_rtti<TestRtti::Derived>(name), "Derived");
}

TEST_CASE("NAMEOF_TYPE_RTTI demangle fallback") {
  require_string_contract(nameof::detail::nameof_type_rtti<void>("not_a_mangled_type"), "not_a_mangled_type");
  require_string_contract(nameof::detail::nameof_full_type_rtti<const int&>("not_a_mangled_type"), "const not_a_mangled_type&");
  require_string_contract(nameof::detail::nameof_short_type_rtti<void>("some_namespace::not_a_mangled_type"), "not_a_mangled_type");
}
#endif

#endif

#if defined(NAMEOF_MEMBER_SUPPORTED) && NAMEOF_MEMBER_SUPPORTED

struct StructMemberInitializationUsingNameof {
  std::string teststringfield = std::string{nameof::nameof_member<&StructMemberInitializationUsingNameof::teststringfield>()};
};

struct StructWithNonConstexprDestructor {
  ~StructWithNonConstexprDestructor() {}
  int somefield;
};

TEST_CASE("NAMEOF_MEMBER") {
  REQUIRE(NAMEOF_MEMBER(&SomeStruct::somefield) == "somefield");
  REQUIRE(NAMEOF_MEMBER(&SomeStruct::SomeMethod1) == "SomeMethod1");
  REQUIRE(NAMEOF_MEMBER(&Long::LL::field) == "field");
  constexpr auto member_ptr = &SomeStruct::somefield;
  REQUIRE(NAMEOF_MEMBER(member_ptr) == "somefield");
  REQUIRE(NAMEOF_MEMBER(&StructMemberInitializationUsingNameof::teststringfield) == "teststringfield");
  REQUIRE(NAMEOF_MEMBER(&StructWithNonConstexprDestructor::somefield) == "somefield");

  auto member_view = std::string_view{};
  member_view = NAMEOF_MEMBER(&NameOfTest::some_member);
  require_string_view_uses_cstring_storage(member_view, nameof::detail::member_name_v<&NameOfTest::some_member>, "some_member");
}

TEST_CASE("nameof_member") {
  REQUIRE(nameof::nameof_member<&SomeStruct::somefield>() == "somefield");
  REQUIRE(nameof::nameof_member<&SomeStruct::SomeMethod1>() == "SomeMethod1");
  REQUIRE(nameof::nameof_member<&Long::LL::field>() == "field");
  constexpr auto member_ptr = &SomeStruct::somefield;
  REQUIRE(nameof::nameof_member<member_ptr>() == "somefield");
  REQUIRE(nameof::nameof_member<&StructMemberInitializationUsingNameof::teststringfield>() == "teststringfield");
  REQUIRE(nameof::nameof_member<&StructWithNonConstexprDestructor::somefield>() == "somefield");
  require_cstring_contract(nameof::nameof_member<&SomeStruct::SomeMethod1>(), "SomeMethod1");
}

TEST_CASE("string_view_regression") {
  std::string_view name{};
  name = nameof::nameof_member<&NameOfTest::some_member>();
  REQUIRE(name == "some_member");
  require_string_view_uses_cstring_storage(name, nameof::detail::member_name_v<&NameOfTest::some_member>, "some_member");
}

#endif

#if defined(NAMEOF_POINTER_SUPPORTED) && NAMEOF_POINTER_SUPPORTED

void somefunction() {}

TEST_CASE("NAMEOF_POINTER") {
  REQUIRE(NAMEOF_POINTER(&SomeStruct::somestaticfield) == "somestaticfield");
  REQUIRE(NAMEOF_POINTER(&SomeStruct::someotherstaticfield) == "someotherstaticfield");
  REQUIRE(NAMEOF_POINTER(static_cast<const char*>(nullptr)) == "nullptr");
  REQUIRE(NAMEOF_POINTER(static_cast<int***>(nullptr)) == "nullptr");
  constexpr auto global_ptr = &someglobalvariable;
  REQUIRE(NAMEOF_POINTER(global_ptr) == "someglobalvariable");
  REQUIRE(NAMEOF_POINTER(&someglobalconstvariable) == "someglobalconstvariable");
  REQUIRE(NAMEOF_POINTER(&somefunction) == "somefunction");

  auto pointer_view = std::string_view{};
  pointer_view = NAMEOF_POINTER(&someglobalconstvariable);
  require_string_view_uses_cstring_storage(pointer_view, nameof::detail::pointer_name_v<&someglobalconstvariable>, "someglobalconstvariable");
}

TEST_CASE("nameof_pointer") {
  REQUIRE(nameof::nameof_pointer<&SomeStruct::somestaticfield>() == "somestaticfield");
  REQUIRE(nameof::nameof_pointer<&SomeStruct::someotherstaticfield>() == "someotherstaticfield");
  REQUIRE(nameof::nameof_pointer<static_cast<const char*>(nullptr)>() == "nullptr");
  REQUIRE(nameof::nameof_pointer<static_cast<int***>(nullptr)>() == "nullptr");
  constexpr auto global_ptr = &someglobalvariable;
  REQUIRE(nameof::nameof_pointer<global_ptr>() == "someglobalvariable");
  REQUIRE(nameof::nameof_pointer<&someglobalconstvariable>() == "someglobalconstvariable");
  REQUIRE(nameof::nameof_pointer<&somefunction>() == "somefunction");

  auto pointer_view = std::string_view{};
  pointer_view = nameof::nameof_pointer<&somefunction>();
  require_string_view_uses_cstring_storage(pointer_view, nameof::detail::pointer_name_v<&somefunction>, "somefunction");

  auto nullptr_view = std::string_view{};
  nullptr_view = nameof::nameof_pointer<static_cast<const char*>(nullptr)>();
  require_string_view_uses_cstring_storage(nullptr_view, nameof::detail::pointer_name_v<static_cast<const char*>(nullptr)>, "nullptr");
}

#endif
