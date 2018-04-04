include_directories(${CMAKE_SOURCE_DIR}/src)

set(SOURCE_EXAMPLE
    ${CMAKE_SOURCE_DIR}/src/nameof.hpp
    example.cpp)

add_executable(${PROJECT_NAME}_example ${SOURCE_EXAMPLE})