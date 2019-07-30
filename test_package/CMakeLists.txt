cmake_minimum_required(VERSION 2.8.12)
project(test_package CXX)

set(CMAKE_VERBOSE_MAKEFILE TRUE)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(test_package test_package.cpp)
target_link_libraries(test_package ${CONAN_LIBS})
set_target_properties(test_package PROPERTIES CXX_STANDARD 17)
