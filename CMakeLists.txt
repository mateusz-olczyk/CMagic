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
option(CMAGIC_WITH_CXX_BINDINGS "Add C++ bindings headers to the library interface" ON)

add_subdirectory(src)

# Things enabled only if this is top level project
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # Tests
    enable_testing()
    add_subdirectory(deps/unity)
    add_subdirectory(test)

    # Docs
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doc")
        set(DOXYGEN_ALWAYS_DETAILED_SEC YES)
        set(DOXYGEN_EXTRACT_STATIC YES)
        set(DOXYGEN_HIDE_UNDOC_MEMBERS YES)
        set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)
        set(DOXYGEN_STRIP_FROM_PATH include)
    
        set(CMAGIC_DOC_DIRS README.md include/cmagic)
        if(CMAGIC_WITH_CXX_BINDINGS)
            list(APPEND CMAGIC_DOC_DIRS include_cxx/cmagic)
            list(APPEND DOXYGEN_STRIP_FROM_PATH include_cxx)
        endif()
    
        doxygen_add_docs(doc ${CMAGIC_DOC_DIRS})
    endif()
endif()

