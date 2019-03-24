# Nameof C++

```text
 _   _                             __    _____
| \ | |                           / _|  / ____|_     _
|  \| | __ _ _ __ ___   ___  ___ | |_  | |   _| |_ _| |_
| . ` |/ _` | '_ ` _ \ / _ \/ _ \|  _| | |  |_   _|_   _|
| |\  | (_| | | | | | |  __/ (_) | |   | |____|_|   |_|
|_| \_|\__,_|_| |_| |_|\___|\___/|_|    \_____|
```

Branch | Linux/OSX | Windows | License | Codacy
-------|-----------|---------|---------|-------
master |[![Build Status](https://travis-ci.org/Neargye/nameof.svg?branch=master)](https://travis-ci.org/Neargye/nameof)|[![Build status](https://ci.appveyor.com/api/projects/status/yq5fk0d9mwljbubt/branch/master?svg=true)](https://ci.appveyor.com/project/Neargye/nameof/branch/master)|[![License](https://img.shields.io/github/license/Neargye/nameof.svg)](LICENSE)|[![Codacy Badge](https://api.codacy.com/project/badge/Grade/1d06f3f07afe4f34acd29c0c8efa830b)](https://www.codacy.com/app/Neargye/nameof?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Neargye/nameof&amp;utm_campaign=Badge_Grade)

## What is Nameof?

Header-only C++17 library provides nameof macros and functions to obtain simple name of variable, type, function, macro, and enum.

Before, you had to use string literals to refer to definitions, which is brittle when renaming code elements because tools do not know to check these string literals.

## Features

* C++17
* Simple syntax
* Header-only
* Dependency-free
* Compile-time

## [Examples](example/example.cpp)

* Name of variable
```cpp
// Name of variable
NAMEOF(somevar) -> "somevar"
// Name of member variable
NAMEOF(person.address.zip_code) -> "zip_code"

constexpr auto cx_name = NAMEOF(somevar);
static_assert("somevar" == cx_name);
```

* Name of function
```cpp
// Name of function
NAMEOF(some_method<int, float>) -> "some_method"
NAMEOF_FULL(some_method<int, float>) -> "some_method<int, float>"
// Name of member function
NAMEOF(somevar.foo()) -> "foo"
NAMEOF_FULL(somevar.boo<int>()) -> "boo<int>"

constexpr auto cx_name = NAMEOF(somevar.foo());
static_assert("foo" == cx_name);
```

* Name of enum
```cpp
auto c = Color::RED;
// Name of enum variable
NAMEOF_ENUM(c) -> "RED"
// Name of enum variable
nameof::nameof_enum(c) -> "RED"

constexpr auto cx_color = Color::BLUE;
// Name of static storage enum variable
nameof::nameof_enum<cx_color>() -> "BLUE"
```

* Name of type
```cpp
// Name of variable type
NAMEOF_TYPE(Color::RED) -> "Color"
// Name of type
NAMEOF_TYPE_T(int) -> "int"
// Name of variable type
nameof::nameof_type<decltype(Color::RED)>() -> "Color"
// Name of type
nameof::nameof_type<int>() -> "int"

constexpr auto cx_name = NAMEOF_TYPE(Color::RED);
static_assert("Color" == cx_name);
```

* Name of macro
```cpp
// Name of macro
NAMEOF(__LINE__) -> "__LINE__"

constexpr auto cx_name = NAMEOF(__LINE__);
static_assert("__LINE__" == cx_name);
```

## Remarks

* Nameof return std::string_view.

* The argument expression identifies code definition, but it is never evaluated.

* If you need raw fully-qualified name, use NAMEOF_RAW.
```cpp
NAMEOF_RAW(somevar.somefield) -> "somevar.somefield"
NAMEOF_RAW(&SomeStruct::SomeMethod) -> "&SomeStruct::SomeMethod"
NAMEOF_RAW(std::string) -> "std::string"
```

* NAMEOF_ENUM does not work on the GCC.
```cpp
auto c = Color::RED;
NAMEOF_ENUM(c) -> "(Color)0"
nameof::nameof_enum(c) -> "(Color)0"

NAMEOF(Color::RED) -> "RED"
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
* GCC >= 7

## Licensed under the [MIT License](LICENSE)
