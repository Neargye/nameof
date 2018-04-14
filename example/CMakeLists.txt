include_directories(${CMAKE_SOURCE_DIR}/include)

set(SOURCE_EXAMPLE
    ${CMAKE_SOURCE_DIR}/include/nameof.hpp
    example.cpp)

add_executable(${PROJECT_NAME}_example ${SOURCE_EXAMPLE})
