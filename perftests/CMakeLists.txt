cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(INCLUDE_DIR "../include/")
set(HEADER_FILES ${INCLUDE_DIR}countingsort.h ${INCLUDE_DIR}radixsort.h ${INCLUDE_DIR}traits.h)
set(SOURCE_FILES tests.cpp)
add_executable(runPerfomanceTests ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(runPerfomanceTests PRIVATE ${INCLUDE_DIR})
target_link_libraries(runPerfomanceTests)

add_test( runPerfomanceTests runPerfomanceTests )