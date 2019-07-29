# set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(Catch2 REQUIRED)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-std=c++20 HAS_GNU_CPP20_FLAG)
check_cxx_compiler_flag(/std:c++20 HAS_MSVC_CPP20_FLAG)

set(HAS_CPP20 (HAS_GNU_CPP20_FLAG OR HAS_MSVC_CPP20_FLAG))


add_executable(test-17 test.cpp)
target_link_libraries(test-17 PRIVATE Catch2::Catch2 nameof::nameof)
set_target_properties(test-17 PROPERTIES CXX_EXTENSIONS OFF)
target_compile_features(test-17 PRIVATE cxx_std_17)

target_compile_options(test-17
  PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
      -Wall -Wextra -pedantic-errors -Werror>
    $<$<CXX_COMPILER_ID:MSVC>:
      /W4 /WX>
)

if(HAS_CPP20)
  add_executable(test-20 test.cpp)
  target_link_libraries(test-20 PRIVATE Catch2::Catch2 nameof::nameof)
  set_target_properties(test-20 PROPERTIES CXX_EXTENSIONS OFF)
  target_compile_features(test-20 PRIVATE cxx_std_20)

  target_compile_options(test-20
    PRIVATE
      $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -pedantic-errors -Werror>
      $<$<CXX_COMPILER_ID:MSVC>:
        /W4 /WX>
  )
endif()
