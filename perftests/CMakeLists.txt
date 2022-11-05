cmake_minimum_required(VERSION 3.10)

set(SOURCE_FILES tests.cpp)

add_executable(runPerfomanceTests ${SOURCE_FILES})
target_include_directories(runPerfomanceTests PRIVATE )
target_link_libraries(runPerfomanceTests allradixsort)
