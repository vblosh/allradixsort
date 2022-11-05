cmake_minimum_required(VERSION 3.10)

#GTEST
find_package(GTest REQUIRED)
include(GoogleTest)

set(SOURCE_FILES tests.cpp)

add_executable(runUnitTests ${SOURCE_FILES})
target_include_directories(runUnitTests PRIVATE )
target_link_libraries(runUnitTests GTest::GTest GTest::Main allradixsort)

add_test( runUnitTests runUnitTests )

