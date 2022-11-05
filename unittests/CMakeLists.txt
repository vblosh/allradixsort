cmake_minimum_required(VERSION 3.10)

#GTEST
include(GoogleTest)

set(SOURCE_FILES tests.cpp)
set(UNIT_TESTS "unittests")

add_executable(${UNIT_TESTS} ${SOURCE_FILES})
target_include_directories(${UNIT_TESTS} PRIVATE )
target_link_libraries(${UNIT_TESTS} GTest::GTest GTest::Main allradixsort)

add_test( ${UNIT_TESTS} ${UNIT_TESTS} )

