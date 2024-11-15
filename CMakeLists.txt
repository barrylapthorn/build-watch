cmake_minimum_required(VERSION 3.28)

project(build-watch
        VERSION 1.0
        LANGUAGES CXX)

#  Allows us to override the standard in a toolchain file if necessary
if (NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 23)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(PACKAGE_TESTS "Build the tests" ON)

# Include helpers
include(CTest)
include(FetchContent)

# Include our helpers in the cmake dir
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(dependencies)
include(compiler_options)

# Threading
find_package(Threads REQUIRED)

# Add third-party header-only code.
add_subdirectory(third-party)

# Include our sub-directories.
add_subdirectory(libs/lib-build-watch)
add_subdirectory(apps/build-watch)
