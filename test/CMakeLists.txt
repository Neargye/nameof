include_directories(3rdparty/Catch2)
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME}_test test.cpp)
add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME}_test)
