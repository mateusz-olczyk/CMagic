/**
 * @file    vector.h
 * @brief   Implementation of a @b vector container.
 * @details Please <b>use provided macros</b> instead of raw functions to gain additional type
 *          checks.
 */

#ifndef CMAGIC_VECTOR_H
#define CMAGIC_VECTOR_H

#include <assert.h>
#include <stdbool.h>
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

bool
cmagic_vector_allocate_back(void **vector_ptr);

bool
cmagic_vector_push_back(void **vector_ptr, const void *new_element_ptr);

void
cmagic_vector_pop_back(void **vector_ptr);

size_t
cmagic_vector_size(void **vector_ptr);

const cmagic_memory_alloc_packet_t *
cmagic_vector_get_alloc_packet(void **vector_ptr);

/**
 * @brief   Convenient alias for @c type**. Returned type of @ref CMAGIC_VECTOR_NEW.
 * @param   type type of vector elements
 */
#define CMAGIC_VECTOR(type) type**

/**
 * @brief   Gets an address to the beginning of the vector data.
 * @param   cmagic_vector a vector allocated before with @ref CMAGIC_VECTOR_NEW
 * @return  address of the first element in the vector
 */
#define CMAGIC_VECTOR_DATA(cmagic_vector) (*(cmagic_vector))

/**
 * @brief   Gets an address of the last element in the vector.
 * @warning Do not use this without ensuring the vector is not empty.
 * @param   cmagic_vector a vector allocated before with @ref CMAGIC_VECTOR_NEW
 * @return  address of the last element in the vector
 */
#define CMAGIC_VECTOR_BACK(cmagic_vector) (assert(CMAGIC_VECTOR_SIZE(cmagic_vector) > 0), \
    CMAGIC_VECTOR_DATA(cmagic_vector) + CMAGIC_VECTOR_SIZE(cmagic_vector) - 1)

/**
 * @brief   Allocates and returns an address of a newly created empty vector.
 * @param   type type of vector elements
 * @param   alloc_packet @ref cmagic_memory_alloc_packet_t suite of dynamic memory managing
 *          functions
 * @return  a new empty vector
 */
#define CMAGIC_VECTOR_NEW(type, alloc_packet) \
    ((CMAGIC_VECTOR(type))cmagic_vector_new(sizeof(type), (alloc_packet)))

/**
 * @brief   Frees the resources allocated by the vector before.
 * @details Must not use @p cmagic_vector after free.
 * @param   cmagic_vector a vector allocated before with @ref CMAGIC_VECTOR_NEW
 */
#define CMAGIC_VECTOR_FREE(cmagic_vector) cmagic_vector_free((void**)(cmagic_vector))

/**
 * @brief   Allocates space for a new element but does not initialize it.
 * @details The new element can be accessed by @ref CMAGIC_VECTOR_BACK
 * @param   cmagic_vector a vector allocated before with @ref CMAGIC_VECTOR_NEW
 * @return  @c true if allocation of the a new element was successful,
 *          @c false if there's not sufficient memory space and the vector was not modified
 */
#define CMAGIC_VECTOR_ALLOCATE_BACK(cmagic_vector) \
    cmagic_vector_allocate_back((void**)(cmagic_vector))

/**
 * @brief   Allocates space for a new element and initializes it with data under @p new_element_ptr
 * @details The new element can be accessed by @ref CMAGIC_VECTOR_BACK
 * @param   cmagic_vector a vector allocated before with @ref CMAGIC_VECTOR_NEW
 * @return  @c true if allocation and initialization of the a new element was successful,
 *          @c false if there's not sufficient memory space and the vector was not modified
 */
#define CMAGIC_VECTOR_PUSH_BACK(cmagic_vector, new_element_ptr) \
    (CMAGIC_UTILS_ASSERT_SAME_TYPE(**(cmagic_vector), *(new_element_ptr)), \
    cmagic_vector_push_back((void**)(cmagic_vector), (new_element_ptr)))

/**
 * @brief   Deallocates the last element in the vector.
 * @warning Do not use this without ensuring the vector is not empty.
 * @param   cmagic_vector a vector allocated before with @ref CMAGIC_VECTOR_NEW
 */
#define CMAGIC_VECTOR_POP_BACK(cmagic_vector) cmagic_vector_pop_back((void**)(cmagic_vector))

/**
 * @brief   Deallocates the last element in the vector.
 * @warning Do not use this without ensuring the vector is not empty.
 * @param   cmagic_vector a vector allocated before with @ref CMAGIC_VECTOR_NEW
 */
#define CMAGIC_VECTOR_SIZE(cmagic_vector) cmagic_vector_size((void**)(cmagic_vector))

/**
 * @brief   Extracts @ref cmagic_memory_alloc_packet_t which was used as an argument of @ref
 *          CMAGIC_VECTOR_NEW
 * @param   cmagic_vector a vector allocated before with @ref CMAGIC_VECTOR_NEW
 * @return  suite of dynamic memory managing functions associated with the vector
 */
#define CMAGIC_VECTOR_GET_ALLOC_PACKET(cmagic_vector) \
    cmagic_vector_get_alloc_packet((void**)(cmagic_vector))

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CMAGIC_VECTOR_H */
