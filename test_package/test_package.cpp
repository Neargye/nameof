#include <nameof.hpp>

#include <cstdlib>
#include <iostream>
#include <string_view>

struct SomeStruct {};
enum class Color { RED = -12, GREEN = 7, BLUE = 15 };

SomeStruct structvar;
int main() {
  // Compile-time.
  constexpr auto name = NAMEOF(structvar);
  static_assert("structvar" == name);

  // Nameof.
  using std::literals::string_view_literals::operator""sv;
  
  std::string_view res1 = NAMEOF(structvar);
  std::string_view res2 = NAMEOF(::structvar);
  
  std::cout << res1 << '\n' << res2 << '\n';
  
  bool success = res1 == "structvar"sv
              && res2 == "structvar"sv;
  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
