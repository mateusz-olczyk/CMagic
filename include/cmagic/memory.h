#ifndef CMAGIC_MEMORY_H
#define CMAGIC_MEMORY_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void
cmagic_memory_init(void *static_memory_pool, size_t static_memory_pool_size);

void *
cmagic_memory_malloc(size_t size);

void *
cmagic_memory_realloc(void *ptr, size_t size);

enum cmagic_memory_free_result {
    CMAGIC_MEMORY_FREE_RESULT_OK,
    CMAGIC_MEMORY_FREE_RESULT_OK_NULLPTR,
    CMAGIC_MEMORY_FREE_RESULT_ERR_NOT_ALLOCATED_BEFORE,
    CMAGIC_MEMORY_FREE_RESULT_ERR_ADDRESS_OUTSIDE_MEMORY_POOL,
    CMAGIC_MEMORY_FREE_RESULT_ERR_UNINITIALIZED
};

enum cmagic_memory_free_result
cmagic_memory_free_ext(void *ptr);

static inline void
cmagic_memory_free(void *ptr) { (void)cmagic_memory_free_ext(ptr); }

size_t
cmagic_memory_get_allocated_bytes(void);

size_t
cmagic_memory_get_free_bytes(void);

size_t
cmagic_memory_get_allocations(void);

#ifdef __cplusplus
}
#endif

#endif /* CMAGIC_MEMORY_H */
