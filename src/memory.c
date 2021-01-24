#include <stdbool.h>
#include "cmagic/memory.h"
#include "cmagic/utils.h"

#define MEMORY_BLOCKS_COUNT 1000

typedef struct chunk chunk_t;

struct chunk {
    _Alignas(max_align_t)
    chunk_t *node_next;
    chunk_t *node_prev;
    size_t allocated_bytes;
};

static chunk_t pool[MEMORY_BLOCKS_COUNT];
static const chunk_t *const pool_end = pool + CMAGIC_UTILS_ARRAY_SIZE(pool);

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
    return pool[0].node_next;
}

static chunk_t *_associated_node(void *data_begin) {
    return (chunk_t *)data_begin - 1;
}

static bool _available_free_space_after_node(chunk_t *node, size_t needed_bytes) {
    const chunk_t *free_space_begin = _data_end(node);
    const chunk_t *free_space_end = node->node_next ? node->node_next : pool_end;
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
    for (chunk_t *node = pool; node; node = node->node_next) {
        if (_available_free_space_after_node(node, size)) {
            chunk_t *new_node = _data_end(node);
            new_node->node_next = node->node_next;
            new_node->node_prev = node;
            new_node->allocated_bytes = size;

            node->node_next = new_node;
            if (new_node->node_next != NULL) {
                new_node->node_next->node_prev = new_node;
            }

            return (void *)_data_begin(new_node);
        }
    }
    return NULL;
}

enum cmagic_memory_free_result
cmagic_memory_free(void *ptr) {
    if (!ptr) {
        return CMAGIC_MEMORY_FREE_RESULT_OK_NULLPTR;
    }

    if (ptr < (const void *)_node_list_head() || (const void *)pool_end <= ptr) {
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
    return sizeof(pool) - (1 + _get_memory_allocated_blocks()) * sizeof(chunk_t);
}

size_t
cmagic_memory_get_allocations(void) {
    size_t allocations = 0;
    for (chunk_t *node = _node_list_head(); node; node = node->node_next) {
        allocations++;
    }
    return allocations;
}
