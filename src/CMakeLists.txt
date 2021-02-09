cmake_minimum_required(VERSION 3.16)
include(utils)
include(config)

add_library(cmagic
    memory.c
    utils.c
    vector.c
)

add_library(cmagic::cmagic ALIAS cmagic)

cmagic_config_file("cmagic_config.h")

target_include_directories(cmagic
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
    PRIVATE "${PROJECT_SOURCE_DIR}/include_cxx"
    INTERFACE "$<$<BOOL:CMAGIC_WITH_CXX_BINDINGS>:${PROJECT_SOURCE_DIR}/include_cxx>"
)

if(CMAGIC_WITH_EXTRA_WARNINGS)
    cmagic_target_add_warnings(cmagic)
endif()
