cmake_minimum_required(VERSION 3.16)
project(CMagic
        VERSION 0.1.0
        DESCRIPTION "Portable C library of utilities and data structures"
        HOMEPAGE_URL "https://github.com/mateusz-olczyk/CMagic"
        LANGUAGES C)

set(CMAGIC_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
list(APPEND CMAKE_MODULE_PATH "${CMAGIC_SOURCE_DIR}/cmake")
include(utils)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(CMAGIC_WITH_EXTRA_WARNINGS "Enable extra compilation warnings" ON)

add_subdirectory(src)
