include_directories(${CMAKE_SOURCE_DIR}/src)

set(SRC
    example.cpp
    ${CMAKE_SOURCE_DIR}/src/nameof.hpp)

add_executable(${PROJECT_NAME}_example ${SRC})