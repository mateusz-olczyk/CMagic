cmake_minimum_required(VERSION 3.16)
include(utils)

add_library(cmagic memory.c)
target_include_directories(cmagic PUBLIC "${CMAGIC_SOURCE_DIR}/include")
if(CMAGIC_WITH_EXTRA_WARNINGS)
    cmagic_target_add_warnings(cmagic)
endif()
