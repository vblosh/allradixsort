cmake_minimum_required(VERSION 3.10)

set(SOURCE_FILES tests.cpp)
set(PERF_TESTS "perftests")

add_executable(${PERF_TESTS} ${SOURCE_FILES})
target_include_directories(${PERF_TESTS} PRIVATE )
target_link_libraries(${PERF_TESTS} allradixsort)
