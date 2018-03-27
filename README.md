# nameof() c++11

C++ alternative to [nameof](https://docs.microsoft.com/en-us/dotnet/csharp/language-reference/keywords/nameof) operator.

 Linux/OSX | Windows
-----------|---------
[![Build Status](https://travis-ci.org/Terik23/nameof.svg?branch=master)](https://travis-ci.org/Terik23/nameof)|[![Build status](https://ci.appveyor.com/api/projects/status/4tyl8lee10ckw10k/branch/master?svg=true)](https://ci.appveyor.com/project/Terik23/nameof/branch/master)

Used to obtain the simple name of a variable, type, function.
Before, you had to use string literals to refer to definitions, which is brittle when renaming code elements because tools do not know to check these string literals.

A nameof expression has this form:

```cpp
std::cout << NAMEOF(person.Address.zip_code) << std::endl; // prints "zip_code"
```

## Features

* Simple name
* C++11
* Header-only
* Dependency-free
* Compile-time
* Compilation check

## [Example & Key Use Cases](https://github.com/Terik23/nameof/blob/master/example/example.cpp)

* Name of a variable, function and etc

```cpp
NAMEOF(someVar) -> "someVar"
NAMEOF(someVar.SomeField) -> "SomeField"

NAMEOF(someVar.SomeMethod1()) -> "SomeMethod1()"
NAMEOF(&SomeStruct::SomeMethod2) -> "SomeMethod2"

NAMEOF_FUNCTION(someVar.SomeMethod1()) -> "SomeMethod1()"
NAMEOF_FUNCTION(&SomeStruct::SomeMethod2) -> "SomeMethod2"

NAMEOF_VAR(someVar) -> "someVar"
NAMEOF_VAR(someVar.SomeField) -> "SomeField"
```

* Name of type

```cpp
NAMEOF_TYPE(int[]) -> "int[]"
NAMEOF_TYPE(std::string) -> "string"
NAMEOF_TYPE(std::stringgg) -> error namespace "std" has no member "stringgg"
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
  NAMEOF_TYPE(std::stringgg) -> error namespace "std" has no member "stringgg"
}
```

* Validate parameters

```cpp
void f(char* s) {
  if (s == nullptr)
    throw std::invalid_argument(NAMEOF(s));
}
```

* Logging

```cpp
void f() {
  Log(NAMEOF(f), " method entry");
}
```

## Remarks

If you need to get the fully-qualified name, you can use the NAMEOF_FULL(). For example:

```cpp
NAMEOF_FULL(someVar.SomeField) -> "someVar.SomeField"
NAMEOF_FULL(&SomeStruct::SomeMethod2) -> "&SomeStruct::SomeMethod2"
NAMEOF_TYPE_FULL(std::string) -> "std::string"
```

## License MIT