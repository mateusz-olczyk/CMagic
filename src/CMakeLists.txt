cmake_minimum_required(VERSION 3.16)
include(CheckCSourceCompiles)
include(utils)

check_c_source_compiles("
    struct scope {
        struct { int a; int b; };
        union { int c; int d; };
    };
    int main(void) { return 0; }"
    CMAGIC_C_ANONYMOUS_STRUCT_SUPPORT
)

check_c_source_compiles("
    #include <stddef.h>
    int main(void) {
        _Alignas(double) int x;
        return 0;
    }"
    CMAGIC_C_ALIGNAS_OPERATOR_SUPPORT
)

check_c_source_compiles("
    int main(void) {
        _Alignof(int);
        return 0;
    }"
    CMAGIC_C_ALIGNOF_OPERATOR_SUPPORT
)

check_c_source_compiles("
    #include <stddef.h>
    int main(void) {
        max_align_t val;
        return 0;
    }"
    CMAGIC_C_MAX_ALIGN_TYPE_SUPPORT
)

add_library(cmagic
    memory.c
    utils.c
    vector.c
)

configure_file("${PROJECT_SOURCE_DIR}/cmake/cmagic_config.h.in" "cmagic_config.h")

target_include_directories(cmagic
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
)

if(CMAGIC_WITH_EXTRA_WARNINGS)
    cmagic_target_add_warnings(cmagic)
endif()
