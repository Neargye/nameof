include(CheckCXXCompilerFlag)

include(CheckCXXSourceCompiles)

# test_standalone.cpp checks that the header version matches the CMake project version.
add_compile_definitions(
    NAMEOF_TEST_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
    NAMEOF_TEST_VERSION_MINOR=${PROJECT_VERSION_MINOR}
    NAMEOF_TEST_VERSION_PATCH=${PROJECT_VERSION_PATCH}
)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC" OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC"))
    set(OPTIONS /W4 /WX)
    set(NAMEOF_TEST_FLAG_PREFIX /std:)
    set(NAMEOF_CPP26_STANDARD_FLAG c++latest)
    set(NAMEOF_NO_RTTI_OPTION /GR-)
    check_cxx_compiler_flag(/permissive- HAS_PERMISSIVE_FLAG)
    if(HAS_PERMISSIVE_FLAG)
        list(APPEND OPTIONS /permissive-)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(OPTIONS -Wall -Wextra -pedantic-errors -Werror)
    set(NAMEOF_TEST_FLAG_PREFIX -std=)
    set(NAMEOF_CPP26_STANDARD_FLAG c++26)
    set(NAMEOF_NO_RTTI_OPTION -fno-rtti)
endif()

set(NAMEOF_STD_REFLECTION_COMPILE_OPTIONS)
set(NAMEOF_CAN_COMPILE_STD_REFLECTION FALSE)
if(NAMEOF_TEST_FLAG_PREFIX)
    check_cxx_compiler_flag(
        "${NAMEOF_TEST_FLAG_PREFIX}${NAMEOF_CPP26_STANDARD_FLAG}"
        NAMEOF_HAS_CPP26_FLAG
    )
endif()

if(NAMEOF_HAS_CPP26_FLAG)
    # Probe the header itself, so the tests follow its backend selection.
    set(NAMEOF_STD_REFLECTION_PROBE [[
        #include <nameof.hpp>
        enum class reflected_enum { value };
        static_assert(nameof::detail::reflection::enumerators_v<reflected_enum>.size() == 1);
        static_assert(nameof::nameof_enum(reflected_enum::value) == "value");
        int main() {}
    ]])
    set(NAMEOF_SAVED_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
    set(NAMEOF_SAVED_REQUIRED_INCLUDES "${CMAKE_REQUIRED_INCLUDES}")
    set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} ${NAMEOF_TEST_FLAG_PREFIX}${NAMEOF_CPP26_STANDARD_FLAG}")
    list(APPEND CMAKE_REQUIRED_INCLUDES "${PROJECT_SOURCE_DIR}/include")
    check_cxx_source_compiles(
        "${NAMEOF_STD_REFLECTION_PROBE}"
        NAMEOF_CAN_COMPILE_STD_REFLECTION_BY_DEFAULT
    )
    if(NAMEOF_CAN_COMPILE_STD_REFLECTION_BY_DEFAULT)
        set(NAMEOF_CAN_COMPILE_STD_REFLECTION TRUE)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -freflection")
        check_cxx_source_compiles(
            "${NAMEOF_STD_REFLECTION_PROBE}"
            NAMEOF_CAN_COMPILE_STD_REFLECTION_WITH_GNU_OPTION
        )
        if(NAMEOF_CAN_COMPILE_STD_REFLECTION_WITH_GNU_OPTION)
            set(NAMEOF_CAN_COMPILE_STD_REFLECTION TRUE)
            list(APPEND NAMEOF_STD_REFLECTION_COMPILE_OPTIONS -freflection)
        endif()
    endif()
    set(CMAKE_REQUIRED_FLAGS "${NAMEOF_SAVED_REQUIRED_FLAGS}")
    set(CMAKE_REQUIRED_INCLUDES "${NAMEOF_SAVED_REQUIRED_INCLUDES}")
    unset(NAMEOF_SAVED_REQUIRED_FLAGS)
    unset(NAMEOF_SAVED_REQUIRED_INCLUDES)
    unset(NAMEOF_STD_REFLECTION_PROBE)
endif()

add_library(nameof-test-main OBJECT test_main.cpp)
target_compile_options(nameof-test-main PRIVATE ${OPTIONS})
target_include_directories(nameof-test-main PUBLIC 3rdparty)
target_compile_definitions(nameof-test-main
    PUBLIC DOCTEST_CONFIG_USE_STD_HEADERS
    INTERFACE FMT_HEADER_ONLY
)
target_compile_features(nameof-test-main PRIVATE cxx_std_17)
set_target_properties(nameof-test-main PROPERTIES CXX_EXTENSIONS OFF)
if(NAMEOF_TEST_FLAG_PREFIX STREQUAL "/std:")
    target_compile_options(nameof-test-main INTERFACE /utf-8)
endif()
if(NAMEOF_TEST_REQUIRE_FMT)
    target_compile_definitions(nameof-test-main INTERFACE NAMEOF_TEST_REQUIRE_FMT)
endif()

function(make_test src target std)
    cmake_parse_arguments(PARSE_ARGV 3 TEST "DOCTEST" "" "")
    add_executable(${target} ${src})
    target_compile_options(${target} PRIVATE ${OPTIONS})
    target_link_libraries(${target} PRIVATE ${PROJECT_NAME})
    if(TEST_DOCTEST)
        target_link_libraries(${target} PRIVATE nameof-test-main)
    endif()
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
    if(std)
        target_compile_features(${target} PRIVATE cxx_std_${std})
        set_target_properties(${target} PROPERTIES CXX_STANDARD ${std} CXX_STANDARD_REQUIRED ON)
        # These tests expect compiler-specific range semantics, even in newer language modes.
        target_compile_definitions(${target} PRIVATE NAMEOF_FORCE_COMPILER_SPECIFIC_REFLECTION=1)
    endif()
    add_test(NAME ${target} COMMAND ${target})
endfunction()

function(make_compile_fail_test case_name case_id expected_text)
    cmake_parse_arguments(PARSE_ARGV 3 TEST "REFLECTION" "UNEXPECTED_TEXT" "")
    if(TEST_REFLECTION)
        set(target "compile-fail-reflection-${case_name}")
        set(src reflection_compile_fail.cpp)
        set(case_definition NAMEOF_REFLECTION_COMPILE_FAIL_CASE=${case_id})
    else()
        set(target "compile-fail-${case_name}")
        set(src compile_fail.cpp)
        set(case_definition NAMEOF_COMPILE_FAIL_CASE=${case_id})
    endif()

    add_executable(${target} EXCLUDE_FROM_ALL ${src})
    target_compile_options(${target} PRIVATE ${OPTIONS})
    target_compile_definitions(${target} PRIVATE ${case_definition})
    target_link_libraries(${target} PRIVATE ${PROJECT_NAME})
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
    if(TEST_REFLECTION)
        target_compile_options(${target} PRIVATE
            "${NAMEOF_TEST_FLAG_PREFIX}${NAMEOF_CPP26_STANDARD_FLAG}"
            ${NAMEOF_STD_REFLECTION_COMPILE_OPTIONS}
        )
    else()
        target_compile_definitions(${target} PRIVATE NAMEOF_FORCE_COMPILER_SPECIFIC_REFLECTION=1)
    endif()

    add_test(
        NAME ${target}
        COMMAND ${CMAKE_COMMAND}
            "-DBUILD_DIR=${CMAKE_BINARY_DIR}"
            "-DTARGET_NAME=${target}"
            "-DCONFIG=$<CONFIG>"
            "-DEXPECTED_TEXT=${expected_text}"
            "-DUNEXPECTED_TEXT=${TEST_UNEXPECTED_TEXT}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/check_compile_fail.cmake"
    )
    set_tests_properties(${target} PROPERTIES RUN_SERIAL TRUE)
endfunction()

make_test(test.cpp test-cpp17 17 DOCTEST)
make_test(test_aliases.cpp test_aliases-cpp17 17 DOCTEST)
make_test(test_range.cpp test-range-cpp17 17)
make_test(test_range_negative.cpp test-range-negative-cpp17 17)
make_test(test_reflection.cpp test-reflection-force-compiler-specific-cpp17 17 DOCTEST)
make_test(test_standalone.cpp test-standalone-cpp17 17)

if(NAMEOF_NO_RTTI_OPTION)
    make_test(test_standalone.cpp test-standalone-no-rtti-cpp17 17)
    target_compile_options(test-standalone-no-rtti-cpp17 PRIVATE ${NAMEOF_NO_RTTI_OPTION})
    target_compile_definitions(test-standalone-no-rtti-cpp17 PRIVATE NAMEOF_TEST_NO_RTTI=1)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 11)
    make_test(test_standalone.cpp test-standalone-wconversion-cpp17 17)
    target_compile_options(test-standalone-wconversion-cpp17 PRIVATE -Wconversion)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # Also implied by -fsanitize=undefined: GCC then rejects `&T::method == nullptr` in constant expressions.
    make_test(test_standalone.cpp test-standalone-no-delete-null-pointer-checks-cpp17 17)
    target_compile_options(test-standalone-no-delete-null-pointer-checks-cpp17 PRIVATE -fno-delete-null-pointer-checks)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    foreach(convention IN ITEMS Gv Gr Gz)
        check_cxx_compiler_flag("/${convention}" NAMEOF_HAS_${convention})
        if(NAMEOF_HAS_${convention})
            # C++20 enables NAMEOF_MEMBER and NAMEOF_POINTER on MSVC, whose function names carry the convention.
            make_test(test_standalone.cpp test-standalone-${convention}-cpp20 20)
            target_compile_options(test-standalone-${convention}-cpp20 PRIVATE /${convention})
        endif()
    endforeach()
endif()

make_compile_fail_test(expression-no-name 1 "Expression does not have a name")
make_compile_fail_test(inverted-range 2 "nameof::customize::enum_range must contain at least one value" UNEXPECTED_TEXT "detail::names_v<")
make_compile_fail_test(range-too-large 3 "nameof::customize::enum_range must contain fewer than UINT16_MAX values" UNEXPECTED_TEXT "detail::names_v<")
make_compile_fail_test(range-not-int 4 "nameof::customize::enum_range bounds must fit in int" UNEXPECTED_TEXT "narrow")
make_compile_fail_test(range-min-not-int 5 "nameof::customize::enum_range bounds must fit in int" UNEXPECTED_TEXT "narrow")
make_compile_fail_test(range-unsigned-not-int 6 "nameof::customize::enum_range bounds must fit in int" UNEXPECTED_TEXT "narrow")
make_compile_fail_test(range-full-int 7 "nameof::customize::enum_range must contain fewer than UINT16_MAX values" UNEXPECTED_TEXT "detail::names_v<")
make_compile_fail_test(name-too-long 8 "nameof::cstring supports names of at most UINT16_MAX characters")
make_compile_fail_test(unnamed-struct-short-name 9 "Type does not have a short name.")
make_compile_fail_test(unnamed-enum-short-name 10 "Type does not have a short name.")
make_compile_fail_test(forward-declared-enum 11 "nameof::nameof_enum requires a defined enum")
make_compile_fail_test(empty-enum 12 "nameof::nameof_enum requires a defined enum")
make_compile_fail_test(enum-out-of-range 13 "nameof::nameof_enum requires a defined enum")
make_compile_fail_test(enum-flag-without-bits 14 "nameof::nameof_enum_flag requires a defined enum")

foreach(std IN ITEMS 20 23 26)
    if("cxx_std_${std}" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
        make_test(test.cpp test-cpp${std} ${std} DOCTEST)
        make_test(test_aliases.cpp test_aliases-cpp${std} ${std} DOCTEST)
        if(std EQUAL 20 AND NAMEOF_NO_RTTI_OPTION)
            make_test(test_standalone.cpp test-standalone-no-rtti-cpp20 20)
            target_compile_options(test-standalone-no-rtti-cpp20 PRIVATE ${NAMEOF_NO_RTTI_OPTION})
            target_compile_definitions(test-standalone-no-rtti-cpp20 PRIVATE NAMEOF_TEST_NO_RTTI=1)
        endif()
    endif()
endforeach()

if(NAMEOF_CAN_COMPILE_STD_REFLECTION)
    make_test(test.cpp test-reflection-cpp26 "" DOCTEST)
    make_test(test_aliases.cpp test_aliases-reflection-cpp26 "" DOCTEST)
    make_test(test_reflection.cpp test-reflection-semantics-cpp26 "" DOCTEST)

    foreach(target IN ITEMS test-reflection-cpp26 test_aliases-reflection-cpp26 test-reflection-semantics-cpp26)
        target_compile_options(${target} PRIVATE
            "${NAMEOF_TEST_FLAG_PREFIX}${NAMEOF_CPP26_STANDARD_FLAG}"
            ${NAMEOF_STD_REFLECTION_COMPILE_OPTIONS}
        )
        target_compile_definitions(${target} PRIVATE NAMEOF_TEST_STD_REFLECTION=1)
    endforeach()

    make_test(test_reflection.cpp test-reflection-force-compiler-specific-cpp26 "" DOCTEST)
    target_compile_options(test-reflection-force-compiler-specific-cpp26 PRIVATE
        "${NAMEOF_TEST_FLAG_PREFIX}${NAMEOF_CPP26_STANDARD_FLAG}"
        ${NAMEOF_STD_REFLECTION_COMPILE_OPTIONS}
    )
    target_compile_definitions(test-reflection-force-compiler-specific-cpp26 PRIVATE NAMEOF_FORCE_COMPILER_SPECIFIC_REFLECTION=1)

    # Checks that NAMEOF_DETAIL_USE_STD_REFLECTION does not leak.
    make_test(test_standalone.cpp test-standalone-reflection-cpp26 "")
    target_compile_options(test-standalone-reflection-cpp26 PRIVATE
        "${NAMEOF_TEST_FLAG_PREFIX}${NAMEOF_CPP26_STANDARD_FLAG}"
        ${NAMEOF_STD_REFLECTION_COMPILE_OPTIONS}
    )
    target_compile_definitions(test-standalone-reflection-cpp26 PRIVATE NAMEOF_TEST_STD_REFLECTION=1)

    make_compile_fail_test(
        incomplete-enum
        1
        "nameof requires a complete enum definition"
        REFLECTION
    )
endif()
