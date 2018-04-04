# nameof() c++11

C++ alternative to [nameof](https://docs.microsoft.com/en-us/dotnet/csharp/language-reference/keywords/nameof) operator in C#.

Branch | Linux/OSX | Windows
-------|-----------|---------
master |[![Build Status](https://travis-ci.org/Neargye/nameof.svg?branch=master)](https://travis-ci.org/Neargye/nameof)|[![Build status](https://ci.appveyor.com/api/projects/status/yq5fk0d9mwljbubt/branch/master?svg=true)](https://ci.appveyor.com/project/Neargye/nameof/branch/master)

Used to obtain the simple name of a variable, type, member or function and etc.
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

## [Example & Key Use Cases](https://github.com/Neargye/nameof/blob/master/example/example.cpp)

* Name of a variable, member or function and etc

```cpp
NAMEOF(someVar) -> "someVar"
NAMEOF(someVar.SomeField) -> "SomeField"

NAMEOF(someVar.SomeMethod1()) -> "SomeMethod1()"
NAMEOF(&SomeStruct::SomeMethod2) -> "SomeMethod2"
```

* Name of enum

```cpp
NAMEOF(SomeEnum::RED) -> "RED"
NAMEOF(SomeEnum::GREEN) -> "GREEN"
```

* Name of type

```cpp
NAMEOF(int[]) -> "int[]"
NAMEOF(std::string) -> "string"
```

* Constexpr

```cpp
void f() {
  int i;
  constexpr auto constexpr_work_fine = NAMEOF(i); -> "i"
}
```

* Compilation check

```cpp
void f() {
  int i;
  NAMEOF(i); -> "i"
  NAMEOF(iii); -> error identifier "iii" is undefined
  NAMEOF(std::stringgg) -> error namespace "std" has no member "stringgg"
}
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

* If you need to get the fully-qualified name, you could use the NAMEOF_FULL().

```cpp
NAMEOF_FULL(someVar.SomeField) -> "someVar.SomeField"
NAMEOF_FULL(&SomeStruct::SomeMethod2) -> "&SomeStruct::SomeMethod2"
NAMEOF_FULL(std::string) -> "std::string"
```

* By default nameof work with RTTI, but can work without RTTI. If compiling without RTTI, you need use the NAMEOF_TYPE() for get name of type.

```cpp
NAMEOF_TYPE(int[]) -> "int[]"
NAMEOF_TYPE(std::string) -> "string"

NAMEOF_TYPE_FULL(std::string) -> "std::string"
```

## License MIT