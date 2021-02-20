#include "cmagic/map.h"
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

static void test_Association(void) {
    const int keys[] = { 3, 8, 5, 2, 1, 7, 10, 4, 9, 6 };
    const char *values[] = {
        "three", "eight", "five", "two", "one", "seven", "ten", "four", "nine", "six"
    };

    CMAGIC_MAP(int) int_str_map = CMAGIC_MAP_NEW(int, const char *, int_ptr_comparator,
                                                 &CMAGIC_MEMORY_ALLOC_PACKET_CUSTOM_CMAGIC);
    for (size_t i = 0; i < CMAGIC_UTILS_ARRAY_SIZE(keys); i++) {
        cmagic_map_insert_result_t insert_result =
            CMAGIC_MAP_INSERT(int_str_map, &keys[i], &values[i]);
        TEST_ASSERT_NOT_NULL(insert_result.inserted_or_existing);
        TEST_ASSERT_FALSE(insert_result.already_exists);
    }

    cmagic_map_iterator_t it;
    int iteration;
    for (it = CMAGIC_MAP_FIRST(int_str_map), iteration = 1;
         it;
         it = CMAGIC_MAP_ITERATOR_NEXT(it), iteration++) {
        TEST_ASSERT_GREATER_OR_EQUAL_size_t((size_t)iteration, CMAGIC_MAP_SIZE(int_str_map));
        TEST_ASSERT_EQUAL_INT(iteration, CMAGIC_MAP_GET_KEY(int, it));

        switch (CMAGIC_MAP_GET_KEY(int, it)) {
        case 1:
            TEST_ASSERT_EQUAL_STRING("one", CMAGIC_MAP_GET_VALUE(const char *, it));
            break;
        case 2:
            TEST_ASSERT_EQUAL_STRING("two", CMAGIC_MAP_GET_VALUE(const char *, it));
            break;
        case 3:
            TEST_ASSERT_EQUAL_STRING("three", CMAGIC_MAP_GET_VALUE(const char *, it));
            break;
        case 4:
            TEST_ASSERT_EQUAL_STRING("four", CMAGIC_MAP_GET_VALUE(const char *, it));
            break;
        case 5:
            TEST_ASSERT_EQUAL_STRING("five", CMAGIC_MAP_GET_VALUE(const char *, it));
            break;
        case 6:
            TEST_ASSERT_EQUAL_STRING("six", CMAGIC_MAP_GET_VALUE(const char *, it));
            break;
        case 7:
            TEST_ASSERT_EQUAL_STRING("seven", CMAGIC_MAP_GET_VALUE(const char *, it));
            break;
        case 8:
            TEST_ASSERT_EQUAL_STRING("eight", CMAGIC_MAP_GET_VALUE(const char *, it));
            break;
        case 9:
            TEST_ASSERT_EQUAL_STRING("nine", CMAGIC_MAP_GET_VALUE(const char *, it));
            break;
        case 10:
            TEST_ASSERT_EQUAL_STRING("ten", CMAGIC_MAP_GET_VALUE(const char *, it));
            break;
        }
    }

    TEST_ASSERT_EQUAL_size_t(CMAGIC_UTILS_ARRAY_SIZE(keys), CMAGIC_MAP_SIZE(int_str_map));
    CMAGIC_MAP_CLEAR(int_str_map);
    TEST_ASSERT_EQUAL_size_t(0, CMAGIC_MAP_SIZE(int_str_map));

    CMAGIC_MAP_FREE(int_str_map);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_Association);
    return UNITY_END();
}
