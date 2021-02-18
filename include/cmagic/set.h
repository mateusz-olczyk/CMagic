/**
 * @file    set.h
 * @brief   Implementation of a @b set container.
 * @details Please <b>use provided macros</b> instead of raw functions to gain additional type
 *          checks.
 */

#ifndef CMAGIC_SET_H
#define CMAGIC_SET_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include "cmagic/memory.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*cmagic_set_key_comparator_t)(const void *key1, const void *key2);

void *
cmagic_set_new(size_t key_size, cmagic_set_key_comparator_t key_comparator,
               const cmagic_memory_alloc_packet_t *alloc_packet);

void
cmagic_set_free(void *set_ptr);

typedef struct {
    const void *key;
} *cmagic_set_iterator_t;

typedef struct {
    cmagic_set_iterator_t inserted_or_existing;
    bool already_exists;
} cmagic_set_insert_result_t;

cmagic_set_insert_result_t
cmagic_set_allocate(void *set_ptr, const void *key);

cmagic_set_insert_result_t
cmagic_set_insert(void *set_ptr, const void *key);

void
cmagic_set_erase(void *set_ptr, const void *key);

void
cmagic_set_clear(void *set_ptr);

size_t
cmagic_set_size(void *set_ptr);

cmagic_set_iterator_t
cmagic_set_first(void *set_ptr);

cmagic_set_iterator_t
cmagic_set_last(void *set_ptr);

cmagic_set_iterator_t
cmagic_set_iterator_next(cmagic_set_iterator_t iterator);

cmagic_set_iterator_t
cmagic_set_iterator_prev(cmagic_set_iterator_t iterator);

cmagic_set_iterator_t
cmagic_set_find(void *set_ptr, const void *key);

#define CMAGIC_SET(key_type) key_type*

#define CMAGIC_SET_NEW(key_type, key_comparator, alloc_packet) \
    ((CMAGIC_SET(key_type))cmagic_set_new(sizeof(key_type), (key_comparator), (alloc_packet)))

#define CMAGIC_SET_FREE(cmagic_set) cmagic_set_free((void*)(cmagic_set))

#define CMAGIC_SET_ALLOCATE(cmagic_set, key) \
    (CMAGIC_UTILS_ASSERT_SAME_TYPE(*(cmagic_set), *(key)), \
    cmagic_set_allocate((void*)(cmagic_set), (key)))

#define CMAGIC_SET_INSERT(cmagic_set, key) \
    (CMAGIC_UTILS_ASSERT_SAME_TYPE(*(cmagic_set), *(key)), \
    cmagic_set_insert((void*)(cmagic_set), (key)))

#define CMAGIC_SET_ERASE(cmagic_set, key) \
    (CMAGIC_UTILS_ASSERT_SAME_TYPE(*(cmagic_set), *(key)), \
    cmagic_set_erase((void*)(cmagic_set), (key)))

#define CMAGIC_SET_CLEAR(cmagic_set) cmagic_set_clear((void*)(cmagic_set))

#define CMAGIC_SET_SIZE(cmagic_set) cmagic_set_size((void*)(cmagic_set))

#define CMAGIC_SET_FIRST(cmagic_set) cmagic_set_first((void*)(cmagic_set))

#define CMAGIC_SET_LAST(cmagic_set) cmagic_set_last((void*)(cmagic_set))

#define CMAGIC_SET_ITERATOR_NEXT(iterator) cmagic_set_iterator_next(iterator)

#define CMAGIC_SET_ITERATOR_PREV(iterator) cmagic_set_iterator_prev(iterator)

#define CMAGIC_SET_FIND(cmagic_set, key) (CMAGIC_UTILS_ASSERT_SAME_TYPE(*(cmagic_set), *(key)), \
    cmagic_set_find((void*)(cmagic_set), (key)))

#define CMAGIC_SET_GET_KEY(key_type, iterator) \
    (assert(iterator), assert((iterator)->key), *((const key_type*)(iterator)->key))

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CMAGIC_SET_H */
