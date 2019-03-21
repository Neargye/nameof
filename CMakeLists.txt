cmake_minimum_required(VERSION 3.6)

project(nameof VERSION "0.6.1" LANGUAGES CXX)

option(NAMEOF_OPT_BUILD_EXAMPLES "Build nameof examples" ON)
option(NAMEOF_OPT_BUILD_TESTS "Build and perform nameof tests" ON)

if(NAMEOF_OPT_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

if(NAMEOF_OPT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
