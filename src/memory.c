#include <stdbool.h>
#include <string.h>
#include "cmagic/memory.h"
#include "cmagic/utils.h"

#if !defined(CMAGIC_C_ANONYMOUS_STRUCT_SUPPORT) || !defined(CMAGIC_C_ALIGN_OPERATORS_SUPPORT)
    #error "Needs info about anonymous struct support and align operators support"
#elif !CMAGIC_C_ANONYMOUS_STRUCT_SUPPORT && !CMAGIC_C_ALIGN_OPERATORS_SUPPORT
    #error "Missing required compiler features"
#endif

#if CMAGIC_C_ALIGN_OPERATORS_SUPPORT

typedef struct chunk chunk_t;

struct chunk {
    _Alignas(max_align_t)
    chunk_t *node_next;
    chunk_t *node_prev;
    size_t allocated_bytes;
};

#elif CMAGIC_C_ANONYMOUS_STRUCT_SUPPORT

union max_align {
    void *ptr;
    void (*fptr)();
    long double ld;
    intmax_t i;
};

#define alignof(type) offsetof(struct { char padding; type value; }, value)

typedef union chunk chunk_t;

union chunk {
    struct chunk_raw {
        chunk_t *node_next;
        chunk_t *node_prev;
        size_t allocated_bytes;
    };
    union max_align padding[CMAGIC_UTILS_DIV_CEIL(sizeof(struct chunk_raw),
                                                  sizeof(union max_align))];
};

#endif

static chunk_t *g_pool_begin;
static const chunk_t *g_pool_end;

void
cmagic_memory_init(void *static_memory_pool, size_t static_memory_pool_size) {
#if CMAGIC_C_ALIGN_OPERATORS_SUPPORT
    const size_t alignment = _Alignof(chunk_t);
#else
    const size_t alignment = alignof(chunk_t);
#endif
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

void *
cmagic_memory_malloc(size_t size) {
    if (!g_pool_begin || !g_pool_end) {
        return NULL;
    }

    for (chunk_t *node = g_pool_begin; node; node = node->node_next) {
        if (_available_free_space_after_node(node, size)) {
            chunk_t *new_node = _data_end(node);
            new_node->node_next = node->node_next;
            new_node->node_prev = node;
            new_node->allocated_bytes = size;

            node->node_next = new_node;
            if (new_node->node_next) {
                new_node->node_next->node_prev = new_node;
            }

            return (void *)_data_begin(new_node);
        }
    }
    return NULL;
}

enum cmagic_memory_free_result
cmagic_memory_free(void *ptr) {
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

    return sizeof(g_pool_begin) - (1 + _get_memory_allocated_blocks()) * sizeof(chunk_t);
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
