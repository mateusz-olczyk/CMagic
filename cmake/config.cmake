cmake_minimum_required(VERSION 3.16)
include_guard()
include(CheckCSourceCompiles)

function(cmagic_config_file OUTPUT_FILE)
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
    configure_file("${PROJECT_SOURCE_DIR}/cmake/cmagic_config.h.in" "${OUTPUT_FILE}")
endfunction()

