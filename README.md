[![GitHub releases](https://img.shields.io/github/release/Neargye/nameof.svg)](https://github.com/Neargye/nameof/releases)
[![Conan package](https://img.shields.io/badge/Conan-package-blueviolet)](https://conan.io/center/recipes/nameof)
[![vcpkg package](https://img.shields.io/badge/vcpkg-package-blueviolet)](https://github.com/microsoft/vcpkg/tree/master/ports/nameof)
[![License](https://img.shields.io/github/license/Neargye/nameof.svg)](LICENSE)
[![Compiler explorer](https://img.shields.io/badge/compiler_explorer-online-blue.svg)](https://godbolt.org/z/s_ecko)

# Nameof C++

A header-only C++17 library that provides nameof macros and functions for obtaining the name of a variable, type, function, macro, or enum.

## Documentation

* [Reference](doc/reference.md)
* [Limitations](doc/limitations.md)
* [Custom names](example/example_custom_name.cpp)
* [Integration](#integration)

## [Features & Examples](example/example.cpp)

* **Nameof**

  ```cpp
  // Name of variable.
  NAMEOF(somevar) -> "somevar"

  // Name of member variable.
  NAMEOF(person.address.zip_code) -> "zip_code"

  // Name of function.
  NAMEOF(foo<int, float>()) -> "foo"

  // Name of member function.
  NAMEOF(somevar.some_method()) -> "some_method"
  NAMEOF(somevar.some_method<int>()) -> "some_method"

  // Name of macro.
  NAMEOF(__LINE__) -> "__LINE__"

  // Obtains full name of variable, function, macro.
  NAMEOF_FULL(somevar.some_method<int>()) -> "some_method<int>"

  // Obtains raw expression text.
  NAMEOF_RAW(somevar.some_method<int>()) -> "somevar.some_method<int>()"
  ```

* **Nameof enum**

  ```cpp
  enum class Color { RED = 1, BLUE = 2, GREEN = 4 };

  auto color = Color::RED;
  // Name of enum value.
  NAMEOF_ENUM(color) -> "RED"
  nameof::nameof_enum(color) -> "RED"

  // Name of enum value known at compile time.
  // This version has a lower compile-time cost and is not restricted by the enum_range limitation.
  NAMEOF_ENUM_CONST(Color::GREEN) -> "GREEN"
  nameof::nameof_enum<Color::GREEN>() -> "GREEN"

  enum AnimalFlags { HasClaws = 1, CanFly = 2, EatsFish = 4 };

  // Enum flag value to string.
  auto flag = static_cast<AnimalFlags>(CanFly | EatsFish);
  NAMEOF_ENUM_FLAG(flag) -> "CanFly|EatsFish"
  nameof::nameof_enum_flag(flag, '$') -> "CanFly$EatsFish"

  // Obtains name of enum value or default value if no name is available.
  NAMEOF_ENUM_OR(Color::GREEN, "none") -> "GREEN"
  NAMEOF_ENUM_OR((Color)0, "none") -> "none"
  nameof::nameof_enum_or(Color::GREEN, "none") -> "GREEN"
  ```

* **Nameof type**

  Exact type spellings are compiler-specific.

  ```cpp
  const my::detail::SomeClass<int>& var_ref = var;
  // Name of variable type.
  NAMEOF_TYPE_EXPR(var_ref) -> "my::detail::SomeClass<int>"
  nameof::nameof_type<decltype(var_ref)>() -> "my::detail::SomeClass<int>"
  NAMEOF_FULL_TYPE_EXPR(var_ref) -> "const my::detail::SomeClass<int>&"
  nameof::nameof_full_type<decltype(var_ref)>() -> "const my::detail::SomeClass<int>&"
  NAMEOF_SHORT_TYPE_EXPR(var_ref) -> "SomeClass"
  nameof::nameof_short_type<decltype(var_ref)>() -> "SomeClass"

  using T = const my::detail::SomeClass<int>&;
  // Name of type.
  NAMEOF_TYPE(T) -> "my::detail::SomeClass<int>"
  nameof::nameof_type<T>() -> "my::detail::SomeClass<int>"
  NAMEOF_FULL_TYPE(T) -> "const my::detail::SomeClass<int>&"
  nameof::nameof_full_type<T>() -> "const my::detail::SomeClass<int>&"
  NAMEOF_SHORT_TYPE(T) -> "SomeClass"
  nameof::nameof_short_type<T>() -> "SomeClass"

  my::detail::Derived derived;
  my::detail::Base* ptr = &derived;
  // Obtains type name using RTTI.
  NAMEOF_TYPE_RTTI(*ptr) -> "my::detail::Derived"
  NAMEOF_FULL_TYPE_RTTI(*ptr) -> "my::detail::Derived&"
  NAMEOF_SHORT_TYPE_RTTI(*ptr) -> "Derived"

  struct A {
    int this_is_the_name;
  };
  // Obtains name of member.
  NAMEOF_MEMBER(&A::this_is_the_name) -> "this_is_the_name"
  nameof::nameof_member<&A::this_is_the_name>() -> "this_is_the_name"

  int someglobalvariable = 0;
  const int someglobalconstvariable = 42;
  // Obtains name of a function, a global or class static variable.
  NAMEOF_POINTER(&someglobalconstvariable) -> "someglobalconstvariable"
  nameof::nameof_pointer<&someglobalconstvariable>() -> "someglobalconstvariable"

  constexpr auto global_ptr = &someglobalvariable;
  NAMEOF_POINTER(global_ptr) -> "someglobalvariable"
  nameof::nameof_pointer<global_ptr>() -> "someglobalvariable"
  ```

## Integration

* **Single header:** add [nameof.hpp](include/nameof.hpp) to your project.

* **CMake package:**

  ```cmake
  find_package(nameof CONFIG REQUIRED)
  target_link_libraries(your_target PRIVATE nameof::nameof)
  ```

* **vcpkg:** install the [nameof package](https://github.com/microsoft/vcpkg/tree/master/ports/nameof) with `vcpkg install nameof`.

* **[Conan](https://www.conan.io/):** add `nameof/x.y.z` to the `requires` section, where `x.y.z` is the required release version.

* **AUR:** install `nameof` with an AUR helper, for example `yay -S nameof`.

* **[CPM](https://github.com/cpm-cmake/CPM.cmake):**

  ```cmake
  CPMAddPackage(
      NAME nameof
      GITHUB_REPOSITORY Neargye/nameof
      GIT_TAG vx.y.z # Replace `x.y.z` with the release version you want to use.
  )
  ```

## Compiler compatibility

nameof requires C++17 or newer and supports:

* Clang/LLVM >= 5
* GCC >= 9
* Visual Studio >= 2019

Enum APIs automatically use [C++26 standard reflection](doc/limitations.md#c26-standard-reflection) when available.

See the [reference](doc/reference.md#compiler-compatibility) for feature-specific requirements.

## Licensed under the [MIT License](LICENSE)
