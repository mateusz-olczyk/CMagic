cmake_minimum_required(VERSION 3.16)
include(utils)

add_library(cmagic_internals
    avl_tree.c
)

target_include_directories(cmagic_internals
    PRIVATE "${PROJECT_SOURCE_DIR}/include"
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}"
)

if(CMAGIC_WITH_EXTRA_WARNINGS)
    cmagic_target_add_warnings(cmagic_internals)
endif()
