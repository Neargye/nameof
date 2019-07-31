find_package(Catch2 REQUIRED)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-std=c++20 HAS_GNU_CPP20_FLAG)
check_cxx_compiler_flag(/std:c++20 HAS_MSVC_CPP20_FLAG)
set(HAS_CPP20 (HAS_GNU_CPP20_FLAG OR HAS_MSVC_CPP20_FLAG))

function(make_test target std)
  add_executable(${target} test.cpp)
  target_link_libraries(${target} PRIVATE Catch2::Catch2 nameof::nameof)
  set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
  target_compile_features(${target} PRIVATE ${std})

  target_compile_options(${target}
    PRIVATE
      $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -Wextra -pedantic-errors -Werror>
      $<$<CXX_COMPILER_ID:MSVC>:
        /W4 /WX>
  )
endfunction()

make_test(nameof-cpp17.t cxx_std_17)

if(HAS_CPP20 AND (cxx_std_20 IN_LIST CMAKE_CXX_COMPILE_FEATURES))
  make_test(nameof-cpp20.t cxx_std_20)
endif()
