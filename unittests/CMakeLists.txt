cmake_minimum_required(VERSION 3.10)

#GTEST
include(GoogleTest)

set(SOURCE_FILES tests.cpp odr_a.cpp odr_b.cpp)
set(UNIT_TESTS unittests)

add_executable(${UNIT_TESTS} ${SOURCE_FILES})
target_link_libraries(${UNIT_TESTS} PRIVATE GTest::GTest GTest::Main allradixsort::allradixsort)

gtest_discover_tests(${UNIT_TESTS})
