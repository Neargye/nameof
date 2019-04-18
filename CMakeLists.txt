cmake_minimum_required(VERSION 3.6)

project(nameof VERSION "0.8.1" LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(IS_TOPLEVEL_PROJECT TRUE)
else()
    set(IS_TOPLEVEL_PROJECT FALSE)
endif()

option(NAMEOF_OPT_BUILD_EXAMPLES "Build nameof examples" ${IS_TOPLEVEL_PROJECT})
option(NAMEOF_OPT_BUILD_TESTS "Build and perform nameof tests" ${IS_TOPLEVEL_PROJECT})

if(NAMEOF_OPT_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

if(NAMEOF_OPT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
