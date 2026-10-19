cmake_minimum_required(VERSION 3.22)

project(nameof_consumer LANGUAGES CXX)

enable_testing()

find_package(nameof CONFIG REQUIRED)

add_executable(nameof_consumer main.cpp)
target_link_libraries(nameof_consumer PRIVATE nameof::nameof)
target_compile_features(nameof_consumer PRIVATE cxx_std_17)
set_target_properties(nameof_consumer PROPERTIES CXX_EXTENSIONS OFF)

add_test(NAME nameof_consumer COMMAND nameof_consumer)
