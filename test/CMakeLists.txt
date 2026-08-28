include(CheckCXXCompilerFlag)

include(CheckCXXSourceCompiles)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC" OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC"))
    set(OPTIONS /W4 /WX)
    set(NAMEOF_TEST_FLAG_PREFIX /std:)
    set(NAMEOF_CPP26_STANDARD_FLAG c++latest)
    check_cxx_compiler_flag(/permissive- HAS_PERMISSIVE_FLAG)
    if(HAS_PERMISSIVE_FLAG)
        set(OPTIONS ${OPTIONS} /permissive-)
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(OPTIONS -Wall -Wextra -pedantic-errors -Werror)
    set(NAMEOF_TEST_FLAG_PREFIX -std=)
    set(NAMEOF_CPP26_STANDARD_FLAG c++26)
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
    set(NAMEOF_STD_REFLECTION_PROBE [[
        #include <meta>
        #include <string_view>
        enum class reflected_enum { value };
        static_assert(std::meta::is_enumerable_type(^^reflected_enum));
        constexpr auto enumerators = std::define_static_array(std::meta::enumerators_of(^^reflected_enum));
        constexpr std::string_view reflected_name(reflected_enum value) {
            template for (constexpr auto enumerator : enumerators) {
                constexpr auto candidate = [:enumerator:];
                if (candidate == value) {
                    constexpr auto identifier = std::meta::identifier_of(enumerator);
                    constexpr auto name = std::define_static_string(identifier);
                    return {name, identifier.size()};
                }
            }
            return {};
        }
        constexpr auto name = reflected_name(reflected_enum::value);
        static_assert(name == "value");
        static_assert(name.data()[name.size()] == char{});
        int main() {}
    ]])
    set(NAMEOF_SAVED_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
    set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} ${NAMEOF_TEST_FLAG_PREFIX}${NAMEOF_CPP26_STANDARD_FLAG}")
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
    unset(NAMEOF_SAVED_REQUIRED_FLAGS)
    unset(NAMEOF_STD_REFLECTION_PROBE)
endif()

function(make_test src target std)
    add_executable(${target} ${src})
    target_compile_options(${target} PRIVATE ${OPTIONS})
    target_include_directories(${target} PRIVATE 3rdparty)
    target_compile_definitions(${target} PRIVATE DOCTEST_CONFIG_USE_STD_HEADERS FMT_HEADER_ONLY)
    if(NAMEOF_TEST_REQUIRE_FMT AND src STREQUAL "test.cpp")
        target_compile_definitions(${target} PRIVATE NAMEOF_TEST_REQUIRE_FMT)
    endif()
    target_link_libraries(${target} PRIVATE ${PROJECT_NAME})
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)
    if(std)
        target_compile_features(${target} PRIVATE cxx_std_${std})
    endif()
    add_test(NAME ${target} COMMAND ${target})
endfunction()

function(make_compile_fail_test case_name case_id expected_text)
    set(target "compile-fail-${case_name}")
    set(unexpected_text "${ARGV3}")

    add_executable(${target} EXCLUDE_FROM_ALL compile_fail.cpp)
    target_compile_options(${target} PRIVATE ${OPTIONS})
    target_compile_definitions(${target} PRIVATE NAMEOF_COMPILE_FAIL_CASE=${case_id})
    target_link_libraries(${target} PRIVATE ${PROJECT_NAME})
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)

    add_test(
        NAME ${target}
        COMMAND ${CMAKE_COMMAND}
            "-DBUILD_DIR=${CMAKE_BINARY_DIR}"
            "-DTARGET_NAME=${target}"
            "-DCONFIG=$<CONFIG>"
            "-DEXPECTED_TEXT=${expected_text}"
            "-DUNEXPECTED_TEXT=${unexpected_text}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/check_compile_fail.cmake"
    )
    set_tests_properties(${target} PROPERTIES RUN_SERIAL TRUE)
endfunction()

function(make_reflection_compile_fail_test case_name case_id expected_text)
    set(target "compile-fail-reflection-${case_name}")
    add_executable(${target} EXCLUDE_FROM_ALL reflection_compile_fail.cpp)
    target_compile_options(${target} PRIVATE
        ${OPTIONS}
        "${NAMEOF_TEST_FLAG_PREFIX}${NAMEOF_CPP26_STANDARD_FLAG}"
        ${NAMEOF_STD_REFLECTION_COMPILE_OPTIONS}
    )
    target_compile_definitions(${target} PRIVATE NAMEOF_REFLECTION_COMPILE_FAIL_CASE=${case_id})
    target_link_libraries(${target} PRIVATE ${PROJECT_NAME})
    set_target_properties(${target} PROPERTIES CXX_EXTENSIONS OFF)

    add_test(
        NAME ${target}
        COMMAND ${CMAKE_COMMAND}
            "-DBUILD_DIR=${CMAKE_BINARY_DIR}"
            "-DTARGET_NAME=${target}"
            "-DCONFIG=$<CONFIG>"
            "-DEXPECTED_TEXT=${expected_text}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/check_compile_fail.cmake"
    )
    set_tests_properties(${target} PROPERTIES RUN_SERIAL TRUE)
endfunction()

make_test(test.cpp test-cpp17 17)
make_test(test_aliases.cpp test_aliases-cpp17 17)
make_test(test_range.cpp test-range-cpp17 17)
make_test(test_range_negative.cpp test-range-negative-cpp17 17)
make_test(test_reflection.cpp test-reflection-force-compiler-specific-cpp17 17)
target_compile_definitions(test-reflection-force-compiler-specific-cpp17 PRIVATE NAMEOF_FORCE_COMPILER_SPECIFIC_REFLECTION=1)
make_test(test_standalone.cpp test-standalone-cpp17 17)

make_compile_fail_test(expression-no-name 1 "Expression does not have a name")
make_compile_fail_test(inverted-range 2 "nameof::customize::enum_range must contain at least one value" "names_v")
make_compile_fail_test(range-too-large 3 "nameof::customize::enum_range must contain fewer than UINT16_MAX values" "names_v")

if("cxx_std_20" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    make_test(test.cpp test-cpp20 20)
    make_test(test_aliases.cpp test_aliases-cpp20 20)
endif()

if("cxx_std_23" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    make_test(test.cpp test-cpp23 23)
    make_test(test_aliases.cpp test_aliases-cpp23 23)
endif()

if("cxx_std_26" IN_LIST CMAKE_CXX_COMPILE_FEATURES)
    make_test(test.cpp test-cpp26 26)
    make_test(test_aliases.cpp test_aliases-cpp26 26)
endif()

if(NAMEOF_CAN_COMPILE_STD_REFLECTION)
    make_test(test.cpp test-reflection-cpp26 "")
    make_test(test_aliases.cpp test_aliases-reflection-cpp26 "")
    make_test(test_reflection.cpp test-reflection-semantics-cpp26 "")

    foreach(target IN ITEMS test-reflection-cpp26 test_aliases-reflection-cpp26 test-reflection-semantics-cpp26)
        target_compile_options(${target} PRIVATE
            "${NAMEOF_TEST_FLAG_PREFIX}${NAMEOF_CPP26_STANDARD_FLAG}"
            ${NAMEOF_STD_REFLECTION_COMPILE_OPTIONS}
        )
        target_compile_definitions(${target} PRIVATE NAMEOF_TEST_STD_REFLECTION=1)
    endforeach()

    make_test(test_reflection.cpp test-reflection-force-compiler-specific-cpp26 "")
    target_compile_options(test-reflection-force-compiler-specific-cpp26 PRIVATE
        "${NAMEOF_TEST_FLAG_PREFIX}${NAMEOF_CPP26_STANDARD_FLAG}"
        ${NAMEOF_STD_REFLECTION_COMPILE_OPTIONS}
    )
    target_compile_definitions(test-reflection-force-compiler-specific-cpp26 PRIVATE NAMEOF_FORCE_COMPILER_SPECIFIC_REFLECTION=1)

    make_reflection_compile_fail_test(
        incomplete-enum
        1
        "nameof requires a complete enum definition"
    )
endif()
