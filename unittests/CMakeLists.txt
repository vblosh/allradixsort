cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#GTEST
enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

set(INCLUDE_DIR "../include/")
set(HEADER_FILES ${INCLUDE_DIR}radixsort.h ${INCLUDE_DIR}traits.h)
set(SOURCE_FILES tests.cpp)

add_executable(runUnitTests ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(runUnitTests PRIVATE ${INCLUDE_DIR})
target_link_libraries(runUnitTests GTest::GTest GTest::Main)

add_test( runUnitTests runUnitTests )