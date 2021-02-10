#ifndef CMAGIC_AVL_TREE_H
#define CMAGIC_AVL_TREE_H

#include <stdbool.h>
#include "cmagic/memory.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*cmagic_avl_tree_key_comparator_t)(const void *key1, const void *key2);

void **
cmagic_avl_tree_new(cmagic_avl_tree_key_comparator_t key_comparator,
                    const cmagic_memory_alloc_packet_t *alloc_packet);

bool
cmagic_avl_tree_insert(void **avl_tree, const void *key, void *value);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CMAGIC_AVL_TREE_H */
