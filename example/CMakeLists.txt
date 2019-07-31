add_executable(example example.cpp)
target_link_libraries(example PRIVATE nameof::nameof)
set_target_properties(example PROPERTIES CXX_EXTENSIONS OFF)

target_compile_options(example
  PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
      -Wall -Wextra -pedantic-errors -Werror>
    $<$<CXX_COMPILER_ID:MSVC>:
      /W4 /WX>
)
