include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(3rdparty/Catch2)

set(SRC test.cpp)

add_executable(${PROJECT_NAME}_test ${SRC})

add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME}_test)