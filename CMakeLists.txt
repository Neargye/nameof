cmake_minimum_required(VERSION 3.2)

include(GNUInstallDirs)

project(nameof LANGUAGES CXX)
set(${PROJECT_NAME}_version "0.5.0")
set(package_name ${PROJECT_NAME})
set(include_source_dir "${CMAKE_SOURCE_DIR}/include")

option(NAMEOF_OPT_BUILD_EXAMPLES "Build nameof examples" ON)
option(NAMEOF_OPT_BUILD_TESTS "Build and perform nameof tests" ON)

add_library(${package_name} INTERFACE)
target_include_directories(${package_name} INTERFACE "$<BUILD_INTERFACE:${include_source_dir}>")

if(NAMEOF_OPT_BUILD_EXAMPLES)
    add_subdirectory(example)
endif()

if(NAMEOF_OPT_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
