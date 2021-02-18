#include <stdint.h>
#include <string.h>
#include "avl_tree.h"
#include "cmagic/set.h"

#ifndef NDEBUG
static const int_least32_t SET_MAGIC_VALUE = 'S' << 16 | 'E' << 8 | 'T';
#endif

typedef struct {
#ifndef NDEBUG
    int_least32_t magic_value;
#endif
    void *internal_avl_tree;
    size_t key_size;
} set_descriptor_t;


void *
cmagic_set_new(size_t key_size, cmagic_set_key_comparator_t key_comparator,
               const cmagic_memory_alloc_packet_t *alloc_packet) {
    assert(key_size > 0);
    assert(key_comparator);
    assert(alloc_packet);

    set_descriptor_t *set_desc =
        (set_descriptor_t *) alloc_packet->malloc_function(sizeof(set_descriptor_t));
    if (!set_desc) {
        return NULL;
    }

    *set_desc = (set_descriptor_t) {
#ifndef NDEBUG
        .magic_value = SET_MAGIC_VALUE,
#endif
        .internal_avl_tree = cmagic_avl_tree_new(key_comparator, alloc_packet),
        .key_size = key_size
    };

    if (!set_desc->internal_avl_tree) {
        alloc_packet->free_function(set_desc);
        return NULL;
    }

    return (void *)set_desc;
}

static set_descriptor_t *_get_set_descriptor(void *set_ptr) {
    assert(set_ptr);
    set_descriptor_t *result = (set_descriptor_t *)set_ptr;
    assert(result->magic_value == SET_MAGIC_VALUE);
    return result;
}

static const cmagic_memory_alloc_packet_t *_get_alloc_packet(set_descriptor_t *set_desc) {
    return cmagic_avl_tree_get_alloc_packet(set_desc->internal_avl_tree);
}

void
cmagic_set_free(void *set_ptr) {
    set_descriptor_t *set_desc = _get_set_descriptor(set_ptr);
    cmagic_set_clear(set_ptr);
    cmagic_avl_tree_free(set_desc->internal_avl_tree);
    _get_alloc_packet(set_desc)->free_function(set_desc);
}

cmagic_set_insert_result_t
cmagic_set_allocate(void *set_ptr, const void *key) {
    set_descriptor_t *set_desc = _get_set_descriptor(set_ptr);
    cmagic_avl_tree_insert_result_t tree_result =
        cmagic_avl_tree_insert(set_desc->internal_avl_tree, key, NULL);
    cmagic_set_insert_result_t result = {
        .inserted_or_existing = (cmagic_set_iterator_t)tree_result.inserted_or_existing,
        .already_exists = tree_result.already_exists
    };

    if (result.already_exists || !result.inserted_or_existing) {
        return result;
    }

    const void *allocated_key = _get_alloc_packet(set_desc)->malloc_function(set_desc->key_size);
    if (!allocated_key) {
        cmagic_avl_tree_erase(set_desc->internal_avl_tree, key);
        result.inserted_or_existing = NULL;
        return result;
    }

    result.inserted_or_existing->key = allocated_key;
    return result;
}

cmagic_set_insert_result_t
cmagic_set_insert(void *set_ptr, const void *key) {
    cmagic_set_insert_result_t result = cmagic_set_allocate(set_ptr, key);

    if (result.inserted_or_existing && !result.already_exists) {
        assert(result.inserted_or_existing->key);
        memcpy((void *)result.inserted_or_existing->key, key,
               _get_set_descriptor(set_ptr)->key_size);
    }

    return result;
}

void
cmagic_set_erase(void *set_ptr, const void *key) {
    set_descriptor_t *set_desc = _get_set_descriptor(set_ptr);
    cmagic_avl_tree_iterator_t found = cmagic_avl_tree_find(set_desc->internal_avl_tree, key);
    if (found) {
        const void *key_to_delete = found->key;
        cmagic_avl_tree_erase(set_desc->internal_avl_tree, key);
        _get_alloc_packet(set_desc)->free_function((void *)key_to_delete);
    }
}

void
cmagic_set_clear(void *set_ptr) {
    set_descriptor_t *set_desc = _get_set_descriptor(set_ptr);
    for (cmagic_avl_tree_iterator_t it = cmagic_avl_tree_first(set_desc->internal_avl_tree);
         it;
         it = cmagic_avl_tree_iterator_next(it)) {
        _get_alloc_packet(set_desc)->free_function((void *)it->key);
    }
    cmagic_avl_tree_clear(set_desc->internal_avl_tree);
}

size_t
cmagic_set_size(void *set_ptr) {
    return cmagic_avl_tree_size(_get_set_descriptor(set_ptr)->internal_avl_tree);
}

cmagic_set_iterator_t
cmagic_set_first(void *set_ptr) {
    return (cmagic_set_iterator_t)
        cmagic_avl_tree_first(_get_set_descriptor(set_ptr)->internal_avl_tree);
}

cmagic_set_iterator_t
cmagic_set_last(void *set_ptr) {
    return (cmagic_set_iterator_t)
        cmagic_avl_tree_last(_get_set_descriptor(set_ptr)->internal_avl_tree);
}

cmagic_set_iterator_t
cmagic_set_iterator_next(cmagic_set_iterator_t iterator) {
    return (cmagic_set_iterator_t)
        cmagic_avl_tree_iterator_next((cmagic_avl_tree_iterator_t)iterator);
}

cmagic_set_iterator_t
cmagic_set_iterator_prev(cmagic_set_iterator_t iterator) {
    return (cmagic_set_iterator_t)
        cmagic_avl_tree_iterator_prev((cmagic_avl_tree_iterator_t)iterator);
}

cmagic_set_iterator_t
cmagic_set_find(void *set_ptr, const void *key) {
    return (cmagic_set_iterator_t)
        cmagic_avl_tree_find(_get_set_descriptor(set_ptr)->internal_avl_tree, key);
}
