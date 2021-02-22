/**
 * @file    set.h
 * @brief   Implementation of a @b set container.
 * @details Please <b>use provided macros</b> instead of raw functions to gain additional type
 *          checks.
 */

#ifndef CMAGIC_SET_H
#define CMAGIC_SET_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include "cmagic/memory.h"
#include "cmagic/utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Pointer to a function that compares two elements
 * @details This function is called by the set implementation to compare two elements, which is
 *          necessary to properly construct and move inside its internal binary tree. Both arguments
 *          must be manually casted to a type corresponding to the type of the set. This function
 *          defines the order of the elements.
 * @param   key1 pointer to the first key value
 * @param   key2 pointer to the second first key value
 * @return
 *          value | meaning
 *          :----:|---------------------------------------------------------------------------------
 *          <0    | The element pointed to by @p key1 goes before the element pointed to by @p key2
 *          0     | The element pointed to by @p key1 is equivalent to the element pointed to @p key2
 *          >0    | The element pointed to by @p key1 goes after the element pointed to by @p key2
 */
typedef int (*cmagic_set_key_comparator_t)(const void *key1, const void *key2);

void *
cmagic_set_new(size_t key_size, cmagic_set_key_comparator_t key_comparator,
               const cmagic_memory_alloc_packet_t *alloc_packet);

void
cmagic_set_free(void *set_ptr);

typedef struct {
    const void *key;
} *cmagic_set_iterator_t;

/**
 * @brief   Set insertion result
 */
typedef struct {

    /**
     * @brief   iterator pointing to a new or already existing element or @c NULL if the allocation
     *          of a new element has failed
     */
    cmagic_set_iterator_t inserted_or_existing;

    /**
     * @brief   @c true if the element already exists in the set and the set was not modified,
     *          @c false if a new element has been allocated
     */
    bool already_exists;

} cmagic_set_insert_result_t;

cmagic_set_insert_result_t
cmagic_set_allocate(void *set_ptr, const void *key);

cmagic_set_insert_result_t
cmagic_set_insert(void *set_ptr, const void *key);

void
cmagic_set_erase(void *set_ptr, const void *key);

void
cmagic_set_clear(void *set_ptr);

size_t
cmagic_set_size(void *set_ptr);

cmagic_set_iterator_t
cmagic_set_first(void *set_ptr);

cmagic_set_iterator_t
cmagic_set_last(void *set_ptr);

cmagic_set_iterator_t
cmagic_set_iterator_next(cmagic_set_iterator_t iterator);

cmagic_set_iterator_t
cmagic_set_iterator_prev(cmagic_set_iterator_t iterator);

cmagic_set_iterator_t
cmagic_set_find(void *set_ptr, const void *key);

const cmagic_memory_alloc_packet_t *
cmagic_set_get_alloc_packet(void *set_ptr);

/**
 * @brief   Convenient alias for @c type*. Returned type of @ref CMAGIC_SET_NEW.
 * @param   type type of set elements
 */
#define CMAGIC_SET(key_type) key_type*

/**
 * @brief   Allocates and returns an address of a newly created empty set.
 * @param   key_type type of set elements
 * @param   key_comparator function of type @ref cmagic_set_key_comparator_t determining the order
 *          of the elements
 * @param   alloc_packet @ref cmagic_memory_alloc_packet_t suite of dynamic memory managing
 *          functions
 * @return  a new empty set
 */
#define CMAGIC_SET_NEW(key_type, key_comparator, alloc_packet) \
    ((CMAGIC_SET(key_type))cmagic_set_new(sizeof(key_type), (key_comparator), (alloc_packet)))

/**
 * @brief   Frees the resources allocated by the set before.
 * @details Must not use @p cmagic_set after free.
 * @param   cmagic_set a set allocated before with @ref CMAGIC_SET_NEW
 */
#define CMAGIC_SET_FREE(cmagic_set) cmagic_set_free((void*)(cmagic_set))

/**
 * @brief   Allocates space for a new element but does not initialize it.
 * @details New element is allocated only if it doesn't already exist in the set.
 * @warning The new element must be initialized right after calling this function. Especially it
 *          must be ready then to call @ref cmagic_set_key_comparator_t on it and return the same
 *          value as for @p key. Otherwise the set will be in an undefined state.
 * @param   cmagic_set a set allocated before with @ref CMAGIC_SET_NEW
 * @param   key pointer to the key value, needed to place a new element in the right place in the
 *          internal binary tree
 * @return  @ref cmagic_set_insert_result_t pointing to the new or already existing element
 */
#define CMAGIC_SET_ALLOCATE(cmagic_set, key) \
    (CMAGIC_UTILS_ASSERT_SAME_TYPE(*(cmagic_set), *(key)), \
    cmagic_set_allocate((void*)(cmagic_set), (key)))

/**
 * @brief   Allocates space for a new element and initializes it with data under @p key
 * @param   cmagic_set a set allocated before with @ref CMAGIC_SET_NEW
 * @param   key pointer to the key value
 * @return  @ref cmagic_set_insert_result_t pointing to the new or already existing element
 */
#define CMAGIC_SET_INSERT(cmagic_set, key) \
    (CMAGIC_UTILS_ASSERT_SAME_TYPE(*(cmagic_set), *(key)), \
    cmagic_set_insert((void*)(cmagic_set), (key)))

/**
 * @brief   Removes a single element from the set
 * @param   cmagic_set a set allocated before with @ref CMAGIC_SET_NEW
 * @param   key pointer to a value to be removed from the set. This function compares by a value,
 *          the @p key doesn't have to be an address of the original key. Does nothing if the
 *          element doesn't exist in the set.
 */
#define CMAGIC_SET_ERASE(cmagic_set, key) \
    (CMAGIC_UTILS_ASSERT_SAME_TYPE(*(cmagic_set), *(key)), \
    cmagic_set_erase((void*)(cmagic_set), (key)))

/**
 * @brief   Removes all elements from the set
 * @param   cmagic_set a set allocated before with @ref CMAGIC_SET_NEW
 */
#define CMAGIC_SET_CLEAR(cmagic_set) cmagic_set_clear((void*)(cmagic_set))

/**
 * @brief   Returns the number of elements in the set
 * @param   cmagic_set a set allocated before with @ref CMAGIC_SET_NEW
 * @return  number of elements in the set
 */
#define CMAGIC_SET_SIZE(cmagic_set) cmagic_set_size((void*)(cmagic_set))

/**
 * @brief   Return iterator to the first element in set
 * @details Returns an iterator pointing to the first element in the set according to the order
 *          defined by @ref cmagic_set_key_comparator_t.
 * @param   cmagic_set a set allocated before with @ref CMAGIC_SET_NEW
 * @return  an iterator to the first element or @c NULL if the set is empty
 */
#define CMAGIC_SET_FIRST(cmagic_set) cmagic_set_first((void*)(cmagic_set))

/**
 * @brief   Return iterator to the last element in set
 * @details Returns an iterator pointing to the last element in the set according to the order
 *          defined by @ref cmagic_set_key_comparator_t.
 * @param   cmagic_set a set allocated before with @ref CMAGIC_SET_NEW
 * @return  an iterator to the last element or @c NULL if the set is empty
 */
#define CMAGIC_SET_LAST(cmagic_set) cmagic_set_last((void*)(cmagic_set))

/**
 * @brief   Returns iterator to the next element in container
 * @param   iterator @ref cmagic_set_iterator_t object
 * @return  iterator to the next element or @c NULL if @p iterator was the last element in container
 */
#define CMAGIC_SET_ITERATOR_NEXT(iterator) cmagic_set_iterator_next(iterator)

/**
 * @brief   Returns iterator to the previous element in container
 * @param   iterator @ref cmagic_set_iterator_t object
 * @return  iterator to the previous element or @c NULL if @p iterator was the first element in
 *          container
 */
#define CMAGIC_SET_ITERATOR_PREV(iterator) cmagic_set_iterator_prev(iterator)

/**
 * @brief   Searches the container for an element equivalent to @p key and returns an iterator to it
 *          if found, otherwise it returns @c NULL.
 * @param   key pointer to a value to be searched for
 * @return  an iterator to the element, if @p key is found, or @c NULL otherwise
 */
#define CMAGIC_SET_FIND(cmagic_set, key) (CMAGIC_UTILS_ASSERT_SAME_TYPE(*(cmagic_set), *(key)), \
    cmagic_set_find((void*)(cmagic_set), (key)))

/**
 * @brief   Helper macro for retrieving the key value from the iterator
 * @warning @p iterator must not be @c NULL
 * @param   key_type type of set elements to which the iterator points to
 * @param   iterator @ref cmagic_set_iterator_t object
 * @return  value of the key
 */
#define CMAGIC_SET_GET_KEY(key_type, iterator) \
    (assert(iterator), assert((iterator)->key), *((const key_type*)(iterator)->key))

/**
 * @brief   Retrieves @ref cmagic_memory_alloc_packet_t associated with the set
 * @param   cmagic_set a set allocated before with @ref CMAGIC_SET_NEW
 * @return  @ref cmagic_memory_alloc_packet_t associated with the set
 */
#define CMAGIC_SET_GET_ALLOC_PACKET(cmagic_set) \
    cmagic_set_get_alloc_packet((void*)(cmagic_set))

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CMAGIC_SET_H */
