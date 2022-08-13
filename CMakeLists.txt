# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.10)

project ("allradixsort" VERSION 0.1)

# Include sub-projects.
add_subdirectory ("unittests")
add_subdirectory ("perftests")

