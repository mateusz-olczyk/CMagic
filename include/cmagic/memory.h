/**
 * @file    memory.h
 * @brief   Portable substitutes of standard @c malloc() and @c free() functions.
 * @details Does not use any platform dependent system calls. Everything is maintained in a static
 *          memory block.
 */

#ifndef CMAGIC_MEMORY_H
#define CMAGIC_MEMORY_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Sets a memory range where all dynamic allocated objects will reside.
 * @details Must be called exactly once before any call to any other function from this header.
 *          Sets a memory range which will be used by every other function from this header and it
 *          must be valid during these calls. So generally it should be located in the static memory
 *          to be accessible during the whole program run time. Typical use is to declare a static
 *          array of chars of a sane size and pass it to this function.
 * @param   static_memory_pool      address of a static byte array declared by user
 * @param   static_memory_pool_size size of the static array
 */
void
cmagic_memory_init(void *static_memory_pool, size_t static_memory_pool_size);

/**
 * @brief   Dynamically allocates a block of memory of requested size, returning a pointer to the
 *          beginning of the block.
 * @details The content of the newly allocated block of memory is not initialized, remaining with
 *          indeterminate values. When the memory block is no longer needed it can be freed using
 *          @ref cmagic_memory_free.
 * @par     Implementation details
 *          The memory pool set by @ref cmagic_memory_init is divided into equal size blocks. Every
 *          block is either raw data or a <b>special node</b>. Special node holds an information
 *          about number of bytes allocated right after it. Special nodes form a linked list. When
 *          @ref cmagic_memory_malloc is called the linked list is traversed in search of holes of
 *          unallocated memory. If a hole big enough is found a new special node is created and
 *          inserted into the linked list. Then the address after the special node is returned. If
 *          all holes are too small or there is just no holes at all, the special node is added at
 *          the end of the linked list.
 * @par     Complexity
 *          O(n)
 * @param   size size of the memory block to allocate, in bytes
 * @return  On success, a pointer to the memory block allocated by the function. The type of this
 *          pointer is always @c void*, which can be cast to the desired type of data pointer in
 *          order to be dereferenceable. If the function failed to allocate the requested block of
 *          memory, a @c NULL pointer is returned.
 */
void *
cmagic_memory_malloc(size_t size);

/**
 * @brief   Reallocates memory block changing its original size.
 * @details Changes the size of the memory block pointed to by @p ptr. May move the memory block to
 *          a new location (whose address is returned by the function). The content of the memory
 *          block is preserved up to the lesser of the new and old sizes, even if the block is moved
 *          to a new location. If the new size is larger, the value of the newly allocated portion
 *          is indeterminate. In case that @p ptr is a @c NULL pointer, the function behaves like
 *          @ref cmagic_memory_malloc, assigning a new block of size bytes and returning a pointer
 *          to its beginning.
 * @par     Complexity
 *          O(1) if @p size is lower or equal to the original size, O(n) otherwise
 * @param   ptr pointer to the memory block allocated before by @ref cmagic_memory_malloc or @ref
 *          cmagic_memory_realloc
 * @param   size updated size of the memory block
 * @return  pointer to a possibly new memory block or @c NULL if reallocation failed. In such a case
 *          original @p ptr is as still valid pointer to an unmodified memory block.
 */
void *
cmagic_memory_realloc(void *ptr, size_t size);

/**
 * @brief   Type of result returned from @ref cmagic_memory_free_ext.
 * @details Values with @c OK word are successful @ref cmagic_memory_free_ext calls. Values with
 *          @c ERR word indicate invalid usage of the dynamic allocations. In such cases user should
 *          inspect 
 */
enum cmagic_memory_free_result {

    /** The memory block was freed successfully. */
    CMAGIC_MEMORY_FREE_RESULT_OK,

    /** Function was called on a @c NULL pointer. It's not an error. */
    CMAGIC_MEMORY_FREE_RESULT_OK_NULLPTR,

    /** Tried to free a memory block which was not allocated before or already freed. */
    CMAGIC_MEMORY_FREE_RESULT_ERR_NOT_ALLOCATED_BEFORE,

    /** Function was called on a pointer outside the memory pool specified with
     *  @ref cmagic_memory_init. */
    CMAGIC_MEMORY_FREE_RESULT_ERR_ADDRESS_OUTSIDE_MEMORY_POOL,

    /** @ref cmagic_memory_init was not called before performing any dynamic memory operations. */
    CMAGIC_MEMORY_FREE_RESULT_ERR_UNINITIALIZED

};

/**
 * @brief   Extended version of @ref cmagic_memory_free.
 * @details Mainly for debug purposes. Allows to detect invalid dynamic memory management.
 * @param   ptr address of a memory block to be freed or @c NULL
 * @return  status value indicating operation success or error
 */
enum cmagic_memory_free_result
cmagic_memory_free_ext(void *ptr);

/**
 * @brief   Deallocates a block of memory previously allocated by a call to @ref
 *          cmagic_memory_malloc or @ref cmagic_memory_realloc is deallocated, making it available
 *          again for further allocations.
 * @details Does nothing for @c NULL pointer or pointer not allocated with above functions. Errors
 *          can be retrieved by using extended version of this function @ref cmagic_memory_free_ext.
 *          It never causes <i>undefined behavior</i>. But <i>undefined behavior</i> occurs if user
 *          dereferences pointer after calling this function (<i>use after free</i>). Notice that
 *          this function does not change the value of ptr itself, hence it still points to the same
 *          (now invalid) location.
 * @par     Complexity
 *          O(1)
 * @param   ptr address of a memory block to be freed or @c NULL
 */
static inline void
cmagic_memory_free(void *ptr) { (void)cmagic_memory_free_ext(ptr); }

/**
 * @brief Returns the sum of currently allocated bytes.
 */
size_t
cmagic_memory_get_allocated_bytes(void);

/**
 * @brief   Returns number of free bytes in the memory pool.
 * @details There is not a guarantee that a next @ref cmagic_memory_malloc with a size under the
 *          result returned from this function won't fail. It is so beacuse the free bytes may not
 *          form continuous free space range.
 */
size_t
cmagic_memory_get_free_bytes(void);

/**
 * @brief   Returns number of allocations made by @ref cmagic_memory_malloc.
 * @details Every successful call to @ref cmagic_memory_malloc increments this value. Every
 *          successful call to @ref cmagic_memory_free decrements this value. A call to @ref
 *          cmagic_memory_realloc doesn't change this value.
 * @par     Implementation details
 *          Internally this fuction counts a number of <b>special nodes</b> mentioned in the
 *          description of @ref cmagic_memory_malloc.
 */
size_t
cmagic_memory_get_allocations(void);

#ifdef __cplusplus
}
#endif

#endif /* CMAGIC_MEMORY_H */
