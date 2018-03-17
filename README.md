# nameof(x) c++

C++ alternative to [nameOf](https://docs.microsoft.com/en-us/dotnet/csharp/language-reference/keywords/nameof) operator.

Simple Example:
```
#include <iostream>
#include <nameof.hpp>

struct SomeStruct {
  int SomeField;
  void SomeMethod() { std::cout << "No called!" << std::endl; }
};

int someVar = 0;

int main() {
  SomeStruct someVar{1};

  constexpr auto a = NAMEOF_VAR(someVar.SomeField);
  constexpr auto b = NAMEOF_VAR((&someVar)->SomeField);
  constexpr auto c = NAMEOF_VAR(someVar);
  constexpr auto d = NAMEOF_VAR(::someVar);
  constexpr auto e = NAMEOF_VAR(&SomeStruct::SomeMethod);
  constexpr auto f = NAMEOF_FUN(someVar.SomeMethod());
  constexpr auto g = NAMEOF_TYPE(SomeStruct);

  std::cout << a << std::endl;  // SomeField
  std::cout << b << std::endl;  // SomeField
  std::cout << c << std::endl;  // someVar
  std::cout << d << std::endl;  // someVar
  std::cout << e << std::endl;  // SomeMethod
  std::cout << f << std::endl;  // SomeMethod()
  std::cout << g << std::endl;  // SomeStruct
}
```
