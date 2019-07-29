cmake_minimum_required(VERSION 3.8)
project(test_package CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_executable(test_package test_package.cpp)
target_link_libraries(test_package PRIVATE CONAN_PKG::nameof)
target_compile_features(test_package PRIVATE cxx_std_17)
