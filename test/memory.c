#include <string.h>
#include "cmagic/memory.h"
#include "cmagic/utils.h"
#include "unity.h"

void setUp(void) {}

void tearDown(void) {}

static void test_String(void) {
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocated_bytes());
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocations());

    const char *string = "Hello World!";
    const size_t string_size = strlen(string) + 1;

    char *allocated_string1 = (char *)cmagic_memory_malloc(string_size);
    TEST_ASSERT_NOT_NULL(allocated_string1);
    TEST_ASSERT_EQUAL_size_t(string_size, cmagic_memory_get_allocated_bytes());
    TEST_ASSERT_EQUAL_size_t(1, cmagic_memory_get_allocations());
    
    char *allocated_string2 = (char *)cmagic_memory_malloc(strlen(string) + 1);
    TEST_ASSERT_NOT_NULL(allocated_string2);
    TEST_ASSERT_EQUAL_size_t(2 * string_size, cmagic_memory_get_allocated_bytes());
    TEST_ASSERT_EQUAL_size_t(2, cmagic_memory_get_allocations());

    strcpy(allocated_string1, string);
    strcpy(allocated_string2, allocated_string1);

    TEST_ASSERT_EQUAL(CMAGIC_MEMORY_FREE_RESULT_OK, cmagic_memory_free(allocated_string1));
    TEST_ASSERT_EQUAL_size_t(string_size, cmagic_memory_get_allocated_bytes());
    TEST_ASSERT_EQUAL_size_t(1, cmagic_memory_get_allocations());

    TEST_ASSERT_EQUAL_STRING(string, allocated_string2);
    TEST_ASSERT_EQUAL(CMAGIC_MEMORY_FREE_RESULT_OK, cmagic_memory_free(allocated_string2));
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocated_bytes());
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocations());
}

static void test_Fail(void) {
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocated_bytes());
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocations());

    const void *fail_ptr = cmagic_memory_malloc(4000000000);
    TEST_ASSERT_NULL(fail_ptr);
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocated_bytes());
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocations());
}

static void test_MemoryFull(void) {
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocated_bytes());
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocations());

    void *memory_blocks[5000];
    size_t successful_allocations = 0;
    for (size_t i = 0; i < CMAGIC_UTILS_ARRAY_SIZE(memory_blocks); i++) {
        memory_blocks[i] = cmagic_memory_malloc(sizeof(int));
        if (memory_blocks[i] == NULL) {
            break;
        }
        else if (i == CMAGIC_UTILS_ARRAY_SIZE(memory_blocks) - 1) {
            TEST_FAIL_MESSAGE("still have free memory");
        }
        else {
            successful_allocations++;
        }
    }

    TEST_ASSERT_EQUAL_size_t(successful_allocations * sizeof(int), cmagic_memory_get_allocated_bytes());
    TEST_ASSERT_EQUAL_size_t(successful_allocations, cmagic_memory_get_allocations());

    for (size_t i = 0; i < successful_allocations; i++) {
        TEST_ASSERT_EQUAL(CMAGIC_MEMORY_FREE_RESULT_OK, cmagic_memory_free(memory_blocks[i]));
    }

    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocated_bytes());
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocations());

    TEST_ASSERT_NOT_NULL(memory_blocks[0] = cmagic_memory_malloc(sizeof(int)));
    TEST_ASSERT_EQUAL(CMAGIC_MEMORY_FREE_RESULT_OK, cmagic_memory_free(memory_blocks[0]));

    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocated_bytes());
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocations());
}

static void test_Errors(void) {
    void *memptr = cmagic_memory_malloc(20);
    TEST_ASSERT_NOT_NULL(memptr);
    TEST_ASSERT_EQUAL(CMAGIC_MEMORY_FREE_RESULT_OK_NULLPTR, cmagic_memory_free(NULL));
    TEST_ASSERT_EQUAL(CMAGIC_MEMORY_FREE_RESULT_ERR_ADDRESS_OUTSIDE_MEMORY_POOL, cmagic_memory_free((void *)1));
    TEST_ASSERT_EQUAL(CMAGIC_MEMORY_FREE_RESULT_OK, cmagic_memory_free(memptr));

    // Double free
    TEST_ASSERT_EQUAL(CMAGIC_MEMORY_FREE_RESULT_ERR_NOT_ALLOCATED_BEFORE, cmagic_memory_free(memptr));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_String);
    RUN_TEST(test_Fail);
    RUN_TEST(test_MemoryFull);
    RUN_TEST(test_Errors);
    return UNITY_END();
}
