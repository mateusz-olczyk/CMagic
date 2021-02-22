#include <stdint.h>
#include <string.h>
#include "cmagic/map.h"
#include "avl_tree.h"

#ifndef NDEBUG
static const int_least32_t MAP_MAGIC_VALUE = 'M' << 16 | 'A' << 8 | 'P';
#endif


typedef struct {
#ifndef NDEBUG
    int_least32_t magic_value;
#endif
    void *internal_avl_tree;
    size_t key_size;
    size_t value_size;
} map_descriptor_t;


void *
cmagic_map_new(size_t key_size, size_t value_size, cmagic_map_key_comparator_t key_comparator,
               const cmagic_memory_alloc_packet_t *alloc_packet) {
    assert(key_size > 0);
    assert(value_size > 0);
    assert(key_comparator);
    assert(alloc_packet);

    map_descriptor_t *map_desc =
        (map_descriptor_t *) alloc_packet->malloc_function(sizeof(map_descriptor_t));
    if (!map_desc) {
        return NULL;
    }

    *map_desc = (map_descriptor_t) {
#ifndef NDEBUG
        .magic_value = MAP_MAGIC_VALUE,
#endif
        .internal_avl_tree = cmagic_avl_tree_new(key_comparator, alloc_packet),
        .key_size = key_size,
        .value_size = value_size
    };

    if (!map_desc->internal_avl_tree) {
        alloc_packet->free_function(map_desc);
        return NULL;
    }

    return (void *)map_desc;
}

static map_descriptor_t *_get_map_descriptor(void *map_ptr) {
    assert(map_ptr);
    map_descriptor_t *result = (map_descriptor_t *)map_ptr;
    assert(result->magic_value == MAP_MAGIC_VALUE);
    return result;
}

static const cmagic_memory_alloc_packet_t *_get_alloc_packet(map_descriptor_t *map_desc) {
    return cmagic_avl_tree_get_alloc_packet(map_desc->internal_avl_tree);
}

void
cmagic_map_free(void *map_ptr) {
    map_descriptor_t *map_desc = _get_map_descriptor(map_ptr);
    const cmagic_memory_alloc_packet_t *alloc_packet = _get_alloc_packet(map_desc);
    cmagic_map_clear(map_ptr);
    cmagic_avl_tree_free(map_desc->internal_avl_tree);
    alloc_packet->free_function(map_desc);
}

cmagic_map_insert_result_t
cmagic_map_allocate(void *map_ptr, const void *key) {
    map_descriptor_t *map_desc = _get_map_descriptor(map_ptr);
    cmagic_avl_tree_insert_result_t tree_result =
        cmagic_avl_tree_insert(map_desc->internal_avl_tree, key, NULL);
    cmagic_map_insert_result_t result = {
        .inserted_or_existing = (cmagic_map_iterator_t)tree_result.inserted_or_existing,
        .already_exists = tree_result.already_exists
    };

    if (result.already_exists || !result.inserted_or_existing) {
        return result;
    }

    const cmagic_memory_alloc_packet_t *alloc_packet = _get_alloc_packet(map_desc);
    const void *allocated_key = alloc_packet->malloc_function(map_desc->key_size);
    if (!allocated_key) {
        cmagic_avl_tree_erase(map_desc->internal_avl_tree, key);
        result.inserted_or_existing = NULL;
        return result;
    }

    void *allocated_value = alloc_packet->malloc_function(map_desc->value_size);
    if (!allocated_value) {
        cmagic_avl_tree_erase(map_desc->internal_avl_tree, key);
        result.inserted_or_existing = NULL;
        alloc_packet->free_function((void *)allocated_key);
        return result;
    }

    result.inserted_or_existing->key = allocated_key;
    result.inserted_or_existing->value = allocated_value;
    return result;
}

cmagic_map_insert_result_t
cmagic_map_insert(void *map_ptr, const void *key, const void *value) {
    cmagic_map_insert_result_t result = cmagic_map_allocate(map_ptr, key);
    map_descriptor_t *map_desc = _get_map_descriptor(map_ptr);

    if (result.inserted_or_existing && !result.already_exists) {
        assert(result.inserted_or_existing->key);
        assert(result.inserted_or_existing->value);
        memcpy((void *)result.inserted_or_existing->key, key, map_desc->key_size);
        memcpy(result.inserted_or_existing->value, value, map_desc->value_size);
    }

    return result;
}

void
cmagic_map_erase(void *map_ptr, const void *key, cmagic_map_erase_destructor_t destructor) {
    map_descriptor_t *map_desc = _get_map_descriptor(map_ptr);
    cmagic_avl_tree_iterator_t found = cmagic_avl_tree_find(map_desc->internal_avl_tree, key);
    if (found) {
        const void *key_to_delete = found->key;
        void *value_to_delete = found->value;
        cmagic_avl_tree_erase(map_desc->internal_avl_tree, key);
        if (destructor) {
            destructor((void *)key_to_delete, value_to_delete);
        }
        const cmagic_memory_alloc_packet_t *alloc_packet = _get_alloc_packet(map_desc);
        alloc_packet->free_function((void *)key_to_delete);
        alloc_packet->free_function(value_to_delete);
    }
}

void
cmagic_map_clear(void *map_ptr) {
    map_descriptor_t *map_desc = _get_map_descriptor(map_ptr);
    const cmagic_memory_alloc_packet_t *alloc_packet = _get_alloc_packet(map_desc);
    for (cmagic_avl_tree_iterator_t it = cmagic_avl_tree_first(map_desc->internal_avl_tree);
         it;
         it = cmagic_avl_tree_iterator_next(it)) {
        alloc_packet->free_function((void *)it->key);
        alloc_packet->free_function(it->value);
    }
    cmagic_avl_tree_clear(map_desc->internal_avl_tree);
}

size_t
cmagic_map_size(void *map_ptr) {
    return cmagic_avl_tree_size(_get_map_descriptor(map_ptr)->internal_avl_tree);
}

cmagic_map_iterator_t
cmagic_map_first(void *map_ptr) {
    return (cmagic_map_iterator_t)
        cmagic_avl_tree_first(_get_map_descriptor(map_ptr)->internal_avl_tree);
}

cmagic_map_iterator_t
cmagic_map_last(void *map_ptr) {
    return (cmagic_map_iterator_t)
        cmagic_avl_tree_last(_get_map_descriptor(map_ptr)->internal_avl_tree);
}

cmagic_map_iterator_t
cmagic_map_iterator_next(cmagic_map_iterator_t iterator) {
    return (cmagic_map_iterator_t)
        cmagic_avl_tree_iterator_next((cmagic_avl_tree_iterator_t)iterator);
}

cmagic_map_iterator_t
cmagic_map_iterator_prev(cmagic_map_iterator_t iterator) {
    return (cmagic_map_iterator_t)
        cmagic_avl_tree_iterator_prev((cmagic_avl_tree_iterator_t)iterator);
}

cmagic_map_iterator_t
cmagic_map_find(void *map_ptr, const void *key) {
    return (cmagic_map_iterator_t)
        cmagic_avl_tree_find(_get_map_descriptor(map_ptr)->internal_avl_tree, key);
}

const cmagic_memory_alloc_packet_t *
cmagic_map_get_alloc_packet(void *map_ptr) {
    return _get_alloc_packet(_get_map_descriptor(map_ptr));
}
