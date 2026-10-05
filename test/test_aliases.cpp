// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.

#include <string>
#include <string_view>
#include <sstream>
#include <iomanip>
#include <type_traits>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

struct MyString {
  MyString() : str{} {} // required
  MyString(const char* s) : str{s} {} // required
  MyString(const char* s, std::size_t l) : str{s, l} {} // required
  bool empty() const { return str.empty(); } // required
  MyString& append(std::size_t count, char c) { str.append(count, c); return *this; } // required
  MyString& append(const char* s, std::size_t count) { str.append(s, count); return *this; } // required
  MyString& append(const MyString& s) { str.append(s.str); return *this; } // required

  std::size_t size() const { return str.size(); }
  int compare(const char* s) const { return str.compare(s); }

 private:
  std::string str;
};

struct MyStringView {
  using value_type = char; // required
  static constexpr auto npos = std::string_view::npos; // required

  constexpr MyStringView() noexcept : str{} {} // required
  constexpr MyStringView(const char* s) noexcept : str{s} {} // required
  constexpr MyStringView(const char* s, std::size_t size) noexcept : str{s, size} {} // required
  constexpr MyStringView(const MyStringView&) noexcept = default;
  constexpr MyStringView& operator=(const MyStringView&) = delete;
  constexpr bool empty() const noexcept { return str.empty(); } // required
  constexpr std::size_t size() const noexcept { return str.size(); } // required
  constexpr const char* data() const noexcept { return str.data(); } // required
  constexpr const char& operator[](std::size_t i) const noexcept { return str[i]; } // required
  constexpr void remove_prefix(std::size_t n) noexcept { str.remove_prefix(n); } // required
  constexpr void remove_suffix(std::size_t n) noexcept { str.remove_suffix(n); } // required
  constexpr int compare(MyStringView s) const noexcept { return str.compare(s.str); } // required

  constexpr int compare(const char* s) const noexcept { return str.compare(s); }

 private:
  std::string_view str;

  constexpr MyStringView(std::string_view s) noexcept : str{s} {}
};

#define NAMEOF_USING_ALIAS_STRING using string = MyString;
#define NAMEOF_USING_ALIAS_STRING_VIEW using string_view = MyStringView;

#include <nameof.hpp>

enum class Color { RED = 1, GREEN = 2, BLUE = 4 };

static_assert(std::is_copy_constructible_v<MyStringView>);
static_assert(!std::is_copy_assignable_v<MyStringView>);
static_assert(!std::is_move_assignable_v<MyStringView>);
static_assert(noexcept(nameof::nameof_enum(Color::RED)));
static_assert(!noexcept(nameof::nameof_enum_flag(Color::RED)));

struct AliasStruct {
  int member = 0;
  static int static_member;
};

int AliasStruct::static_member = 0;

TEST_CASE("string") {
  auto cr = nameof::nameof_enum_flag(Color::RED);
  REQUIRE_FALSE(cr.empty());
  REQUIRE(cr.compare("RED") == 0);

  auto crg = nameof::nameof_enum_flag(static_cast<Color>(1 | 2));
  REQUIRE_FALSE(crg.empty());
  REQUIRE(crg.compare("RED|GREEN") == 0);

  auto cn = nameof::nameof_enum_flag(Color{0});
  REQUIRE(cn.empty());
  REQUIRE(cn.size() == 0);

  auto qualified_name = nameof::detail::full_type_name<const volatile int&&>(nameof::string{"int"});
  REQUIRE(qualified_name.compare("volatile const int&&") == 0);
#if defined(NAMEOF_TYPE_RTTI_SUPPORTED)
  void (* const callback)() = nullptr;
  const auto pointer_name = NAMEOF_FULL_TYPE_RTTI(callback);
  REQUIRE(pointer_name.compare(NAMEOF_FULL_TYPE_EXPR(callback).c_str()) == 0);
#endif
}

TEST_CASE("cstring stream formatting with aliases") {
  std::ostringstream os;
  os << std::setfill('.') << std::setw(6) << NAMEOF_TYPE(int);
  REQUIRE(os.str() == "...int");
  REQUIRE(os.width() == 0);
  os << '|';
  REQUIRE(os.str() == "...int|");

  std::wostringstream wos;
  wos << std::left << std::setfill(L'.') << std::setw(6) << NAMEOF_TYPE(int);
  REQUIRE(wos.str() == L"int...");
  REQUIRE(wos.width() == 0);
  wos << L'|';
  REQUIRE(wos.str() == L"int...|");
}

TEST_CASE("string_view") {
  auto cr = nameof::nameof_enum(Color::RED);
  REQUIRE_FALSE(cr.empty());
  REQUIRE(cr.compare("RED") == 0);

  auto cn = nameof::nameof_enum(Color{0});
  REQUIRE(cn.empty());
  REQUIRE(cn.size() == 0);
}

TEST_CASE("string enum_or") {
  auto red = nameof::nameof_enum_or(Color::RED, "fallback");
  REQUIRE_FALSE(red.empty());
  REQUIRE(red.compare("RED") == 0);

  auto fallback = nameof::nameof_enum_or(Color{0}, "fallback");
  REQUIRE_FALSE(fallback.empty());
  REQUIRE(fallback.compare("fallback") == 0);

  auto macro_fallback = NAMEOF_ENUM_OR(Color{0}, "macro fallback");
  REQUIRE_FALSE(macro_fallback.empty());
  REQUIRE(macro_fallback.compare("macro fallback") == 0);

  auto empty_fallback = NAMEOF_ENUM_OR(Color{0}, "");
  REQUIRE(empty_fallback.empty());
}

TEST_CASE("short type names") {
#if defined(NAMEOF_TYPE_SUPPORTED)
  constexpr nameof::string_view arithmetic_name = NAMEOF_SHORT_TYPE(const volatile unsigned int&);
  constexpr nameof::string_view null_name = NAMEOF_SHORT_TYPE(std::nullptr_t);
  static_assert(arithmetic_name.compare("unsigned int") == 0);
  static_assert(null_name.compare("nullptr_t") == 0);
#endif
#if defined(NAMEOF_TYPE_RTTI_SUPPORTED)
  const unsigned int value = 0;
  REQUIRE(NAMEOF_SHORT_TYPE_RTTI(value).compare("unsigned int") == 0);
  REQUIRE(NAMEOF_SHORT_TYPE_RTTI(nullptr).compare("nullptr_t") == 0);
#endif
}

TEST_CASE("static cstring api") {
  std::ostringstream stream;
  stream << nameof::cstring<3>{"abc"};
  REQUIRE(stream.str() == "abc");

#if defined(NAMEOF_ENUM_SUPPORTED)
  constexpr auto enum_name = NAMEOF_ENUM_CONST(Color::RED);
  REQUIRE(enum_name.compare("RED") == 0);

  auto enum_string = nameof::string{enum_name};
  REQUIRE(enum_string.compare("RED") == 0);

  constexpr auto empty_name = nameof::cstring<0>{};
  auto empty_string = nameof::string{empty_name};
  REQUIRE(empty_string.empty());
#endif

#if defined(NAMEOF_TYPE_SUPPORTED)
  constexpr auto type_name = NAMEOF_TYPE(Color);
  static_assert(NAMEOF_TYPE(int) == NAMEOF_TYPE(const int&));
  static_assert(NAMEOF_TYPE(int) != NAMEOF_TYPE(long));
  static_assert(NAMEOF_TYPE(int) < NAMEOF_TYPE(long));
  static_assert(NAMEOF_TYPE(int) <= NAMEOF_TYPE(const int&));
  static_assert(NAMEOF_TYPE(long) > NAMEOF_TYPE(int));
  static_assert(NAMEOF_TYPE(long) >= NAMEOF_TYPE(long));
#  if defined(_MSC_VER) && !defined(__clang__)
  REQUIRE(type_name.compare("enum Color") == 0);
#  else
  REQUIRE(type_name.compare("Color") == 0);
#  endif
#endif

#if defined(NAMEOF_MEMBER_SUPPORTED)
  constexpr auto member_name = NAMEOF_MEMBER(&AliasStruct::member);
  REQUIRE(member_name.compare("member") == 0);
#endif

#if defined(NAMEOF_POINTER_SUPPORTED)
  constexpr auto pointer_name = NAMEOF_POINTER(&AliasStruct::static_member);
  REQUIRE(pointer_name.compare("static_member") == 0);
#endif
}

TEST_CASE("static bool enum names with aliases") {
#if defined(NAMEOF_ENUM_SUPPORTED)
  enum class EmptyBool : bool {};
  enum class FalseBool : bool { Off = false };
  enum class TrueBool : bool { On = true };
  static_assert(nameof::nameof_enum<static_cast<EmptyBool>(false)>().empty());
  static_assert(nameof::nameof_enum<static_cast<EmptyBool>(true)>().empty());
  static_assert(nameof::nameof_enum<static_cast<FalseBool>(true)>().empty());
  static_assert(nameof::nameof_enum<static_cast<TrueBool>(false)>().empty());
  static_assert(NAMEOF_ENUM_CONST(static_cast<FalseBool>(true)).empty());
  static_assert(NAMEOF_ENUM_CONST(static_cast<TrueBool>(false)).empty());
  static_assert(nameof::nameof_enum<FalseBool::Off>().compare("Off") == 0);
  static_assert(nameof::nameof_enum<TrueBool::On>().compare("On") == 0);
#endif
}
