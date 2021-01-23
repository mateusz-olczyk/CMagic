cmake_minimum_required(VERSION 3.16)

function(cmagic_add_test_case TEST_SOURCE_PATH)
    get_filename_component(TEST_SOURCE_FILENAME "${TEST_SOURCE_PATH}" NAME_WE)
    set(TEST_EXECUTABLE_TARGET "test_${TEST_SOURCE_FILENAME}_app")

    add_executable(${TEST_EXECUTABLE_TARGET} "${TEST_SOURCE_PATH}")
    cmagic_target_add_warnings(${TEST_EXECUTABLE_TARGET})
    target_link_libraries(${TEST_EXECUTABLE_TARGET} PRIVATE cmagic unity)

    add_test(NAME "test_${TEST_SOURCE_FILENAME}" COMMAND "${TEST_EXECUTABLE_TARGET}")
endfunction()

cmagic_add_test_case(memory.c)
