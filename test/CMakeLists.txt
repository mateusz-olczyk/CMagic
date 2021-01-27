cmake_minimum_required(VERSION 3.16)
include(utils)

if(TARGET unity)
    # Turn off advanced MSVC warnings for Unity library
    get_target_property(UNITY_COMPILE_OPTIONS unity COMPILE_OPTIONS)
    list(REMOVE_ITEM UNITY_COMPILE_OPTIONS "$<$<C_COMPILER_ID:MSVC>:/Wall>")
    list(APPEND UNITY_COMPILE_OPTIONS "$<$<C_COMPILER_ID:MSVC>:/W4>")
    set_target_properties(unity PROPERTIES COMPILE_OPTIONS "${UNITY_COMPILE_OPTIONS}")
else()
    message(FATAL_ERROR "Test sources need Unity test framework library!")
endif()

function(cmagic_add_test_case TEST_SOURCE_PATH)
    get_filename_component(TEST_SOURCE_FILENAME "${TEST_SOURCE_PATH}" NAME_WE)
    set(TEST_EXECUTABLE_TARGET "test_${TEST_SOURCE_FILENAME}_app")

    add_executable(${TEST_EXECUTABLE_TARGET} "${TEST_SOURCE_PATH}")
    cmagic_target_add_warnings(${TEST_EXECUTABLE_TARGET})
    target_link_libraries(${TEST_EXECUTABLE_TARGET} PRIVATE cmagic unity)

    add_test(NAME "test_${TEST_SOURCE_FILENAME}" COMMAND "${TEST_EXECUTABLE_TARGET}")
endfunction()

cmagic_add_test_case(memory.c)
cmagic_add_test_case(memory_cxx.cpp)
cmagic_add_test_case(utils.c)
cmagic_add_test_case(vector.c)
