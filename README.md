# nameof(x) c++

Simple Example:
```
#include <iostream>
#include <cstdint>
#include <nameof.hpp>
struct SomeStruct {
  int SomeField;
  void SomeMethod() { std::cout << "No called!" << std::endl; }
};

int someVar = 0;

int main() {
  SomeStruct someVar{1};

  constexpr auto a = nameof_variable(someVar.SomeField);
  constexpr auto b = nameof_variable((&someVar)->SomeField);
  constexpr auto c = nameof_variable(someVar);
  constexpr auto d = nameof_variable(::someVar);
  constexpr auto e = nameof_variable(&SomeStruct::SomeMethod);
  constexpr auto f = nameof_function(someVar.SomeMethod());
  constexpr auto g = nameof_type(SomeStruct);

  std::cout << a << std::endl;  // SomeField
  std::cout << b << std::endl;  // SomeField
  std::cout << c << std::endl;  // someVar
  std::cout << d << std::endl;  // someVar
  std::cout << e << std::endl;  // SomeMethod
  std::cout << f << std::endl;  // SomeMethod()
  std::cout << g << std::endl;  // SomeStruct
}
```
