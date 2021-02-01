#ifndef CMAGIC_VECTOR_H
#define CMAGIC_VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdlib.h>
#include "cmagic/utils.h"

typedef void* (*malloc_fptr)(size_t size);
typedef void* (*realloc_fptr)(void* ptr, size_t size);
typedef void (*free_fptr)(void *ptr);

void **
cmagic_vector_new(size_t member_size, malloc_fptr malloc_function, realloc_fptr realloc_function,
                  free_fptr free_function);

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

#define CMAGIC_VECTOR_NEW_NOSTD_ALLOC(type, malloc_function, realloc_function, free_function) \
    ((CMAGIC_VECTOR(type))cmagic_vector_new(sizeof(type), (malloc_function), (realloc_function), \
                                            (free_function)))

#define CMAGIC_VECTOR_NEW(type) CMAGIC_VECTOR_NEW_NOSTD_ALLOC(type, malloc, realloc, free)

#define CMAGIC_VECTOR_FREE(cmagic_vector) cmagic_vector_free((void**)(cmagic_vector))

#define CMAGIC_VECTOR_PUSH_BACK(cmagic_vector, new_element_ptr) \
    (CMAGIC_ASSERT_SAME_TYPE(**(cmagic_vector), *(new_element_ptr)), \
    cmagic_vector_push_back((void**)(cmagic_vector), (new_element_ptr)))

#define CMAGIC_VECTOR_POP_BACK(cmagic_vector) cmagic_vector_pop_back((void**)(cmagic_vector))

#define CMAGIC_VECTOR_SIZE(cmagic_vector) cmagic_vector_size((void**)(cmagic_vector))

#ifdef __cplusplus
}
#endif

#endif /* CMAGIC_VECTOR_H */
