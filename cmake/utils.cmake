cmake_minimum_required(VERSION 3.16)
include_guard()

function(cmagic_target_add_warnings TARGET)
    target_compile_options(${TARGET} PRIVATE
        $<$<C_COMPILER_ID:Clang,GNU>: -Wall
                                      -Wextra
                                      -Winit-self
                                      -Wmissing-declarations
                                      -Wc++-compat
                                      -Wsign-conversion
                                      -Wconversion >
        $<$<C_COMPILER_ID:MSVC>: /Wall >
    )
endfunction()
