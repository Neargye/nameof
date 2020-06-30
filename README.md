```text
 _   _                             __    _____
| \ | |                           / _|  / ____|_     _
|  \| | __ _ _ __ ___   ___  ___ | |_  | |   _| |_ _| |_
| . ` |/ _` | '_ ` _ \ / _ \/ _ \|  _| | |  |_   _|_   _|
| |\  | (_| | | | | | |  __/ (_) | |   | |____|_|   |_|
|_| \_|\__,_|_| |_| |_|\___|\___/|_|    \_____|
```

[![Github releases](https://img.shields.io/github/release/Neargye/nameof.svg)](https://github.com/Neargye/nameof/releases)
[![Conan package](https://img.shields.io/badge/Conan-package-blueviolet)](https://conan.io/center/nameof/0.9.4)
[![Vcpkg package](https://img.shields.io/badge/Vcpkg-package-blueviolet)](https://github.com/microsoft/vcpkg/tree/master/ports/nameof)
[![License](https://img.shields.io/github/license/Neargye/nameof.svg)](LICENSE)
[![Build status](https://travis-ci.org/Neargye/nameof.svg?branch=master)](https://travis-ci.org/Neargye/nameof)
[![Build status](https://ci.appveyor.com/api/projects/status/yq5fk0d9mwljbubt/branch/master?svg=true)](https://ci.appveyor.com/project/Neargye/nameof/branch/master)
[![Codacy badge](https://api.codacy.com/project/badge/Grade/1d06f3f07afe4f34acd29c0c8efa830b)](https://www.codacy.com/app/Neargye/nameof?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Neargye/nameof&amp;utm_campaign=Badge_Grade)
[![Try online](https://img.shields.io/badge/try-online-blue.svg)](https://wandbox.org/permlink/VL44h47DKALBE5Oh)
[![Compiler explorer](https://img.shields.io/badge/compiler_explorer-online-blue.svg)](https://godbolt.org/z/s_ecko)

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

## Documentation

* [Reference](doc/reference.md)
* [Limitations](doc/limitations.md)
* [Integration](#Integration)

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
  enum class Color { RED = 1, BLUE = 2, GREEN = 4 };

  auto color = Color::RED;
  // Name of enum variable.
  NAMEOF_ENUM(color) -> "RED"
  nameof::nameof_enum(color) -> "RED"

  // Static storage enum variable to string.
  // This version is much lighter on the compile times and is not restricted to the enum_range limitation.
  NAMEOF_CONST_ENUM(Color::GREEN) -> "GREEN"
  nameof::nameof_enum<Color::GREEN>() -> "GREEN"

  // Enum flag variable to string.
  NAMEOF_ENUM_FLAG(Color::GREEN | Color::BLUE) -> "GREEN|BLUE"
  nameof::nameof_enum_flag<Color::GREEN>() -> "GREEN|BLUE"
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

* Before use, read the [limitations](doc/limitations.md) of functionality.

## Integration

You should add required file [nameof.hpp](include/nameof.hpp).

If you are using [vcpkg](https://github.com/Microsoft/vcpkg/) on your project for external dependencies, then you can use the [nameof package](https://github.com/microsoft/vcpkg/tree/master/ports/nameof).

If you are using [Conan](https://www.conan.io/) to manage your dependencies, merely add `nameof/x.y.z` to your conan's requires, where `x.y.z` is the release version you want to use.

Alternatively, you can use something like [CPM](https://github.com/TheLartians/CPM) which is based on CMake's `Fetch_Content` module.

```cmake
CPMAddPackage(
    NAME nameof
    GITHUB_REPOSITORY Neargye/nameof
    GIT_TAG x.y.z # Where `x.y.z` is the release version you want to use.
)
```

## Compiler compatibility

* Clang/LLVM >= 5
* MSVC++ >= 14.11 / Visual Studio >= 2017
* Xcode >= 10
* GCC >= 7 (GCC >= 9 for NAMEOF_ENUM)

## Licensed under the [MIT License](LICENSE)
