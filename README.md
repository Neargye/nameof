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

  constexpr auto const_color = Color::BLUE;
  // Name of static storage enum variable.
  NAMEOF_CONST_ENUM(const_color) -> "BLUE"
  nameof::nameof_enum<const_color>() -> "BLUE"
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

## Remarks

* Nameof returns `std::string_view`. If argument does not have name, returns empty string.

* Nameof expression argument are identified, but do not evaluated.

* Enum variable must be in range `(-NAMEOF_ENUM_RANGE, NAMEOF_ENUM_RANGE)`. By default `NAMEOF_ENUM_RANGE = 128`. If you need a larger range, redefine the macro `NAMEOF_ENUM_RANGE`.
  ```cpp
  #define NAMEOF_ENUM_RANGE 1028 // Redefine NAMEOF_ENUM_RANGE for larger range.
  #include <nameof.hpp>
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

## Integration

You should add required file [nameof.hpp](include/nameof.hpp).

## Compiler compatibility

* Clang/LLVM >= 5
* Visual C++ >= 15.3 / Visual Studio >= 2017
* Xcode >= 9
* GCC >= 7 (GCC >= 9 for NAMEOF_ENUM)

## Licensed under the [MIT License](LICENSE)
