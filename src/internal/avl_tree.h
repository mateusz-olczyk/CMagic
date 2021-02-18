#ifndef CMAGIC_AVL_TREE_H
#define CMAGIC_AVL_TREE_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include "cmagic/memory.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*cmagic_avl_tree_key_comparator_t)(const void *key1, const void *key2);

void **
cmagic_avl_tree_new(cmagic_avl_tree_key_comparator_t key_comparator,
                    const cmagic_memory_alloc_packet_t *alloc_packet);

void
cmagic_avl_tree_free(void **avl_tree);

typedef struct {
    const void *key;
    void *value;
} *cmagic_avl_tree_iterator_t;

typedef struct {
    cmagic_avl_tree_iterator_t inserted_or_existing;
    bool already_exists;
} cmagic_avl_tree_insert_result_t;

cmagic_avl_tree_insert_result_t
cmagic_avl_tree_insert(void **avl_tree, const void *key, void *value);

void
cmagic_avl_tree_erase(void **avl_tree, const void *key);

void
cmagic_avl_tree_clear(void **avl_tree);

size_t
cmagic_avl_tree_size(void **avl_tree);

cmagic_avl_tree_iterator_t
cmagic_avl_tree_first(void **avl_tree);

cmagic_avl_tree_iterator_t
cmagic_avl_tree_last(void **avl_tree);

cmagic_avl_tree_iterator_t
cmagic_avl_tree_iterator_next(cmagic_avl_tree_iterator_t iterator);

cmagic_avl_tree_iterator_t
cmagic_avl_tree_iterator_prev(cmagic_avl_tree_iterator_t iterator);

cmagic_avl_tree_iterator_t
cmagic_avl_tree_find(void **avl_tree, const void *key);

#define CMAGIC_AVL_TREE(key_type) key_type**

#define CMAGIC_AVL_TREE_NEW(key_type, key_comparator, alloc_packet) \
    ((CMAGIC_AVL_TREE(key_type))cmagic_avl_tree_new((key_comparator), (alloc_packet)))

#define CMAGIC_AVL_TREE_FREE(avl_tree) cmagic_avl_tree_free((void**)(avl_tree))

#define CMAGIC_AVL_TREE_INSERT(avl_tree, key, value) \
    (CMAGIC_UTILS_ASSERT_SAME_TYPE(**(avl_tree), *(key)), \
    cmagic_avl_tree_insert((void**)(avl_tree), (key), (value)))

#define CMAGIC_AVL_TREE_ERASE(avl_tree, key) \
    (CMAGIC_UTILS_ASSERT_SAME_TYPE(**(avl_tree), *(key)), \
    cmagic_avl_tree_erase((void**)(avl_tree), (key)))

#define CMAGIC_AVL_TREE_CLEAR(avl_tree) cmagic_avl_tree_clear((void**)(avl_tree))

#define CMAGIC_AVL_TREE_SIZE(avl_tree) cmagic_avl_tree_size((void**)(avl_tree))

#define CMAGIC_AVL_TREE_FIRST(avl_tree) cmagic_avl_tree_first((void**)(avl_tree))

#define CMAGIC_AVL_TREE_LAST(avl_tree) cmagic_avl_tree_last((void**)(avl_tree))

#define CMAGIC_AVL_TREE_ITERATOR_NEXT(iterator) cmagic_avl_tree_iterator_next(iterator)

#define CMAGIC_AVL_TREE_ITERATOR_PREV(iterator) cmagic_avl_tree_iterator_prev(iterator)

#define CMAGIC_AVL_TREE_GET_KEY(key_type, iterator) \
    (assert(iterator), assert((iterator)->key), *((const key_type*)(iterator)->key))

#define CMAGIC_AVL_TREE_FIND(avl_tree, key) (CMAGIC_UTILS_ASSERT_SAME_TYPE(**(avl_tree), *(key)), \
    cmagic_avl_tree_find((void**)(avl_tree), (key)))

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CMAGIC_AVL_TREE_H */
