#include <assert.h>
#include "cmagic/memory.h"
#include "cmagic/vector.h"
#include "unity.h"

void setUp(void) {
    static uint8_t memory_pool[1000];
    cmagic_memory_init(memory_pool, sizeof(memory_pool));
}

void tearDown(void) {}

static void test_Empty(void) {
    CMAGIC_VECTOR(int) vector = CMAGIC_VECTOR_NEW(int, &CMAGIC_MEMORY_ALLOC_PACKET_STD);
    TEST_ASSERT_NOT_NULL(vector);
    TEST_ASSERT_EQUAL_size_t(0, CMAGIC_VECTOR_SIZE(vector));
    CMAGIC_VECTOR_FREE(vector);
}

static void test_Single(void) {
    CMAGIC_VECTOR(int) vector = CMAGIC_VECTOR_NEW(int, &CMAGIC_MEMORY_ALLOC_PACKET_STD);
    TEST_ASSERT_NOT_NULL(vector);
    TEST_ASSERT_TRUE(CMAGIC_VECTOR_PUSH_BACK(vector, &(int){123}));
    TEST_ASSERT_EQUAL_size_t(1, CMAGIC_VECTOR_SIZE(vector));
    TEST_ASSERT_EQUAL_INT(123, CMAGIC_VECTOR_DATA(vector)[0]);
    CMAGIC_VECTOR_FREE(vector);
}

static void test_100(void) {
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocations());
    CMAGIC_VECTOR(int) vector = CMAGIC_VECTOR_NEW(int, &CMAGIC_MEMORY_ALLOC_PACKET_CUSTOM_CMAGIC);
    TEST_ASSERT_NOT_NULL(vector);
    TEST_ASSERT_GREATER_THAN_size_t(0, cmagic_memory_get_allocations());
    for (int i = 1; i <= 100; i++) {
        TEST_ASSERT_TRUE(CMAGIC_VECTOR_PUSH_BACK(vector, &i));
    }
    TEST_ASSERT_EQUAL_size_t(100, CMAGIC_VECTOR_SIZE(vector));
    for (int i = 0; i < 100; i++) {
        TEST_ASSERT_EQUAL_INT(i + 1, CMAGIC_VECTOR_DATA(vector)[i]);
    }
    CMAGIC_VECTOR_FREE(vector);
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocations());
}

static void test_PushMaximum(void) {
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocations());
    CMAGIC_VECTOR(int) vector = CMAGIC_VECTOR_NEW(int, &CMAGIC_MEMORY_ALLOC_PACKET_CUSTOM_CMAGIC);
    const size_t empty_vector_size = cmagic_memory_get_allocated_bytes();
    while (CMAGIC_VECTOR_PUSH_BACK(vector, &(int){123})) {
        TEST_ASSERT_EQUAL_size_t(2, cmagic_memory_get_allocations());
    }
    TEST_ASSERT_GREATER_THAN_size_t(empty_vector_size, cmagic_memory_get_allocated_bytes());
    CMAGIC_VECTOR_FREE(vector);
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocations());
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_Empty);
    RUN_TEST(test_Single);
    RUN_TEST(test_100);
    RUN_TEST(test_PushMaximum);
    return UNITY_END();
}
