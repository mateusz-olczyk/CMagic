#ifndef CMAGIC_VECTOR_H
#define CMAGIC_VECTOR_H

#include <stddef.h>
#include "cmagic/utils.h"
#include "cmagic/memory.h"

#ifdef __cplusplus
extern "C" {
#endif

void **
cmagic_vector_new(size_t member_size, const cmagic_memory_alloc_packet_t *alloc_packet);

void
cmagic_vector_free(void **vector_ptr);

int
cmagic_vector_push_back(void **vector_ptr, const void *new_element_ptr);

void
cmagic_vector_pop_back(void **vector_ptr);

size_t
cmagic_vector_size(void **vector_ptr);

#define CMAGIC_VECTOR(type) type**

#define CMAGIC_VECTOR_DATA(cmagic_vector) (*(cmagic_vector))

#define CMAGIC_VECTOR_NEW(type, alloc_packet) \
    ((CMAGIC_VECTOR(type))cmagic_vector_new(sizeof(type), (alloc_packet)))

#define CMAGIC_VECTOR_FREE(cmagic_vector) cmagic_vector_free((void**)(cmagic_vector))

#define CMAGIC_VECTOR_PUSH_BACK(cmagic_vector, new_element_ptr) \
    (CMAGIC_UTILS_ASSERT_SAME_TYPE(**(cmagic_vector), *(new_element_ptr)), \
    cmagic_vector_push_back((void**)(cmagic_vector), (new_element_ptr)))

#define CMAGIC_VECTOR_POP_BACK(cmagic_vector) cmagic_vector_pop_back((void**)(cmagic_vector))

#define CMAGIC_VECTOR_SIZE(cmagic_vector) cmagic_vector_size((void**)(cmagic_vector))

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CMAGIC_VECTOR_H */
