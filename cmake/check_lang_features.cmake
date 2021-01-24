cmake_minimum_required(VERSION 3.16)
include_guard()
include(CheckCSourceCompiles)

function(cmagic_c_check_anonymous_struct_support RESULT_VAR)
    check_c_source_compiles("
        struct scope {
            struct {
                int a;
                int b;
            };
            union {
                int c;
                int d;
            };
        };
    
        int main(void) {
            return 0;
        }
    " CMAGIC_C_ANONYMOUS_STRUCT_SUPPORT)
    if(CMAGIC_C_ANONYMOUS_STRUCT_SUPPORT)
        set(${RESULT_VAR} YES PARENT_SCOPE)
    else()
        set(${RESULT_VAR} NO PARENT_SCOPE)
    endif()
endfunction()

function(cmagic_c_check_alignas_support RESULT_VAR)
    check_c_source_compiles("
        #include <stddef.h>

        int main(void) {
            _Alignas(max_align_t) int x;
            (void) x;
            return 0;
        }    
    " CMAGIC_C_ALIGNAS_SUPPORT)
    if(CMAGIC_C_ALIGNAS_SUPPORT)
        set(${RESULT_VAR} YES PARENT_SCOPE)
    else()
        set(${RESULT_VAR} NO PARENT_SCOPE)
    endif()
endfunction()
