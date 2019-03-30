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

* Name of variable
```cpp
// Name of variable
NAMEOF(somevar) -> "somevar"
// Name of member variable
NAMEOF(person.address.zip_code) -> "zip_code"
```

* Name of function
```cpp
// Name of function
NAMEOF(foo<int, float>()) -> "foo"
NAMEOF_FULL(foo<int, float>()) -> "foo<int, float>"

// Name of member function
NAMEOF(somevar.some_method()) -> "some_method"
NAMEOF_FULL(somevar.some_method<int>()) -> "some_method<int>"
```

* Name of enum
```cpp
auto color = Color::RED;
// Name of enum variable
NAMEOF_ENUM(color) -> "RED"
// Name of enum variable
nameof::nameof_enum(color) -> "RED"

constexpr auto cx_color = Color::BLUE;
// Name of static storage enum variable
nameof::nameof_enum<cx_color>() -> "BLUE"
```

* Name of type
```cpp
using T = int;
T var = 42;
// Name of variable type
NAMEOF_TYPE(var) -> "int"
nameof::nameof_type<decltype(var)>() -> "int"
// Name of type
NAMEOF_TYPE_T(T) -> "int"
nameof::nameof_type<T>() -> "int"
```

* Name of macro
```cpp
// Name of macro
NAMEOF(__LINE__) -> "__LINE__"
```

## Remarks

* Nameof return std::string_view.

* Nameof expression arguments are identified, but do not evaluated.

* NAMEOF_ENUM supported on the GCC >= 9.

* If you need of raw fully-qualified name, use NAMEOF_RAW.
```cpp
NAMEOF_RAW(somevar.somefield) -> "somevar.somefield"
NAMEOF_RAW(&SomeStruct::SomeMethod) -> "&SomeStruct::SomeMethod"
NAMEOF_RAW(std::string) -> "std::string"
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
