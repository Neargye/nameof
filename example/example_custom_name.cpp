// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
// SPDX-License-Identifier: MIT
// Copyright (c) 2020 - 2022 Daniil Goncharov <neargye@gmail.com>.
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

#include <iostream>

#include <nameof.hpp>

enum class Color { RED = -10, BLUE = 0, GREEN = 10 };
enum class Numbers { One, Two, Three };

#if defined(NAMEOF_ENUM_SUPPORTED)
// Сustom definitions of names for enum.
// Specialization of `enum_name` must be injected in `namespace nameof::customize`.
template <>
constexpr std::string_view nameof::customize::enum_name<Color>(Color value) noexcept {
  switch (value) {
    case Color::RED:
      return "the red color";
    case Color::BLUE:
      return "The BLUE";
    case Color::GREEN:
      return {}; // Empty string for default value.
  }
  return {}; // Empty string for unknow value.
}

// Сustom definitions of names for enum.
// Specialization of `enum_name` must be injected in `namespace nameof::customize`.
template <>
constexpr std::string_view nameof::customize::enum_name<Numbers>(Numbers value) noexcept {
  switch (value) {
    case Numbers::One:
      return "the one";
    default:
      return {}; // Empty string for default or unknow value.
  }
}
#endif

// Сustom definitions of names for type.
// Specialization of `type_name` must be injected in `namespace nameof::customize`.
template <>
constexpr std::string_view nameof::customize::type_name<Color>() noexcept {
  return "The Color";
}

class a1_test {};
class a2_test {};

// Сustom definitions of names for type.
// Specialization of `type_name` must be injected in `namespace nameof::customize`.
template <>
constexpr std::string_view nameof::customize::type_name<a1_test>() noexcept {
  return "Animal";
}

int main() {
#if defined(NAMEOF_ENUM_SUPPORTED)
  std::cout << nameof::nameof_enum(Color::RED) << std::endl; // 'the red color'
  std::cout << nameof::nameof_enum(Color::BLUE) << std::endl; // 'The BLUE'
  std::cout << nameof::nameof_enum(Color::GREEN) << std::endl; // 'GREEN'

  std::cout << nameof::nameof_enum(Numbers::One) << std::endl; // 'the one'
  std::cout << nameof::nameof_enum(Numbers::Two) << std::endl; // 'Two'
  std::cout << nameof::nameof_enum(Numbers::Three) << std::endl; // 'Three'
#endif

  std::cout << nameof::nameof_type<Color>() << std::endl; // 'The Color'
  std::cout << nameof::nameof_type<Numbers>() << std::endl; // 'Numbers'
  std::cout << nameof::nameof_type<a1_test>() << std::endl; // 'Animal'
  std::cout << nameof::nameof_type<a2_test>() << std::endl; // 'a2_test'

  return 0;
}
