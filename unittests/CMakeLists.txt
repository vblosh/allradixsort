cmake_minimum_required(VERSION 3.10)

if(MSVC)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
else(MSVC)
set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17)
endif(MSVC)

#GTEST
enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

set(INCLUDE_DIR "../include/")
set(HEADER_FILES ${INCLUDE_DIR}countingsort.h ${INCLUDE_DIR}radixsort.h ${INCLUDE_DIR}traits.h)
set(SOURCE_FILES tests.cpp)

add_executable(runUnitTests ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(runUnitTests PRIVATE ${INCLUDE_DIR})
target_link_libraries(runUnitTests GTest::GTest GTest::Main)

add_test( runUnitTests runUnitTests )