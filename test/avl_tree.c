#include <string.h>
#include <stdlib.h>
#include "cmagic/utils.h"
#include "avl_tree.h"
#include "unity.h"

void setUp(void) {
    static uint8_t memory_pool[2000];
    cmagic_memory_init(memory_pool, sizeof(memory_pool));
}

void tearDown(void) {}

static int string_ptr_comparator(const void *key1, const void *key2) {
    TEST_ASSERT_NOT_NULL(key1);
    TEST_ASSERT_NOT_NULL(key2);
    const char *string_key1 = *(const char **)key1;
    const char *string_key2 = *(const char **)key2;
    TEST_ASSERT_NOT_NULL(string_key1);
    TEST_ASSERT_NOT_NULL(string_key2);
    return strcmp(string_key1, string_key2);
}

static void test_StringTree(void) {
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocated_bytes());

    CMAGIC_AVL_TREE(const char *) tree = CMAGIC_AVL_TREE_NEW(
        const char *, string_ptr_comparator, &CMAGIC_MEMORY_ALLOC_PACKET_CUSTOM_CMAGIC);
    TEST_ASSERT_NOT_NULL(tree);
    TEST_ASSERT_GREATER_THAN_size_t(0, cmagic_memory_get_allocated_bytes());

    const char *const keys[] = {
        "Oliver", "Jake", "Noah", "James", "Jack", "Connor", "Liam", "John", "Harry", "Callum",
        "Mason", "Robert", "Jacob", "Michael", "Charlie", "Kyle", "William", "Thomas", "Joe",
        "Ethan", "David", "George", "Reece", "Richard", "Oscar", "Rhys", "Alexander", "Joseph",
        "Charles", "Damian", "Daniel"
    };

    const char *keys_sorted[CMAGIC_UTILS_ARRAY_SIZE(keys)];
    memcpy(keys_sorted, keys, sizeof(keys));
    qsort(keys_sorted, CMAGIC_UTILS_ARRAY_SIZE(keys_sorted), sizeof(*keys_sorted),
          string_ptr_comparator);


    for (size_t i = 0; i < CMAGIC_UTILS_ARRAY_SIZE(keys); i++) {
        TEST_ASSERT_TRUE(CMAGIC_AVL_TREE_INSERT(tree, &keys[i], NULL));
    }

    size_t iteration;
    cmagic_avl_tree_iterator_t it;
    for (iteration = 0, it = CMAGIC_AVL_TREE_FIRST(tree);
         it;
         iteration++, it = CMAGIC_AVL_TREE_ITERATOR_NEXT(it)) {
        TEST_ASSERT_LESS_THAN_size_t(CMAGIC_UTILS_ARRAY_SIZE(keys), iteration);
        const char *string_key = CMAGIC_AVL_TREE_GET_KEY(const char *, it);
        TEST_ASSERT_NOT_NULL(string_key);
        TEST_ASSERT_EQUAL_STRING(keys_sorted[iteration], string_key);
    }

    TEST_ASSERT_FALSE(CMAGIC_AVL_TREE_INSERT(tree, &keys[0], NULL));

    CMAGIC_AVL_TREE_FREE(tree);
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocated_bytes());
}

static int int_ptr_comparator(const void *key1, const void *key2) {
    TEST_ASSERT_NOT_NULL(key1);
    TEST_ASSERT_NOT_NULL(key2);
    int int_key1 = *(int *)key1;
    int int_key2 = *(int *)key2;
    return int_key1 - int_key2;
}

static void test_IntTree(void) {
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocated_bytes());

    CMAGIC_AVL_TREE(int) tree = CMAGIC_AVL_TREE_NEW(int, int_ptr_comparator,
                                                    &CMAGIC_MEMORY_ALLOC_PACKET_CUSTOM_CMAGIC);
    TEST_ASSERT_NOT_NULL(tree);
    TEST_ASSERT_GREATER_THAN_size_t(0, cmagic_memory_get_allocated_bytes());

    const int keys[] = { 4, 1, -2, 2, 3, -5, -4, -3, -1, 0, 5 };

    int keys_sorted[CMAGIC_UTILS_ARRAY_SIZE(keys)];
    memcpy(keys_sorted, keys, sizeof(keys));
    qsort(keys_sorted, CMAGIC_UTILS_ARRAY_SIZE(keys_sorted), sizeof(*keys_sorted),
          int_ptr_comparator);


    for (size_t i = 0; i < CMAGIC_UTILS_ARRAY_SIZE(keys); i++) {
        TEST_ASSERT_TRUE(CMAGIC_AVL_TREE_INSERT(tree, &keys[i], NULL));
    }

    for (size_t i = 0; i < CMAGIC_UTILS_ARRAY_SIZE(keys); i++) {
        TEST_ASSERT_FALSE(CMAGIC_AVL_TREE_INSERT(tree, &keys[i], NULL));
    }

    size_t iteration;
    cmagic_avl_tree_iterator_t it;
    for (iteration = 0, it = CMAGIC_AVL_TREE_FIRST(tree);
         it;
         iteration++, it = CMAGIC_AVL_TREE_ITERATOR_NEXT(it)) {
        TEST_ASSERT_LESS_THAN_size_t(CMAGIC_UTILS_ARRAY_SIZE(keys), iteration);
        int int_key = CMAGIC_AVL_TREE_GET_KEY(int, it);
        TEST_ASSERT_EQUAL_INT(keys_sorted[iteration], int_key);
    }

    for (iteration = 0, it = CMAGIC_AVL_TREE_LAST(tree);
         it;
         iteration++, it = CMAGIC_AVL_TREE_ITERATOR_PREV(it)) {
        TEST_ASSERT_LESS_THAN_size_t(CMAGIC_UTILS_ARRAY_SIZE(keys), iteration);
        int int_key = CMAGIC_AVL_TREE_GET_KEY(int, it);
        TEST_ASSERT_EQUAL_INT(keys_sorted[CMAGIC_UTILS_ARRAY_SIZE(keys) - 1 - iteration], int_key);
    }

    TEST_ASSERT_FALSE(CMAGIC_AVL_TREE_INSERT(tree, &keys[0], NULL));

    CMAGIC_AVL_TREE_FREE(tree);
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocated_bytes());
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_StringTree);
    RUN_TEST(test_IntTree);
    return UNITY_END();
}
