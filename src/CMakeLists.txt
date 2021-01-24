cmake_minimum_required(VERSION 3.16)
include(CheckCSourceCompiles)
include(utils)

check_c_source_compiles("
    struct scope {
        struct { int a; int b; };
        union { int c; int d; };
    };
    int main(void) { return 0; }
    " CMAGIC_C_ANONYMOUS_STRUCT_SUPPORT)

check_c_source_compiles("
    #include <stddef.h>
    int main(void) {
        _Alignas(max_align_t) int x;
        (void) _Alignof(x);
        return 0;
    }
    " CMAGIC_C_ALIGN_OPERATORS_SUPPORT)

add_library(cmagic
    memory.c utils.c
)

cmagic_target_bool_compile_definitions(cmagic
    CMAGIC_C_ANONYMOUS_STRUCT_SUPPORT CMAGIC_C_ALIGN_OPERATORS_SUPPORT
)

target_include_directories(cmagic
    PUBLIC "${CMAGIC_SOURCE_DIR}/include"
)

if(CMAGIC_WITH_EXTRA_WARNINGS)
    cmagic_target_add_warnings(cmagic)
endif()
