#include "cmagic/set.h"
#include "cmagic/utils.h"
#include "unity.h"

void setUp(void) {
    static uint8_t memory_pool[5000];
    cmagic_memory_init(memory_pool, sizeof(memory_pool));
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocated_bytes());
}

void tearDown(void) {
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocated_bytes());
}

static int int_ptr_comparator(const void *key1, const void *key2) {
    TEST_ASSERT_NOT_NULL(key1);
    TEST_ASSERT_NOT_NULL(key2);
    int int_key1 = *(int *)key1;
    int int_key2 = *(int *)key2;
    return int_key1 - int_key2;
}

static void test_Sorting(void) {
    const int numbers[] = { 3, 8, 5, 2, 1, 7, 10, 4, 9, 6 };

    CMAGIC_SET(int) int_set = CMAGIC_SET_NEW(int, int_ptr_comparator,
                                             &CMAGIC_MEMORY_ALLOC_PACKET_CUSTOM_CMAGIC);
    for (size_t i = 0; i < CMAGIC_UTILS_ARRAY_SIZE(numbers); i++) {
        cmagic_set_insert_result_t insert_result = CMAGIC_SET_INSERT(int_set, &numbers[i]);
        TEST_ASSERT_NOT_NULL(insert_result.inserted_or_existing);
        TEST_ASSERT_FALSE(insert_result.already_exists);
    }

    cmagic_set_iterator_t it;
    int iteration;
    for (it = CMAGIC_SET_FIRST(int_set), iteration = 1;
         it;
         it = CMAGIC_SET_ITERATOR_NEXT(it), iteration++) {
        TEST_ASSERT_GREATER_OR_EQUAL_size_t((size_t)iteration, CMAGIC_SET_SIZE(int_set));
        TEST_ASSERT_EQUAL_INT(iteration, CMAGIC_SET_GET_KEY(int, it));
    }

    TEST_ASSERT_EQUAL_size_t(CMAGIC_UTILS_ARRAY_SIZE(numbers), CMAGIC_SET_SIZE(int_set));
    CMAGIC_SET_CLEAR(int_set);
    TEST_ASSERT_EQUAL_size_t(0, CMAGIC_SET_SIZE(int_set));

    CMAGIC_SET_FREE(int_set);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_Sorting);
    return UNITY_END();
}
