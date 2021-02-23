#include <string.h>
#include <stdlib.h>
#include "cmagic/utils.h"
#include "avl_tree.h"
#include "unity.h"

void setUp(void) {
    static uint8_t memory_pool[3200];
    cmagic_memory_init(memory_pool, sizeof(memory_pool));
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocated_bytes());
}

void tearDown(void) {
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocated_bytes());
}

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
    CMAGIC_AVL_TREE(const char *) tree = CMAGIC_AVL_TREE_NEW(
        const char *, string_ptr_comparator, &CMAGIC_MEMORY_ALLOC_PACKET_CUSTOM_CMAGIC);
    TEST_ASSERT_NOT_NULL(tree);
    TEST_ASSERT_GREATER_THAN_size_t(0, cmagic_memory_get_allocated_bytes());
    TEST_ASSERT_EQUAL_size_t(0, CMAGIC_AVL_TREE_SIZE(tree));

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
        cmagic_avl_tree_insert_result_t insert_result = CMAGIC_AVL_TREE_INSERT(tree, &keys[i], NULL);
        TEST_ASSERT_NOT_NULL(insert_result.inserted_or_existing);
        TEST_ASSERT_FALSE(insert_result.already_exists);
        TEST_ASSERT_EQUAL_size_t(i + 1, CMAGIC_AVL_TREE_SIZE(tree));
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

    cmagic_avl_tree_insert_result_t insert_result = CMAGIC_AVL_TREE_INSERT(tree, &keys[0], NULL);
    TEST_ASSERT_NOT_NULL(insert_result.inserted_or_existing);
    TEST_ASSERT_TRUE(insert_result.already_exists);
    TEST_ASSERT_EQUAL_size_t(CMAGIC_UTILS_ARRAY_SIZE(keys), CMAGIC_AVL_TREE_SIZE(tree));

    CMAGIC_AVL_TREE_FREE(tree);
}

static int int_ptr_comparator(const void *key1, const void *key2) {
    TEST_ASSERT_NOT_NULL(key1);
    TEST_ASSERT_NOT_NULL(key2);
    int int_key1 = *(int *)key1;
    int int_key2 = *(int *)key2;
    return int_key1 - int_key2;
}

static void test_IntTree(void) {
    CMAGIC_AVL_TREE(int) tree = CMAGIC_AVL_TREE_NEW(int, int_ptr_comparator,
                                                    &CMAGIC_MEMORY_ALLOC_PACKET_CUSTOM_CMAGIC);
    TEST_ASSERT_NOT_NULL(tree);
    TEST_ASSERT_GREATER_THAN_size_t(0, cmagic_memory_get_allocated_bytes());

    const int keys[] = { 4, 1, -2, 2, 3, -5, -4, -3, -1, 0, 5 };

    int keys_sorted[CMAGIC_UTILS_ARRAY_SIZE(keys)];
    memcpy(keys_sorted, keys, sizeof(keys));
    qsort(keys_sorted, CMAGIC_UTILS_ARRAY_SIZE(keys_sorted), sizeof(*keys_sorted),
          int_ptr_comparator);

    TEST_ASSERT_EQUAL_size_t(0, CMAGIC_AVL_TREE_SIZE(tree));

    cmagic_avl_tree_insert_result_t insert_result;

    for (size_t i = 0; i < CMAGIC_UTILS_ARRAY_SIZE(keys); i++) {
        insert_result = CMAGIC_AVL_TREE_INSERT(tree, &keys[i], NULL);
        TEST_ASSERT_NOT_NULL(insert_result.inserted_or_existing);
        TEST_ASSERT_FALSE(insert_result.already_exists);
        TEST_ASSERT_EQUAL_size_t(i + 1, CMAGIC_AVL_TREE_SIZE(tree));
    }

    for (size_t i = 0; i < CMAGIC_UTILS_ARRAY_SIZE(keys); i++) {
        insert_result = CMAGIC_AVL_TREE_INSERT(tree, &keys[i], NULL);
        TEST_ASSERT_NOT_NULL(insert_result.inserted_or_existing);
        TEST_ASSERT_TRUE(insert_result.already_exists);
        TEST_ASSERT_EQUAL_size_t(CMAGIC_UTILS_ARRAY_SIZE(keys), CMAGIC_AVL_TREE_SIZE(tree));
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

    insert_result = CMAGIC_AVL_TREE_INSERT(tree, &keys[0], NULL);
    TEST_ASSERT_NOT_NULL(insert_result.inserted_or_existing);
    TEST_ASSERT_TRUE(insert_result.already_exists);
    TEST_ASSERT_EQUAL_size_t(CMAGIC_UTILS_ARRAY_SIZE(keys), CMAGIC_AVL_TREE_SIZE(tree));

    CMAGIC_AVL_TREE_FREE(tree);
}

static void test_FindValue(void) {
    CMAGIC_AVL_TREE(int) tree = CMAGIC_AVL_TREE_NEW(int, int_ptr_comparator,
                                                    &CMAGIC_MEMORY_ALLOC_PACKET_STD);

    const int keys[] = { 4, 2, 5, 1, 3, 6, 9, 0, 7, 8 };
    char values[] = { 'A'+4, 'A'+2, 'A'+5, 'A'+1, 'A'+3, 'A'+6, 'A'+9, 'A'+0, 'A'+7, 'A'+8 };
    for (size_t i = 0; i < CMAGIC_UTILS_ARRAY_SIZE(keys); i++) {
        cmagic_avl_tree_insert_result_t insert_result = CMAGIC_AVL_TREE_INSERT(tree, &keys[i],
                                                                               &values[i]);
        TEST_ASSERT_NOT_NULL(insert_result.inserted_or_existing);
        TEST_ASSERT_FALSE(insert_result.already_exists);
    }

    const char expected_values[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J' };
    size_t iteration;
    cmagic_avl_tree_iterator_t it;
    for (iteration = 0, it = CMAGIC_AVL_TREE_FIRST(tree);
         it;
         iteration++, it = CMAGIC_AVL_TREE_ITERATOR_NEXT(it)) {
        TEST_ASSERT_LESS_THAN_size_t(CMAGIC_UTILS_ARRAY_SIZE(keys), iteration);
        TEST_ASSERT_NOT_NULL(it->value);
        int key = CMAGIC_AVL_TREE_GET_KEY(int, it);
        char value = *(char *)it->value;
        TEST_ASSERT_EQUAL_size_t(iteration, (size_t)key);
        TEST_ASSERT_EQUAL_CHAR(expected_values[iteration], value);
    }

    it = CMAGIC_AVL_TREE_FIND(tree, &(int){3});
    TEST_ASSERT_NOT_NULL(it);
    TEST_ASSERT_NOT_NULL(it->key);
    TEST_ASSERT_EQUAL_INT(3, CMAGIC_AVL_TREE_GET_KEY(int, it));
    TEST_ASSERT_NOT_NULL(it->value);
    TEST_ASSERT_EQUAL_CHAR('D', *(char *)it->value);

    CMAGIC_AVL_TREE_FREE(tree);
}

static void test_InsertOneDeleteOne(void) {
    CMAGIC_AVL_TREE(int) tree = CMAGIC_AVL_TREE_NEW(int, int_ptr_comparator,
                                                    &CMAGIC_MEMORY_ALLOC_PACKET_CUSTOM_CMAGIC);
    TEST_ASSERT_NOT_NULL(tree);
    TEST_ASSERT_GREATER_THAN_size_t(0, cmagic_memory_get_allocated_bytes());

    cmagic_avl_tree_insert_result_t insert_result;
    cmagic_avl_tree_iterator_t it;

    TEST_ASSERT_EQUAL_size_t(0, CMAGIC_AVL_TREE_SIZE(tree));
    insert_result = CMAGIC_AVL_TREE_INSERT(tree, &(int){123}, NULL);
    TEST_ASSERT_NOT_NULL(insert_result.inserted_or_existing);
    TEST_ASSERT_FALSE(insert_result.already_exists);
    TEST_ASSERT_EQUAL_size_t(1, CMAGIC_AVL_TREE_SIZE(tree));
    it = CMAGIC_AVL_TREE_FIND(tree, &(int){123});
    TEST_ASSERT_NOT_NULL(it);
    TEST_ASSERT_NOT_NULL(it->key);
    TEST_ASSERT_EQUAL_INT(123, CMAGIC_AVL_TREE_GET_KEY(int, it));
    insert_result = CMAGIC_AVL_TREE_INSERT(tree, &(int){123}, NULL);
    TEST_ASSERT_NOT_NULL(insert_result.inserted_or_existing);
    TEST_ASSERT_TRUE(insert_result.already_exists);
    TEST_ASSERT_EQUAL_size_t(1, CMAGIC_AVL_TREE_SIZE(tree));
    CMAGIC_AVL_TREE_ERASE(tree, &(int){123});
    TEST_ASSERT_EQUAL_size_t(0, CMAGIC_AVL_TREE_SIZE(tree));
    it = CMAGIC_AVL_TREE_FIND(tree, &(int){123});
    TEST_ASSERT_NULL(it);
    insert_result = CMAGIC_AVL_TREE_INSERT(tree, &(int){123}, NULL);
    TEST_ASSERT_NOT_NULL(insert_result.inserted_or_existing);
    TEST_ASSERT_FALSE(insert_result.already_exists);
    TEST_ASSERT_EQUAL_size_t(1, CMAGIC_AVL_TREE_SIZE(tree));

    CMAGIC_AVL_TREE_FREE(tree);
}

static void test_InsertManyDeleteOne(void) {
    CMAGIC_AVL_TREE(int) tree = CMAGIC_AVL_TREE_NEW(int, int_ptr_comparator,
                                                    &CMAGIC_MEMORY_ALLOC_PACKET_CUSTOM_CMAGIC);
    TEST_ASSERT_NOT_NULL(tree);
    TEST_ASSERT_GREATER_THAN_size_t(0, cmagic_memory_get_allocated_bytes());

    const int keys[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    cmagic_avl_tree_insert_result_t insert_result;

    for (size_t i = 0; i < CMAGIC_UTILS_ARRAY_SIZE(keys); i++) {
        insert_result = CMAGIC_AVL_TREE_INSERT(tree, &keys[i], NULL);
        TEST_ASSERT_NOT_NULL(insert_result.inserted_or_existing);
        TEST_ASSERT_FALSE(insert_result.already_exists);
    }

    TEST_ASSERT_EQUAL_size_t(CMAGIC_UTILS_ARRAY_SIZE(keys), CMAGIC_AVL_TREE_SIZE(tree));
    CMAGIC_AVL_TREE_ERASE(tree, &(int){5});
    TEST_ASSERT_EQUAL_size_t(CMAGIC_UTILS_ARRAY_SIZE(keys) - 1, CMAGIC_AVL_TREE_SIZE(tree));
    size_t iteration;
    cmagic_avl_tree_iterator_t it;
    for (iteration = 1, it = CMAGIC_AVL_TREE_FIRST(tree);
         it;
         iteration++, it = CMAGIC_AVL_TREE_ITERATOR_NEXT(it)) {
        TEST_ASSERT_LESS_THAN_size_t(CMAGIC_UTILS_ARRAY_SIZE(keys), iteration);
        TEST_ASSERT_NOT_NULL(it->key);
        if (iteration < 5) {
            TEST_ASSERT_EQUAL_INT(iteration, CMAGIC_AVL_TREE_GET_KEY(int, it));
        } else {
            TEST_ASSERT_EQUAL_INT(iteration + 1, CMAGIC_AVL_TREE_GET_KEY(int, it));
        }
    }

    CMAGIC_AVL_TREE_FREE(tree);
}

static void test_Clear(void) {
    CMAGIC_AVL_TREE(int) tree = CMAGIC_AVL_TREE_NEW(int, int_ptr_comparator,
                                                    &CMAGIC_MEMORY_ALLOC_PACKET_CUSTOM_CMAGIC);
    TEST_ASSERT_NOT_NULL(tree);
    TEST_ASSERT_GREATER_THAN_size_t(0, cmagic_memory_get_allocated_bytes());

    const int keys[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    cmagic_avl_tree_insert_result_t insert_result;

    for (size_t i = 0; i < CMAGIC_UTILS_ARRAY_SIZE(keys); i++) {
        insert_result = CMAGIC_AVL_TREE_INSERT(tree, &keys[i], NULL);
        TEST_ASSERT_NOT_NULL(insert_result.inserted_or_existing);
        TEST_ASSERT_FALSE(insert_result.already_exists);
    }

    TEST_ASSERT_EQUAL_size_t(CMAGIC_UTILS_ARRAY_SIZE(keys), CMAGIC_AVL_TREE_SIZE(tree));
    CMAGIC_AVL_TREE_CLEAR(tree);
    TEST_ASSERT_EQUAL_size_t(0, CMAGIC_AVL_TREE_SIZE(tree));

    for (size_t i = 0; i < CMAGIC_UTILS_ARRAY_SIZE(keys); i++) {
        insert_result = CMAGIC_AVL_TREE_INSERT(tree, &keys[i], NULL);
        TEST_ASSERT_NOT_NULL(insert_result.inserted_or_existing);
        TEST_ASSERT_FALSE(insert_result.already_exists);
    }

    TEST_ASSERT_EQUAL_size_t(CMAGIC_UTILS_ARRAY_SIZE(keys), CMAGIC_AVL_TREE_SIZE(tree));

    CMAGIC_AVL_TREE_FREE(tree);
}

static void test_DeleteNodeWithTwoKids(void) {
    CMAGIC_AVL_TREE(int) tree = CMAGIC_AVL_TREE_NEW(int, int_ptr_comparator,
                                                    &CMAGIC_MEMORY_ALLOC_PACKET_CUSTOM_CMAGIC);
    TEST_ASSERT_NOT_NULL(tree);
    TEST_ASSERT_GREATER_THAN_size_t(0, cmagic_memory_get_allocated_bytes());

    const int keys[] = { 2, 1, 3 };
    for (size_t i = 0; i < CMAGIC_UTILS_ARRAY_SIZE(keys); i++) {
        cmagic_avl_tree_insert_result_t insert_result = CMAGIC_AVL_TREE_INSERT(tree, &keys[i], NULL);
        TEST_ASSERT_NOT_NULL(insert_result.inserted_or_existing);
        TEST_ASSERT_FALSE(insert_result.already_exists);
    }

    TEST_ASSERT_EQUAL_size_t(CMAGIC_UTILS_ARRAY_SIZE(keys), CMAGIC_AVL_TREE_SIZE(tree));
    CMAGIC_AVL_TREE_ERASE(tree, &(int){2});
    TEST_ASSERT_EQUAL_size_t(CMAGIC_UTILS_ARRAY_SIZE(keys) - 1, CMAGIC_AVL_TREE_SIZE(tree));

    cmagic_avl_tree_iterator_t it = CMAGIC_AVL_TREE_FIRST(tree);
    TEST_ASSERT_EQUAL_INT(1, CMAGIC_AVL_TREE_GET_KEY(int, it));
    it = CMAGIC_AVL_TREE_ITERATOR_NEXT(it);
    TEST_ASSERT_EQUAL_INT(3, CMAGIC_AVL_TREE_GET_KEY(int, it));
    it = CMAGIC_AVL_TREE_ITERATOR_NEXT(it);
    TEST_ASSERT_NULL(it);

    CMAGIC_AVL_TREE_FREE(tree);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_StringTree);
    RUN_TEST(test_IntTree);
    RUN_TEST(test_FindValue);
    RUN_TEST(test_InsertOneDeleteOne);
    RUN_TEST(test_InsertManyDeleteOne);
    RUN_TEST(test_Clear);
    RUN_TEST(test_DeleteNodeWithTwoKids);
    return UNITY_END();
}
