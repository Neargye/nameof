include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(3rdparty/Catch2)

set(SRC
    test.cpp
    3rdparty/Catch2/catch.hpp
    ${CMAKE_SOURCE_DIR}/src/nameof.hpp)

add_executable(${PROJECT_NAME}_test ${SRC})