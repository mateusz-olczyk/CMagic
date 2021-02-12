#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include "cmagic/memory.h"
#include "cmagic/utils.h"
#include "cmagic_config.h"

#if defined(CMAGIC_C_ALIGNAS_OPERATOR_SUPPORT)
    typedef struct chunk {
        _Alignas(max_align_t)
        struct chunk *node_next;
        struct chunk *node_prev;
        size_t allocated_bytes;
    } chunk_t;
#elif defined(CMAGIC_C_ANONYMOUS_STRUCT_SUPPORT)
    typedef union chunk {
        struct chunk_raw {
            union chunk *node_next;
            union chunk *node_prev;
            size_t allocated_bytes;
        };
        max_align_t padding[CMAGIC_UTILS_DIV_CEIL(sizeof(struct chunk_raw), sizeof(max_align_t))];
    } chunk_t;
#else // !defined(CMAGIC_C_ALIGNAS_OPERATOR_SUPPORT) && !defined(CMAGIC_C_ANONYMOUS_STRUCT_SUPPORT)
    #error "Missing required compiler features"
#endif

static chunk_t *g_pool_begin;
static const chunk_t *g_pool_end;

void
cmagic_memory_init(void *static_memory_pool, size_t static_memory_pool_size) {
    static const size_t alignment = _Alignof(chunk_t);
    chunk_t *pool_begin_aligned = (chunk_t *)cmagic_utils_align_address_up(
        (uintptr_t)static_memory_pool, alignment);
    const chunk_t *pool_end_aligned =
        (chunk_t *)cmagic_utils_align_address_down(
            (uintptr_t)static_memory_pool + static_memory_pool_size, alignment);

    if (pool_end_aligned - pool_begin_aligned > 0) {
        memset(pool_begin_aligned, 0, sizeof(chunk_t));
        g_pool_begin = pool_begin_aligned;
        g_pool_end = pool_end_aligned;
    } else {
        g_pool_begin = NULL;
        g_pool_end = NULL;
    }
}

static size_t _count_needed_blocks(size_t bytes_count) {
    return 1 /* for node block */ + CMAGIC_UTILS_DIV_CEIL(bytes_count, sizeof(chunk_t));
}

static chunk_t *_data_begin(chunk_t *node) {
    return &node[1];
}

static chunk_t *_data_end(chunk_t *node) {
    return &node[_count_needed_blocks(node->allocated_bytes)];
}

static chunk_t *_node_list_head(void) {
    return g_pool_begin[0].node_next;
}

static chunk_t *_associated_node(void *data_begin) {
    return (chunk_t *)data_begin - 1;
}

static bool _available_free_space_after_node(chunk_t *node, size_t needed_bytes) {
    const chunk_t *free_space_begin = _data_end(node);
    const chunk_t *free_space_end = node->node_next ? node->node_next : g_pool_end;
    const size_t blocks_left = (size_t)(free_space_end - free_space_begin);
    return _count_needed_blocks(needed_bytes) <= blocks_left;
}

static bool _is_existing_memory_node(chunk_t *node) {
    for (chunk_t *node_from_list = _node_list_head(); node_from_list;
         node_from_list = node_from_list->node_next) {
        if (node_from_list == node) {
            return true;
        }
    }
    return false;
}

static chunk_t *_insert_node(chunk_t *const new_node, size_t new_node_allocated_bytes,
                             chunk_t *const prev_node, chunk_t *const next_node) {
    new_node->node_next = next_node;
    new_node->node_prev = prev_node;
    new_node->allocated_bytes = new_node_allocated_bytes;

    prev_node->node_next = new_node;
    if (next_node) {
        next_node->node_prev = new_node;
    }

    return new_node;
}

void *
cmagic_memory_malloc(size_t size) {
    if (!g_pool_begin || !g_pool_end) {
        return NULL;
    }

    for (chunk_t *node = g_pool_begin; node; node = node->node_next) {
        if (_available_free_space_after_node(node, size)) {
            return (void *)_data_begin(_insert_node(_data_end(node), size, node, node->node_next));
        }
    }
    return NULL;
}

void *
cmagic_memory_realloc(void *ptr, size_t size) {
    if (!ptr) {
        return cmagic_memory_malloc(size);
    }

    chunk_t *node = _associated_node(ptr);
    if (!_is_existing_memory_node(node)) {
        return NULL;
    }

    void *original_data = (void *)_data_begin(node);
    const size_t bytes_to_copy = CMAGIC_UTILS_MIN(size, node->allocated_bytes);

    chunk_t *potential_free_space_begin = _data_end(node->node_prev);
    const chunk_t *potential_free_space_end = node->node_next ? node->node_next : g_pool_end;
    const size_t free_blocks = (size_t)(potential_free_space_end - potential_free_space_begin);
    if (_count_needed_blocks(size) <= free_blocks) {
        void *result = _data_begin(_insert_node(potential_free_space_begin, size, node->node_prev,
                                                node->node_next));
        if (result != original_data) {
            memmove(result, original_data, bytes_to_copy);
        }
        return result;
    }

    void *result = cmagic_memory_malloc(size);
    if (result) {
        memcpy(result, original_data, bytes_to_copy);
        cmagic_memory_free(ptr);
        return result;
    }

    return NULL;
}

enum cmagic_memory_free_result
cmagic_memory_free_ext(void *ptr) {
    if (!g_pool_begin || !g_pool_end) {
        return CMAGIC_MEMORY_FREE_RESULT_ERR_UNINITIALIZED;
    }

    if (!ptr) {
        return CMAGIC_MEMORY_FREE_RESULT_OK_NULLPTR;
    }

    if (ptr < (const void *)_node_list_head() || (const void *)g_pool_end <= ptr) {
        return CMAGIC_MEMORY_FREE_RESULT_ERR_ADDRESS_OUTSIDE_MEMORY_POOL;
    }

    chunk_t *node = _associated_node(ptr);
    if (!_is_existing_memory_node(node)) {
        return CMAGIC_MEMORY_FREE_RESULT_ERR_NOT_ALLOCATED_BEFORE;
    }

    node->node_prev->node_next = node->node_next;
    if (node->node_next) {
        node->node_next->node_prev = node->node_prev;
    }

    return CMAGIC_MEMORY_FREE_RESULT_OK;
}

void
cmagic_memory_free(void *ptr) {
    enum cmagic_memory_free_result result = cmagic_memory_free_ext(ptr);
    (void) result;
    assert(result == CMAGIC_MEMORY_FREE_RESULT_OK
           || result == CMAGIC_MEMORY_FREE_RESULT_OK_NULLPTR);
}

size_t
cmagic_memory_get_allocated_bytes(void) {
    if (!g_pool_begin || !g_pool_end) {
        return 0;
    }

    size_t allocated_bytes = 0;
    for (chunk_t *node = _node_list_head(); node; node = node->node_next) {
        allocated_bytes += node->allocated_bytes;
    }
    return allocated_bytes;
}

static size_t _get_memory_allocated_blocks(void) {
    size_t allocated_blocks = 0;
    for (chunk_t *node = _node_list_head(); node; node = node->node_next) {
        allocated_blocks += _count_needed_blocks(node->allocated_bytes);
    }
    return allocated_blocks;
}

size_t
cmagic_memory_get_free_bytes(void) {
    if (!g_pool_begin || !g_pool_end) {
        return 0;
    }

    return sizeof(chunk_t) * ((size_t)(g_pool_end - g_pool_begin)
                              - 1 - _get_memory_allocated_blocks());
}

size_t
cmagic_memory_get_allocations(void) {
    if (!g_pool_begin || !g_pool_end) {
        return 0;
    }

    size_t allocations = 0;
    for (chunk_t *node = _node_list_head(); node; node = node->node_next) {
        allocations++;
    }
    return allocations;
}
