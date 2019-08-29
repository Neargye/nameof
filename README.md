```text
 _   _                             __    _____
| \ | |                           / _|  / ____|_     _
|  \| | __ _ _ __ ___   ___  ___ | |_  | |   _| |_ _| |_
| . ` |/ _` | '_ ` _ \ / _ \/ _ \|  _| | |  |_   _|_   _|
| |\  | (_| | | | | | |  __/ (_) | |   | |____|_|   |_|
|_| \_|\__,_|_| |_| |_|\___|\___/|_|    \_____|
```

[![Github releases](https://img.shields.io/github/release/Neargye/nameof.svg)](https://github.com/Neargye/nameof/releases)
[![Conan package](https://img.shields.io/badge/Conan-package-blueviolet)](https://bintray.com/neargye/conan-packages/nameof:neargye)
[![Vcpkg package](https://img.shields.io/badge/Vcpkg-package-blueviolet)](https://github.com/microsoft/vcpkg/tree/master/ports/nameof)
[![License](https://img.shields.io/github/license/Neargye/nameof.svg)](LICENSE)
[![Build status](https://travis-ci.org/Neargye/nameof.svg?branch=master)](https://travis-ci.org/Neargye/nameof)
[![Build status](https://ci.appveyor.com/api/projects/status/yq5fk0d9mwljbubt/branch/master?svg=true)](https://ci.appveyor.com/project/Neargye/nameof/branch/master)
[![Codacy badge](https://api.codacy.com/project/badge/Grade/1d06f3f07afe4f34acd29c0c8efa830b)](https://www.codacy.com/app/Neargye/nameof?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Neargye/nameof&amp;utm_campaign=Badge_Grade)
[![Try online](https://img.shields.io/badge/try-online-blue.svg)](https://wandbox.org/permlink/XjaqDqqHOGJT56HE)

# Nameof C++

Header-only C++17 library provides nameof macros and functions to simply obtain the name of a variable, type, function, macro, and enum.

## Features

* C++17
* Header-only
* Dependency-free
* Compile-time
* Name of variable, member variable
* Name of type, variable type
* Name of function, member function
* Name of enum, enum variable
* Name of macro
* Enum to string

## [Examples](example/example.cpp)

* Nameof
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
  NAMEOF(NAMEOF(structvar)) -> "NAMEOF"
  ```

* Nameof enum
  ```cpp
  auto color = Color::RED;
  // Name of enum variable.
  NAMEOF_ENUM(color) -> "RED"
  nameof::nameof_enum(color) -> "RED"

  // Static storage enum variable to string.
  // This version is much lighter on the compile times and is not restricted to the enum_range limitation.
  NAMEOF_CONST_ENUM(Color::GREEN) -> "GREEN"
  nameof::nameof_enum<Color::GREEN>() -> "GREEN"
  ```

* Nameof type
  ```cpp
  using T = const int&;
  T var = 42;
  // Name of variable type.
  NAMEOF_TYPE_EXPR(var) -> "int"
  NAMEOF_FULL_TYPE_EXPR(var) -> "const int&"
  nameof::nameof_type<decltype(var)>() -> "int"
  nameof::nameof_full_type<decltype(var)>() -> "const int&"

  // Name of type.
  NAMEOF_TYPE(T) -> "int"
  NAMEOF_FULL_TYPE(T) -> "const int&"
  nameof::nameof_type<T>() -> "int"
  nameof::nameof_full_type<T>() -> "const int&"
  ```

* Compile-time
  ```cpp
  constexpr auto somevar_name = NAMEOF(somevar);
  // somevar_name -> "somevar"
  constexpr auto color_name = NAMEOF_ENUM(Color::BLUE); // or nameof::nameof_enum(Color::BLUE)
  // color_name -> "BLUE"
  constexpr auto var_type_name = NAMEOF_TYPE_EXPR(var); // or nameof::nameof_type<decltype(var)>()
  // var_type_name -> "int"
  constexpr auto type_name = NAMEOF_TYPE(T); // or nameof::nameof_type<T>()
  // type_name -> "int"
  ```

## Remarks

* Nameof returns `std::string_view`. If argument does not have name, returns empty string.

* Nameof expression argument are identified, but are not evaluated.

* Nameof type returns compiler-specific type name. In all cases, reference and cv-qualifiers are ignored by `nameof_type` (that is, `nameof_type<const T&>() == nameof_type<T>()`). If you need detailed name of full type, use `nameof_full_type`.

* If you need name with template suffix, use NAMEOF_FULL.
  ```cpp
  // Full name of template function.
  NAMEOF_FULL(foo<int, float>()) -> "foo<int, float>"

  // Full name of template member function.
  NAMEOF_FULL(somevar.some_method<int>()) -> "some_method<int>"
  ```

* If you need raw fully-qualified name, use NAMEOF_RAW.
  ```cpp
  NAMEOF_RAW(::somevar.somefield) -> "::somevar.somefield"
  NAMEOF_RAW(&some_class::some_method<int>) -> "&some_class::some_method<int>"
  ```

* Enum value must be in range `[NAMEOF_ENUM_RANGE_MIN, NAMEOF_ENUM_RANGE_MAX]`. By default `NAMEOF_ENUM_RANGE_MIN = -128`, `NAMEOF_ENUM_RANGE_MAX = 128`.

  If need another range for all enum types by default, redefine the macro `NAMEOF_ENUM_RANGE_MIN` and `NAMEOF_ENUM_RANGE_MAX`.
  ```cpp
  #define NAMEOF_ENUM_RANGE_MIN 0
  #define NAMEOF_ENUM_RANGE_MAX 256
  #include <nameof.hpp>
  ```

  If need another range for specific enum type, add specialization `enum_range` for necessary enum type.
  ```cpp
  #include <nameof.hpp>

  enum number { one = 100, two = 200, three = 300 };

  namespace nameof {
  template <>
  struct enum_range<number> {
    static constexpr int min = 100;
    static constexpr int max = 300;
  };
  }
  ```

* Nameof enum obtains the first defined value enums, and won't work if value are aliased.
  ```cpp
  enum ShapeKind {
    ConvexBegin = 0,
    Box = 0, // Won't work.
    Sphere = 1,
    ConvexEnd = 2,
    Donut = 2, // Won't work too.
    Banana = 3,
    COUNT = 4,
  };
  // NAMEOF_ENUM(ShapeKind::Box) -> "ConvexBegin"
  // nameof::nameof_enum(ShapeKind::Box) -> "ConvexBegin"
  ```
  Work around the issue:
  ```cpp
  enum ShapeKind {
    // Convex shapes, see ConvexBegin and ConvexEnd below.
    Box = 0,
    Sphere = 1,

    // Non-convex shapes.
    Donut = 2,
    Banana = 3,

    COUNT = Banana + 1,

    // Non-reflected aliases.
    ConvexBegin = Box,
    ConvexEnd = Sphere + 1,
  };
  // NAMEOF_ENUM(ShapeKind::Box) -> "Box"
  // nameof::nameof_enum(ShapeKind::Box) -> "Box"

  // Non-reflected aliases.
  // NAMEOF_ENUM(ShapeKind::ConvexBegin) -> "Box"
  // nameof::nameof_enum(ShapeKind::ConvexBegin) -> "Box"
  ```

## Integration

You should add required file [nameof.hpp](include/nameof.hpp).

If you are using [vcpkg](https://github.com/Microsoft/vcpkg/) on your project for external dependencies, then you can use the [nameof package](https://github.com/microsoft/vcpkg/tree/master/ports/nameof).

If you are using [Conan](https://www.conan.io/) to manage your dependencies, merely add `nameof/x.y.z@neargye/stable` to your conan's requires, where `x.y.z` is the release version you want to use.

## Compiler compatibility

* Clang/LLVM >= 5
* Visual C++ >= 15.3 / Visual Studio >= 2017
* Xcode >= 9
* GCC >= 7 (GCC >= 9 for NAMEOF_ENUM)

## Licensed under the [MIT License](LICENSE)
