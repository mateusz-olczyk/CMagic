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
    get_filename_component(TEST_SOURCE_FILENAME_WITHOUT_EXTENSION "${TEST_SOURCE_PATH}" NAME_WE)
    set(TEST_NAME "${TEST_SOURCE_FILENAME_WITHOUT_EXTENSION}")
    set(TEST_EXECUTABLE "test_${TEST_NAME}")

    add_executable(${TEST_EXECUTABLE} "${TEST_SOURCE_PATH}")
    cmagic_target_add_warnings(${TEST_EXECUTABLE})
    target_link_libraries(${TEST_EXECUTABLE}
        PRIVATE cmagic
        PRIVATE cmagic_internals
        PRIVATE unity
    )

    add_test(NAME "${TEST_NAME}" COMMAND "${TEST_EXECUTABLE}")
endfunction()

cmagic_add_test_case(avl_tree.c)
cmagic_add_test_case(memory.c)
cmagic_add_test_case(memory_cxx.cpp)
cmagic_add_test_case(set.c)
cmagic_add_test_case(set_cxx.cpp)
cmagic_add_test_case(utils.c)
cmagic_add_test_case(vector.c)
cmagic_add_test_case(vector_cxx.cpp)
