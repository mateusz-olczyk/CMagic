/**
 * @file    map.h
 * @brief   Implementation of a @b map container.
 * @details Please <b>use provided macros</b> instead of raw functions to gain additional type
 *          checks.
 */

#ifndef CMAGIC_MAP_H
#define CMAGIC_MAP_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include "cmagic/memory.h"
#include "cmagic/utils.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*cmagic_map_key_comparator_t)(const void *key1, const void *key2);

void *
cmagic_map_new(size_t key_size, size_t value_size, cmagic_map_key_comparator_t key_comparator,
               const cmagic_memory_alloc_packet_t *alloc_packet);

void
cmagic_map_free(void *map_ptr);

typedef struct {
    const void *key;
    void *value;
} *cmagic_map_iterator_t;

typedef struct {
    cmagic_map_iterator_t inserted_or_existing;
    bool already_exists;
} cmagic_map_insert_result_t;

cmagic_map_insert_result_t
cmagic_map_allocate(void *map_ptr, const void *key);

cmagic_map_insert_result_t
cmagic_map_insert(void *map_ptr, const void *key, const void *value);

void
cmagic_map_erase(void *map_ptr, const void *key);

void
cmagic_map_clear(void *map_ptr);

size_t
cmagic_map_size(void *map_ptr);

cmagic_map_iterator_t
cmagic_map_first(void *map_ptr);

cmagic_map_iterator_t
cmagic_map_last(void *map_ptr);

cmagic_map_iterator_t
cmagic_map_iterator_next(cmagic_map_iterator_t iterator);

cmagic_map_iterator_t
cmagic_map_iterator_prev(cmagic_map_iterator_t iterator);

cmagic_map_iterator_t
cmagic_map_find(void *map_ptr, const void *key);

const cmagic_memory_alloc_packet_t *
cmagic_map_get_alloc_packet(void *map_ptr);

#define CMAGIC_MAP(key_type) key_type*

#define CMAGIC_MAP_NEW(key_type, value_type, key_comparator, alloc_packet) ((CMAGIC_MAP(key_type)) \
    cmagic_map_new(sizeof(key_type), sizeof(value_type), (key_comparator), (alloc_packet)))

#define CMAGIC_MAP_FREE(cmagic_map) cmagic_map_free((void*)(cmagic_map))

#define CMAGIC_MAP_ALLOCATE(cmagic_map, key) \
    (CMAGIC_UTILS_ASSERT_SAME_TYPE(*(cmagic_map), *(key)), \
    cmagic_map_allocate((void*)(cmagic_map), (key)))

#define CMAGIC_MAP_INSERT(cmagic_map, key, value) \
    (CMAGIC_UTILS_ASSERT_SAME_TYPE(*(cmagic_map), *(key)), \
    cmagic_map_insert((void*)(cmagic_map), (key), (value)))

#define CMAGIC_MAP_ERASE(cmagic_map, key) \
    (CMAGIC_UTILS_ASSERT_SAME_TYPE(*(cmagic_map), *(key)), \
    cmagic_map_erase((void*)(cmagic_map), (key)))

#define CMAGIC_MAP_CLEAR(cmagic_map) cmagic_map_clear((void*)(cmagic_map))

#define CMAGIC_MAP_SIZE(cmagic_map) cmagic_map_size((void*)(cmagic_map))

#define CMAGIC_MAP_FIRST(cmagic_map) cmagic_map_first((void*)(cmagic_map))

#define CMAGIC_MAP_LAST(cmagic_map) cmagic_map_last((void*)(cmagic_map))

#define CMAGIC_MAP_ITERATOR_NEXT(iterator) cmagic_map_iterator_next(iterator)

#define CMAGIC_MAP_ITERATOR_PREV(iterator) cmagic_map_iterator_prev(iterator)

#define CMAGIC_MAP_FIND(cmagic_map, key) (CMAGIC_UTILS_ASSERT_SAME_TYPE(*(cmagic_map), *(key)), \
    cmagic_map_find((void*)(cmagic_map), (key)))

#define CMAGIC_MAP_GET_KEY(key_type, iterator) \
    (assert(iterator), assert((iterator)->key), *((const key_type*)(iterator)->key))

#define CMAGIC_MAP_GET_VALUE(value_type, iterator) \
    (assert(iterator), assert((iterator)->value), *((value_type*)(iterator)->value))

#define CMAGIC_MAP_GET_ALLOC_PACKET(cmagic_map) \
    cmagic_map_get_alloc_packet((void*)(cmagic_map))

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CMAGIC_MAP_H */
