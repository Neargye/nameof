include_directories(3rdparty/Catch2)
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(${PROJECT_NAME}_test test.cpp)
add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME}_test)

add_executable(${PROJECT_NAME}_test_no_rtti test.cpp)
if((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    target_compile_options(${PROJECT_NAME}_test_no_rtti PUBLIC -fno-rtti)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${PROJECT_NAME}_test_no_rtti PUBLIC /GR-)
endif()
add_test(NAME ${PROJECT_NAME}_test_no_rtti COMMAND ${PROJECT_NAME}_test_no_rtti)