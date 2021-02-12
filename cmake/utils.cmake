cmake_minimum_required(VERSION 3.16)
include_guard()

function(cmagic_target_add_warnings TARGET)
    set(GNU_CXX_FLAGS -Wall -Wextra -Wmissing-declarations -Wsign-conversion
        -Wconversion -pedantic)
    set(GNU_C_FLAGS ${GNU_CXX_FLAGS} -Wc++-compat)
    set(MSVC_FLAGS /W4 /wd4201 /wd4996 /wd4116 /wd4204 /wd4232 /wd4090 /wd4114)
    target_compile_options(${TARGET} PRIVATE
        $<$<COMPILE_LANG_AND_ID:C,Clang,GNU>:${GNU_C_FLAGS}>
        $<$<COMPILE_LANG_AND_ID:CXX,Clang,GNU>:${GNU_CXX_FLAGS}>
        $<$<COMPILE_LANG_AND_ID:C,MSVC>:${MSVC_FLAGS}>
        $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:${MSVC_FLAGS}>
    )
endfunction()
