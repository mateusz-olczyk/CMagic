cmake_minimum_required(VERSION 3.16)
project(CMagic
        VERSION 0.1.0
        DESCRIPTION "Portable C library of utilities and data structures"
        HOMEPAGE_URL "https://github.com/mateusz-olczyk/CMagic"
        LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(CMAGIC_WITH_EXTRA_WARNINGS "Enable extra compilation warnings" ON)
option(CMAGIC_WITH_TESTS "Enable unit tests of CMagic library" ON)

add_subdirectory(src)
if(CMAGIC_WITH_TESTS)
    enable_testing()
    add_subdirectory(deps/unity)
    add_subdirectory(test)
endif()
