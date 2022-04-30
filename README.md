[![Stand With Ukraine](https://raw.githubusercontent.com/vshymanskyy/StandWithUkraine/main/banner2-direct.svg)](https://bit.ly/3OMysM8)

```text
 _   _                             __    _____
| \ | |                           / _|  / ____|_     _
|  \| | __ _ _ __ ___   ___  ___ | |_  | |   _| |_ _| |_
| . ` |/ _` | '_ ` _ \ / _ \/ _ \|  _| | |  |_   _|_   _|
| |\  | (_| | | | | | |  __/ (_) | |   | |____|_|   |_|
|_| \_|\__,_|_| |_| |_|\___|\___/|_|    \_____|
```

[![Github releases](https://img.shields.io/github/release/Neargye/nameof.svg)](https://github.com/Neargye/nameof/releases)
[![Conan package](https://img.shields.io/badge/Conan-package-blueviolet)](https://conan.io/center/nameof)
[![Vcpkg package](https://img.shields.io/badge/Vcpkg-package-blueviolet)](https://github.com/microsoft/vcpkg/tree/master/ports/nameof)
[![License](https://img.shields.io/github/license/Neargye/nameof.svg)](LICENSE)
[![Try online](https://img.shields.io/badge/try-online-blue.svg)](https://wandbox.org/permlink/PBBzVKlbMIfC3WOk)
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
  NAMEOF_ENUM_CONST(Color::GREEN) -> "GREEN"
  nameof::nameof_enum<Color::GREEN>() -> "GREEN"

  // Enum flag variable to string.
  NAMEOF_ENUM_FLAG(Color::GREEN | Color::BLUE) -> "GREEN|BLUE"
  nameof::nameof_enum_flag<Color::GREEN>() -> "GREEN|BLUE"
  ```

* Nameof type

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
  NAMEOF_TYPE(T) ->"my::detail::SomeClass<int>"
  nameof::nameof_type<T>() -> "my::detail::SomeClass<int>"
  NAMEOF_FULL_TYPE(T) -> "const my::detail::SomeClass<int>&"
  nameof::nameof_full_type<T>() -> "const my::detail::SomeClass<int>&"
  NAMEOF_SHORT_TYPE(T) -> "SomeClass"
  nameof::nameof_short_type<T>() -> "SomeClass"

  my::detail::Base* ptr = new my::detail::Derived();
  // Name of type, using rtti.
  NAMEOF_TYPE_RTTI(*ptr) -> "my::detail::Derived"
  NAMEOF_FULL_TYPE_RTTI(*ptr) -> "volatile const my::detail::Derived&"
  NAMEOF_SHORT_TYPE_RTTI(*ptr) -> "Derived"
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

Archlinux users can install `nameof` by package manager AUR, using the following command: `yay -S nameof`.

Alternatively, you can use something like [CPM](https://github.com/TheLartians/CPM) which is based on CMake's `Fetch_Content` module.

```cmake
CPMAddPackage(
    NAME nameof
    GITHUB_REPOSITORY Neargye/nameof
    GIT_TAG x.y.z # Where `x.y.z` is the release version you want to use.
)
```

## Compiler compatibility

* Clang/LLVM >= 6
* MSVC++ >= 14.11 / Visual Studio >= 2017
* Xcode >= 10
* GCC >= 7 (GCC >= 9 for NAMEOF_ENUM)

## Licensed under the [MIT License](LICENSE)
