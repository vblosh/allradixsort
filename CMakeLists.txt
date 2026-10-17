cmake_minimum_required(VERSION 3.14)

project(allradixsort
    VERSION 0.1.0
    DESCRIPTION "Stable radix sort for primitive integer and floating-point keys"
    LANGUAGES CXX)

include(GNUInstallDirs)

add_library(allradixsort INTERFACE)
add_library(allradixsort::allradixsort ALIAS allradixsort)
target_compile_features(allradixsort INTERFACE cxx_std_17)
target_include_directories(allradixsort INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(ALLRADIXSORT_IS_TOP_LEVEL ON)
else()
    set(ALLRADIXSORT_IS_TOP_LEVEL OFF)
endif()

option(ALLRADIXSORT_INSTALL "Generate install targets" ${ALLRADIXSORT_IS_TOP_LEVEL})

if(ALLRADIXSORT_IS_TOP_LEVEL)
    option(ALLRADIXSORT_BUILD_TESTS "Build allradixsort unit tests" ON)
    option(ALLRADIXSORT_BUILD_BENCHMARKS "Build allradixsort performance tests" ON)

    if(ALLRADIXSORT_BUILD_TESTS)
        include(CTest)
        find_package(GTest QUIET)
        if(GTest_FOUND)
            add_subdirectory(unittests)
        else()
            message(WARNING "GoogleTest was not found; unit tests will not be built")
        endif()
    endif()

    if(ALLRADIXSORT_BUILD_BENCHMARKS)
        add_subdirectory(perftests)
    endif()
endif()

if(ALLRADIXSORT_INSTALL)
    include(CMakePackageConfigHelpers)
    configure_package_config_file(
        cmake/allradixsortConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/allradixsortConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/allradixsort)
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/allradixsortConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

    install(TARGETS allradixsort EXPORT allradixsortTargets)
    install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    install(EXPORT allradixsortTargets
        FILE allradixsortTargets.cmake
        NAMESPACE allradixsort::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/allradixsort)
    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/allradixsortConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/allradixsortConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/allradixsort)
endif()
