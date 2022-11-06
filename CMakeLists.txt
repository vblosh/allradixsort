# Works with 3.14 
cmake_minimum_required(VERSION 3.14)

project (allradixsort 
    VERSION 0.1 
    DESCRIPTION "It is a radix sort implementation for all primitive data types"
    LANGUAGES CXX)

file(GLOB HEADER_LIST CONFIGURE_DEPENDS "include/allradixsort/*.hpp")
add_library(allradixsort INTERFACE ${HEADER_LIST})
target_include_directories(allradixsort INTERFACE include)

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    # Optionally set things like CMAKE_CXX_STANDARD, CMAKE_POSITION_INDEPENDENT_CODE here
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    # Let's ensure -std=c++xx instead of -std=g++xx
    set(CMAKE_CXX_EXTENSIONS OFF)

    # Let's nicely support folders in IDEs
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    # Testing only available if this is the main app and GTest installed
    find_package(GTest)
    if(GTest_FOUND)
        message("GTest found, build unit tests")

        # Note this needs to be done in the main CMakeLists
        # since it calls enable_testing, which must be in the
        # main CMakeLists.
        include(CTest)
        add_subdirectory(unittests)
    endif()

    # The compiled perfomance tests code is here
    add_subdirectory (perftests)
endif()

