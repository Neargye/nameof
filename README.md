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

Used to obtain the simple name of a variable, type, member, function, macros.
Before, you had to use string literals to refer to definitions, which is brittle when renaming code elements because tools do not know to check these string literals.

## Features

* C++17
* Header-only
* Dependency-free
* Compile-time
* Compilation check
* Name of variable
* Name of member
* Name of function
* Name of emum
* Name of macrose
* Name of type

## [Examples](example/example.cpp)

* A nameof macros expression has this form

```cpp
// Name of variable
NAMEOF(somevar) -> "somevar"
// Name of member
NAMEOF(person.address.zip_code) -> "zip_code"
// Name of function
NAMEOF(SomeMethod<int, float>) -> "SomeMethod"
NAMEOF_FULL(SomeMethod<int, float>) -> "SomeMethod4<int, float>"
// Name of enum
auto c = Color::RED;
NAMEOF_ENUM(c) -> "RED"
// Name of type
NAMEOF_TYPE(Color::RED) -> "Color"
NAMEOF_TYPE_T(int) -> "int"
// Name of macros
NAMEOF(__LINE__) -> "__LINE__"
```

* Constexpr

```cpp
constexpr auto cx = NAMEOF(somevar); -> "somevar"
static_assert("somevar" == cx, "Wrong name!");
```

* Compilation check

```cpp
NAMEOF_TYPE_T(std::stringgg) -> error namespace "std" has no member "stringgg"
```

* Validate parameters

```cpp
void f(char* s) {
  if (s == nullptr)
    throw std::invalid_argument(NAMEOF(s).apend("with type = ").apend(NAMEOF_TYPE(s)));
}
```

* Serialization, for example json:

```cpp
void to_json(json& j, const person& p) {
  j = json{{NAMEOF(p.name), p.name},
           {NAMEOF(p.address), p.address},
           {NAMEOF(p.age), p.age}};
}

void from_json(const json& j, person& p) {
  p.name =    j.at(NAMEOF(p.name));
  p.address = j.at(NAMEOF(p.address));
  p.age =     j.at(NAMEOF(p.age));
}
```

* Logging

```cpp
template <typename T>
void f() {
  Log(NAMEOF(f) + " method entry with T = " + NAMEOF_TYPE_T(T));
}
```

## Remarks

* Nameof return std::string_view.

* The argument expression identifies a code definition, but it is never evaluated.

* If you need raw fully-qualified name, use NAMEOF_RAW().

```cpp
NAMEOF_RAW(somevar.somefield) -> "somevar.somefield"
NAMEOF_RAW(&SomeStruct::SomeMethod) -> "&SomeStruct::SomeMethod"
```

* Instead of macros NAMEOF_ENUM, you can use the function nameof::NameofEnum().

```cpp
nameof::NameofEnum(Color::RED) -> "RED"
auto c = Color::RED;
nameof::NameofEnum(c) -> "RED"
```

* Instead of macros NAMEOF_TYPE, you can use the function nameof::NameofType<>.

```cpp
nameof::NameofType(Color::RED) -> "Color"
nameof::NameofType<decltype(Color::RED)>() -> "Color"
nameof::NameofType<int> -> "int"
```

* NAMEOF_ENUM does not work on the GCC.

```cpp
auto c = Color::RED;
NAMEOF_ENUM(c) -> "(Color)0"
```

* Spaces and Tabs ignored

```cpp
NAMEOF(   somevar   ) -> "somevar"
NAMEOF(	somevar	) -> "somevar"
```

## Integration

You should add required file [nameof.hpp](include/nameof.hpp).

## Compiler compatibility

* GCC
* Clang
* MSVC

## Licensed under the [MIT License](LICENSE)
