#include <nameof.hpp>

#include <cstdlib>

struct SomeStruct {};

SomeStruct structvar;

int main() {
  constexpr auto name = NAMEOF(structvar);
  static_assert("structvar" == name);

  std::string_view res1 = NAMEOF(structvar);
  std::string_view res2 = NAMEOF(::structvar);

  bool success = (res1 == "structvar") && (res2 == "structvar");

  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
