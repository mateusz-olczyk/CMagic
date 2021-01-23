cmake_minimum_required(VERSION 3.16)
project(CMagic
        VERSION 0.1.0
        DESCRIPTION "Portable C library of utilities and data structures"
        HOMEPAGE_URL "https://github.com/mateusz-olczyk/CMagic"
        LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(CMAGIC_WITH_EXTRA_WARNINGS "Enable extra compilation warnings" ON)

add_library(cmagic src/memory.c)
target_include_directories(cmagic PUBLIC include)
if(CMAGIC_WITH_EXTRA_WARNINGS)
    target_compile_options(cmagic PRIVATE
        $<$<C_COMPILER_ID:Clang,GNU>: -Wall
                                      -Wextra
                                      -Winit-self
                                      -Wmissing-declarations
                                      -Wc++-compat
                                      -Wsign-conversion
                                      -Wconversion >
    )
endif()
