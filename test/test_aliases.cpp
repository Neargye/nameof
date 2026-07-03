// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2019 - 2026 Daniil Goncharov <neargye@gmail.com>.
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

#include <string>
#include <string_view>

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

  constexpr MyStringView() : str{} {} // required
  constexpr MyStringView(const char* s) : str{s} {} // required
  constexpr MyStringView(const char* s, std::size_t size) : str{s, size} {} // required
  constexpr bool empty() const { return str.empty(); } // required
  constexpr std::size_t size() const { return str.size(); } // required
  constexpr const char* data() const { return str.data(); } // required
  constexpr const char& operator[](std::size_t i) const { return str[i]; } // required
  constexpr void remove_prefix(std::size_t n) { str.remove_prefix(n); } // required
  constexpr void remove_suffix(std::size_t n) { str.remove_suffix(n); } // required
  constexpr int compare(MyStringView s) const { return str.compare(s.str); } // required

  constexpr int compare(const char* s) const { return str.compare(s); }

 private:
  std::string_view str;

  constexpr MyStringView(std::string_view s) : str{s} {}
};

#define NAMEOF_USING_ALIAS_STRING using string = MyString;
#define NAMEOF_USING_ALIAS_STRING_VIEW using string_view = MyStringView;

#include <nameof.hpp>

enum class Color { RED = 1, GREEN = 2, BLUE = 4 };

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

TEST_CASE("static cstring api") {
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
#  if defined(_MSC_VER)
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
