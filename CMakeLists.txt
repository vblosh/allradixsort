cmake_minimum_required (VERSION 3.10)

project (allradixsort 
    VERSION 0.1 
    DESCRIPTION "It is a radix sort implementation for all primitive data types"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(allradixsort INTERFACE )
target_include_directories(allradixsort INTERFACE include)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(CTest)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING)
    enable_testing()
    add_subdirectory ("unittests")
    add_subdirectory ("perftests")
endif()



