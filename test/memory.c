#include <string.h>
#include "cmagic/memory.h"
#include "cmagic/utils.h"
#include "unity.h"

void setUp(void) {
    static uint8_t memory_pool[600]; 
    cmagic_memory_init(memory_pool, sizeof(memory_pool));
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocated_bytes());
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocations());
}

void tearDown(void) {
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocated_bytes());
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocations());
}

static void test_String(void) {
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

    TEST_ASSERT_TRUE(cmagic_memory_is_allocated(allocated_string1));
    TEST_ASSERT_TRUE(cmagic_memory_is_allocated(allocated_string2));
    strcpy(allocated_string1, string);
    strcpy(allocated_string2, allocated_string1);

    TEST_ASSERT_EQUAL(CMAGIC_MEMORY_FREE_RESULT_OK, cmagic_memory_free_ext(allocated_string1));
    TEST_ASSERT_EQUAL_size_t(string_size, cmagic_memory_get_allocated_bytes());
    TEST_ASSERT_EQUAL_size_t(1, cmagic_memory_get_allocations());
    TEST_ASSERT_FALSE(cmagic_memory_is_allocated(allocated_string1));
    TEST_ASSERT_TRUE(cmagic_memory_is_allocated(allocated_string2));

    TEST_ASSERT_EQUAL_STRING(string, allocated_string2);
    TEST_ASSERT_EQUAL(CMAGIC_MEMORY_FREE_RESULT_OK, cmagic_memory_free_ext(allocated_string2));
    TEST_ASSERT_FALSE(cmagic_memory_is_allocated(allocated_string1));
    TEST_ASSERT_FALSE(cmagic_memory_is_allocated(allocated_string2));
}

static void test_Fail(void) {
    void *fail_ptr = cmagic_memory_malloc(4000000000);
    TEST_ASSERT_NULL(fail_ptr);
    TEST_ASSERT_FALSE(cmagic_memory_is_allocated(fail_ptr));
}

static void test_MemoryFull(void) {
    void *memory_blocks[100];
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
            TEST_ASSERT_TRUE(cmagic_memory_is_allocated(memory_blocks[i]));
            successful_allocations++;
        }
    }

    TEST_ASSERT_EQUAL_size_t(successful_allocations * sizeof(int), cmagic_memory_get_allocated_bytes());
    TEST_ASSERT_EQUAL_size_t(successful_allocations, cmagic_memory_get_allocations());

    for (size_t i = 0; i < successful_allocations; i++) {
        TEST_ASSERT_EQUAL(CMAGIC_MEMORY_FREE_RESULT_OK, cmagic_memory_free_ext(memory_blocks[i]));
        TEST_ASSERT_FALSE(cmagic_memory_is_allocated(memory_blocks[i]));
    }

    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocated_bytes());
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocations());

    TEST_ASSERT_NOT_NULL(memory_blocks[0] = cmagic_memory_malloc(sizeof(int)));
    TEST_ASSERT_TRUE(cmagic_memory_is_allocated(memory_blocks[0]));
    TEST_ASSERT_EQUAL(CMAGIC_MEMORY_FREE_RESULT_OK, cmagic_memory_free_ext(memory_blocks[0]));
    TEST_ASSERT_FALSE(cmagic_memory_is_allocated(memory_blocks[0]));
}

static void test_Errors(void) {
    void *memptr = cmagic_memory_malloc(20);
    TEST_ASSERT_NOT_NULL(memptr);
    TEST_ASSERT_TRUE(cmagic_memory_is_allocated(memptr));
    TEST_ASSERT_EQUAL(CMAGIC_MEMORY_FREE_RESULT_OK_NULLPTR, cmagic_memory_free_ext(NULL));
    TEST_ASSERT_EQUAL(CMAGIC_MEMORY_FREE_RESULT_ERR_ADDRESS_OUTSIDE_MEMORY_POOL, cmagic_memory_free_ext((void *)1));
    TEST_ASSERT_EQUAL(CMAGIC_MEMORY_FREE_RESULT_OK, cmagic_memory_free_ext(memptr));
    TEST_ASSERT_FALSE(cmagic_memory_is_allocated(memptr));

    // Double free
    TEST_ASSERT_EQUAL(CMAGIC_MEMORY_FREE_RESULT_ERR_NOT_ALLOCATED_BEFORE, cmagic_memory_free_ext(memptr));
}

static void test_realloc(void) {
    void *memptr = cmagic_memory_malloc(70);
    TEST_ASSERT_NOT_NULL(memptr);
    TEST_ASSERT_EQUAL_size_t(70, cmagic_memory_get_allocated_bytes());
    TEST_ASSERT_TRUE(cmagic_memory_is_allocated(memptr));

    memptr = cmagic_memory_realloc(memptr, 35);
    TEST_ASSERT_NOT_NULL(memptr);
    TEST_ASSERT_EQUAL_size_t(35, cmagic_memory_get_allocated_bytes());
    TEST_ASSERT_TRUE(cmagic_memory_is_allocated(memptr));

    memptr = cmagic_memory_realloc(memptr, 140);
    TEST_ASSERT_NOT_NULL(memptr);
    TEST_ASSERT_EQUAL_size_t(140, cmagic_memory_get_allocated_bytes());
    TEST_ASSERT_TRUE(cmagic_memory_is_allocated(memptr));

    void *failed_memptr = cmagic_memory_realloc(memptr, 1500);
    TEST_ASSERT_NULL(failed_memptr);
    TEST_ASSERT_EQUAL_size_t(140, cmagic_memory_get_allocated_bytes());
    TEST_ASSERT_FALSE(cmagic_memory_is_allocated(failed_memptr));
    TEST_ASSERT_TRUE(cmagic_memory_is_allocated(memptr));

    TEST_ASSERT_EQUAL(CMAGIC_MEMORY_FREE_RESULT_OK, cmagic_memory_free_ext(memptr));
    TEST_ASSERT_FALSE(cmagic_memory_is_allocated(memptr));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_String);
    RUN_TEST(test_Fail);
    RUN_TEST(test_MemoryFull);
    RUN_TEST(test_Errors);
    RUN_TEST(test_realloc);
    return UNITY_END();
}
