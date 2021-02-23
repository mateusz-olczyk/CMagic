/**
 * @file    map.h
 * @brief   Implementation of a @b map container.
 * @details Please <b>use provided macros</b> instead of raw functions to gain additional type
 *          checks.
 */

#ifndef CMAGIC_MAP_H
#define CMAGIC_MAP_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include "cmagic/memory.h"
#include "cmagic/utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Pointer to a function that compares two keys
 * @details This function is called by the map implementation to compare two kes, which is necessary
 *          to properly construct and move inside its internal binary tree. Both arguments must be
 *          manually casted to a type corresponding to the type of the map key. This function
 *          defines the order of the elements.
 * @param   key1 pointer to the first key value
 * @param   key2 pointer to the second first key value
 * @return
 *          value | meaning
 *          :----:|---------------------------------------------------------------------------------
 *          <0    | The element pointed to by @p key1 goes before the element pointed to by @p key2
 *          0     | The element pointed to by @p key1 is equivalent to the element pointed to by @p key2
 *          >0    | The element pointed to by @p key1 goes after the element pointed to by @p key2
 */
typedef int (*cmagic_map_key_comparator_t)(const void *key1, const void *key2);

/**
 * @brief   User defined additional tasks to be executed right before map element deletion
 * @warning Do not call @c free function on the @c key or @c value. It's called later internally by
 *          map implementation
 * @param   key pointer to key to be deleted
 * @param   value pointer to the value to be deleted
 */
typedef void (*cmagic_map_erase_destructor_t)(void *key, void *value);

void *
cmagic_map_new(size_t key_size, size_t value_size, cmagic_map_key_comparator_t key_comparator,
               const cmagic_memory_alloc_packet_t *alloc_packet);

void
cmagic_map_free(void *map_ptr);

typedef struct {
    const void *key;
    void *value;
} *cmagic_map_iterator_t;

/**
 * @brief   Map insertion result
 */
typedef struct {

    /**
     * @brief   iterator pointing to a new or already existing element or @c NULL if the allocation
     *          of a new element has failed
     */
    cmagic_map_iterator_t inserted_or_existing;

    /**
     * @brief   @c true if the element already exists in the map and the map was not modified,
     *          @c false if a new element has been allocated
     */
    bool already_exists;

} cmagic_map_insert_result_t;

cmagic_map_insert_result_t
cmagic_map_allocate(void *map_ptr, const void *key);

cmagic_map_insert_result_t
cmagic_map_insert(void *map_ptr, const void *key, const void *value);

void
cmagic_map_erase(void *map_ptr, const void *key, cmagic_map_erase_destructor_t destructor);

void
cmagic_map_clear(void *map_ptr);

size_t
cmagic_map_size(void *map_ptr);

cmagic_map_iterator_t
cmagic_map_first(void *map_ptr);

cmagic_map_iterator_t
cmagic_map_last(void *map_ptr);

cmagic_map_iterator_t
cmagic_map_iterator_next(cmagic_map_iterator_t iterator);

cmagic_map_iterator_t
cmagic_map_iterator_prev(cmagic_map_iterator_t iterator);

cmagic_map_iterator_t
cmagic_map_find(void *map_ptr, const void *key);

const cmagic_memory_alloc_packet_t *
cmagic_map_get_alloc_packet(void *map_ptr);

/**
 * @brief   Convenient alias for @c type*. Returned type of @ref CMAGIC_MAP_NEW.
 * @warning Because of C language limitations all type checks are performed only for map keys. User
 *          has to be careful while passing map values to macros from this file. Their types must
 *          match to the type of map values but this is not checked at compile time.
 * @param   type type of map keys
 */
#define CMAGIC_MAP(key_type) key_type*

/**
 * @brief   Allocates and returns an address of a newly created empty map.
 * @param   key_type type of map elements
 * @param   value_type type of map values
 * @param   key_comparator function of type @ref cmagic_map_key_comparator_t determining the order
 *          of the elements
 * @param   alloc_packet @ref cmagic_memory_alloc_packet_t suite of dynamic memory managing
 *          functions
 * @return  a new empty map
 */
#define CMAGIC_MAP_NEW(key_type, value_type, key_comparator, alloc_packet) ((CMAGIC_MAP(key_type)) \
    cmagic_map_new(sizeof(key_type), sizeof(value_type), (key_comparator), (alloc_packet)))

/**
 * @brief   Frees the resources allocated by the map before.
 * @details Must not use @p cmagic_map after free.
 * @param   cmagic_map a map allocated before with @ref CMAGIC_MAP_NEW
 */
#define CMAGIC_MAP_FREE(cmagic_map) cmagic_map_free((void*)(cmagic_map))

/**
 * @brief   Allocates space for a new element (key-value pair) but does not initialize it.
 * @details New element is allocated only if @p key doesn't already exist in the map.
 * @warning The new element must be initialized right after calling this function. Especially it
 *          must be ready then to call @ref cmagic_map_key_comparator_t on it and return the same
 *          value as for @p key. Otherwise the map will be in an undefined state.
 * @param   cmagic_map a map allocated before with @ref CMAGIC_MAP_NEW
 * @param   key pointer to the key value, needed to place a new element in the right place in the
 *          internal binary tree
 * @return  @ref cmagic_map_insert_result_t pointing to the new or already existing element
 */
#define CMAGIC_MAP_ALLOCATE(cmagic_map, key) \
    (CMAGIC_UTILS_ASSERT_SAME_TYPE(*(cmagic_map), *(key)), \
    cmagic_map_allocate((void*)(cmagic_map), (key)))

/**
 * @brief   Allocates space for a new element and initializes it with data under @p key and @p value
 * @param   cmagic_map a map allocated before with @ref CMAGIC_MAP_NEW
 * @param   key pointer to the key value
 * @param   value pointer to the value value
 * @return  @ref cmagic_map_insert_result_t pointing to the new or already existing element
 */
#define CMAGIC_MAP_INSERT(cmagic_map, key, value) \
    (CMAGIC_UTILS_ASSERT_SAME_TYPE(*(cmagic_map), *(key)), \
    cmagic_map_insert((void*)(cmagic_map), (key), (value)))

/**
 * @brief   Extended version of @ref CMAGIC_MAP_ERASE
 * @param   cmagic_map a map allocated before with @ref CMAGIC_MAP_NEW
 * @param   key pointer to a key to be removed from the map. This function compares by a value, the
 *          @p key doesn't have to be an address of the original key. Does nothing if the element
 *          doesn't exist in the map.
 * @param   destructor function of type @ref cmagic_map_erase_destructor_t to be called on the key
 *          and value right before deleting them
 */
#define CMAGIC_MAP_ERASE_EXT(cmagic_map, key, destructor) \
    (CMAGIC_UTILS_ASSERT_SAME_TYPE(*(cmagic_map), *(key)), \
    cmagic_map_erase((void*)(cmagic_map), (key), (destructor)))

/**
 * @brief   Removes a single element (key-value pair) from the map
 * @param   cmagic_map a map allocated before with @ref CMAGIC_MAP_NEW
 * @param   key pointer to a key to be removed from the map. This function compares by a value, the
 *          @p key doesn't have to be an address of the original key. Does nothing if the element
 *          doesn't exist in the map.
 */
#define CMAGIC_MAP_ERASE(cmagic_map, key) CMAGIC_MAP_ERASE_EXT(cmagic_map, key, NULL)

/**
 * @brief   Removes all elements from the map
 * @param   cmagic_map a map allocated before with @ref CMAGIC_MAP_NEW
 */
#define CMAGIC_MAP_CLEAR(cmagic_map) cmagic_map_clear((void*)(cmagic_map))

/**
 * @brief   Returns the number of elements in the map
 * @param   cmagic_map a map allocated before with @ref CMAGIC_MAP_NEW
 * @return  number of elements in the map
 */
#define CMAGIC_MAP_SIZE(cmagic_map) cmagic_map_size((void*)(cmagic_map))

/**
 * @brief   Return iterator to the first element in map
 * @details Returns an iterator pointing to the first element in the map according to the order
 *          defined by @ref cmagic_map_key_comparator_t.
 * @param   cmagic_map a map allocated before with @ref CMAGIC_MAP_NEW
 * @return  an iterator to the first element or @c NULL if the map is empty
 */
#define CMAGIC_MAP_FIRST(cmagic_map) cmagic_map_first((void*)(cmagic_map))

/**
 * @brief   Return iterator to the last element in map
 * @details Returns an iterator pointing to the last element in the map according to the order
 *          defined by @ref cmagic_map_key_comparator_t.
 * @param   cmagic_map a map allocated before with @ref CMAGIC_MAP_NEW
 * @return  an iterator to the last element or @c NULL if the map is empty
 */
#define CMAGIC_MAP_LAST(cmagic_map) cmagic_map_last((void*)(cmagic_map))

/**
 * @brief   Returns iterator to the next element in container
 * @param   iterator @ref cmagic_map_iterator_t object
 * @return  iterator to the next element or @c NULL if @p iterator was the last element in container
 */
#define CMAGIC_MAP_ITERATOR_NEXT(iterator) cmagic_map_iterator_next(iterator)

/**
 * @brief   Returns iterator to the previous element in container
 * @param   iterator @ref cmagic_map_iterator_t object
 * @return  iterator to the previous element or @c NULL if @p iterator was the first element in
 *          container
 */
#define CMAGIC_MAP_ITERATOR_PREV(iterator) cmagic_map_iterator_prev(iterator)

/**
 * @brief   Searches the container for an element with a key equivalent to @p key and returns an
 *          iterator to it if found, otherwise it returns @c NULL.
 * @param   key pointer to a key to be searched for
 * @return  an iterator to the element, if @p key is found, or @c NULL otherwise
 */
#define CMAGIC_MAP_FIND(cmagic_map, key) (CMAGIC_UTILS_ASSERT_SAME_TYPE(*(cmagic_map), *(key)), \
    cmagic_map_find((void*)(cmagic_map), (key)))

/**
 * @brief   Helper macro for retrieving the key from the iterator
 * @warning @p iterator must not be @c NULL
 * @param   key_type type of the keys of the map which the iterator is associated with
 * @param   iterator @ref cmagic_map_iterator_t object
 * @return  map key
 */
#define CMAGIC_MAP_GET_KEY(key_type, iterator) \
    (assert(iterator), assert((iterator)->key), *((const key_type*)(iterator)->key))

/**
 * @brief   Helper macro for retrieving the value from the iterator
 * @warning @p iterator must not be @c NULL
 * @param   value_type type of the values of the map which the iterator is associated with
 * @param   iterator @ref cmagic_map_iterator_t object
 * @return  map value
 */
#define CMAGIC_MAP_GET_VALUE(value_type, iterator) \
    (assert(iterator), assert((iterator)->value), *((value_type*)(iterator)->value))

/**
 * @brief   Retrieves @ref cmagic_memory_alloc_packet_t associated with the map
 * @param   cmagic_map a map allocated before with @ref CMAGIC_MAP_NEW
 * @return  @ref cmagic_memory_alloc_packet_t associated with the map
 */
#define CMAGIC_MAP_GET_ALLOC_PACKET(cmagic_map) \
    cmagic_map_get_alloc_packet((void*)(cmagic_map))

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CMAGIC_MAP_H */
