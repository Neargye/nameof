# nameof c++

```text
 _   _                             __    _____
| \ | |                           / _|  / ____|_     _
|  \| | __ _ _ __ ___   ___  ___ | |_  | |   _| |_ _| |_
| . ` |/ _` | '_ ` _ \ / _ \/ _ \|  _| | |  |_   _|_   _|
| |\  | (_| | | | | | |  __/ (_) | |   | |____|_|   |_|
|_| \_|\__,_|_| |_| |_|\___|\___/|_|    \_____|
```

Branch | Linux/OSX | Windows | Codacy
-------|-----------|---------|-------
master |[![Build Status](https://travis-ci.org/Neargye/nameof.svg?branch=master)](https://travis-ci.org/Neargye/nameof)|[![Build status](https://ci.appveyor.com/api/projects/status/yq5fk0d9mwljbubt/branch/master?svg=true)](https://ci.appveyor.com/project/Neargye/nameof/branch/master)|[![Codacy Badge](https://api.codacy.com/project/badge/Grade/1d06f3f07afe4f34acd29c0c8efa830b)](https://www.codacy.com/app/Neargye/nameof?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Neargye/nameof&amp;utm_campaign=Badge_Grade)

C++ alternative to [nameof](https://docs.microsoft.com/en-us/dotnet/csharp/language-reference/keywords/nameof) operator in [C#](https://en.wikipedia.org/wiki/C_Sharp_(programming_language)).

Used to obtain the simple name of a variable, type, member, function, macros and etc.
Before, you had to use string literals to refer to definitions, which is brittle when renaming code elements because tools do not know to check these string literals.

A nameof macros expression has this form:

```cpp
std::cout << NAMEOF(person.address.zip_code) << std::endl; // prints "zip_code"
```

## Features

* Simple name
* C++11
* Header-only
* Dependency-free
* Compile-time
* Compilation check

## [Example](example/example.cpp) & Key Use Cases

* Name of a variable, member or function and etc

```cpp
NAMEOF(somevar) -> "somevar"
NAMEOF(somevar.somefield) -> "somefield"
NAMEOF(SomeMethod) -> "SomeMethod"
```

* Name of enum

```cpp
NAMEOF(SomeEnum::RED) -> "RED"
NAMEOF(SomeEnum::GREEN) -> "GREEN"
```

* Name of type

```cpp
NAMEOF(volatile const int) -> "volatile const int int"
NAMEOF(std::string) -> "string"
```

* Name of macros

```cpp
NAMEOF(__LINE__) -> "__LINE__"
NAMEOF(__FILE__) -> "__FILE__"
```

* Constexpr

```cpp
constexpr auto constexpr_work_fine = NAMEOF(somevar); -> "somevar"
```

* Compilation check

```cpp
NAMEOF(std::stringgg) -> error namespace "std" has no member "stringgg"
```

* Validate parameters

```cpp
void f(char* s) {
  if (s == nullptr)
    throw std::invalid_argument(NAMEOF(s));
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
  p.name = j.at(NAMEOF(p.name));
  p.address = j.at(NAMEOF(p.address));
  p.age = j.at(NAMEOF(p.age));
}
```

* Logging

```cpp
void f() {
  Log(NAMEOF(f), " method entry");
}
```

## Remarks

* The argument expression identifies a code definition, but it is never evaluated.

* If you need to get the fully-qualified name, you could use the NAMEOF_FULL().

```cpp
NAMEOF_FULL(somevar.somefield) -> "somevar.somefield"
NAMEOF_FULL(&SomeStruct::SomeMethod) -> "&SomeStruct::SomeMethod"
NAMEOF_FULL(std::string) -> "std::string"
```

## Integration

You need to add the single required file [nameof.hpp](include/nameof.hpp), and the necessary switches to enable C++11.

## Compiler compatibility

* GCC
* Clang
* MSVC

## Licensed under the [MIT License](LICENSE)