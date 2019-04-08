# Nameof C++

```text
 _   _                             __    _____
| \ | |                           / _|  / ____|_     _
|  \| | __ _ _ __ ___   ___  ___ | |_  | |   _| |_ _| |_
| . ` |/ _` | '_ ` _ \ / _ \/ _ \|  _| | |  |_   _|_   _|
| |\  | (_| | | | | | |  __/ (_) | |   | |____|_|   |_|
|_| \_|\__,_|_| |_| |_|\___|\___/|_|    \_____|
```

[![License](https://img.shields.io/github/license/Neargye/nameof.svg)](LICENSE)
[![Build Status](https://travis-ci.org/Neargye/nameof.svg?branch=master)](https://travis-ci.org/Neargye/nameof)
[![Build status](https://ci.appveyor.com/api/projects/status/yq5fk0d9mwljbubt/branch/master?svg=true)](https://ci.appveyor.com/project/Neargye/nameof/branch/master)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/1d06f3f07afe4f34acd29c0c8efa830b)](https://www.codacy.com/app/Neargye/nameof?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Neargye/nameof&amp;utm_campaign=Badge_Grade)
[![Try online](https://img.shields.io/badge/try-online-blue.svg)](https://wandbox.org/permlink/irgD3NwQM2ikhVMC)

## What is Nameof?

Header-only C++17 library provides nameof macros and functions to obtain simple name of variable, type, function, macro, and enum.

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
  ```

* Nameof type
  ```cpp
  using T = int;
  T var = 42;
  // Name of variable type.
  NAMEOF_VAR_TYPE(var) -> "int"
  nameof::nameof_type<decltype(var)>() -> "int"

  // Name of type.
  NAMEOF_TYPE(T) -> "int"
  nameof::nameof_type<T>() -> "int"
  ```

* Compile-time
  ```cpp
  constexpr auto somevar_name = NAMEOF(somevar);
  // somevar_name -> "somevar"
  constexpr auto color_name = NAMEOF_ENUM(Color::BLUE); // or nameof::nameof_enum(Color::BLUE)
  // color_name -> "BLUE"
  constexpr auto var_type_name = NAMEOF_VAR_TYPE(var); // or nameof::nameof_type<decltype(var)>()
  // var_type_name -> "int"
  constexpr auto type_name = NAMEOF_VAR_TYPE(T); // or nameof::nameof_type<T>()
  // type_name -> "int"
  ```

## Remarks

* Nameof returns `std::string_view`. If argument does not have name, returns empty string.

* Nameof expression argument are identified, but do not evaluated.

* Enum value must be in range `[-256, 256]`. If you need another range, add specialization enum_range for necessary enum type.
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

* If you need name with template suffix, use NAMEOF_FULL.
  ```cpp
  // Name of function.
  NAMEOF_FULL(foo<int, float>()) -> "foo<int, float>"

  // Name of member function.
  NAMEOF_FULL(somevar.some_method<int>()) -> "some_method<int>"
  ```

* If you need raw fully-qualified name, use NAMEOF_RAW.
  ```cpp
  NAMEOF_RAW(somevar.somefield) -> "somevar.somefield"
  NAMEOF_RAW(&some_class::some_method<int>) -> "&some_class::some_method<int>"
  ```

* Spaces and Tabs ignored
  ```cpp
  NAMEOF(   somevar   ) -> "somevar"
  NAMEOF(	somevar	) -> "somevar"
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

## Compiler compatibility

* Clang/LLVM >= 5
* Visual C++ >= 15.3 / Visual Studio >= 2017
* Xcode >= 9
* GCC >= 7 (GCC >= 9 for NAMEOF_ENUM)

## Licensed under the [MIT License](LICENSE)
