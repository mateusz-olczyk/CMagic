#ifndef CMAGIC_AVL_TREE_H
#define CMAGIC_AVL_TREE_H

#include "cmagic/memory.h"

#ifdef __cplusplus
extern "C" {
#endif

void **
cmagic_avl_tree_new(size_t key_size, size_t value_size,
                    const cmagic_memory_alloc_packet_t *alloc_packet);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CMAGIC_AVL_TREE_H */
