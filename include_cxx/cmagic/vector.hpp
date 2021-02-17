/**
 * @file    vector.hpp
 * @brief   Template implementation of a @b vector container.
 * @details This is a wrapper over C implementation from @ref vector.h
 */

#ifndef CMAGIC_VECTOR_HPP
#define CMAGIC_VECTOR_HPP

#include <cassert>
#include <type_traits>
#include <new>
#include "cmagic/vector.h"


namespace cmagic {

/**
 * @brief   A sequence container representing array that can change in size.
 * @details The content forms contiguous storage location. Size can change dynamically, with storage
 *          being handled automatically by the container. Reallocations if needed happen only at
 *          logarithmically growing intervals of size so that the insertion of individual elements
 *          at the end of the vector can be provided with amortized constant time complexity.
 */
template<typename T>
class vector {

public:
    /**
     * @brief   Type of vector elements.
     */
    using value_type = T;

    /**
     * @brief   Type used to measure element size and position in a vector.
     */
    using size_type = size_t;

private:
    static_assert(std::is_copy_assignable<T>(), "value type must be copy-assignable");
    static_assert(std::is_copy_constructible<T>(), "value type must be copy-constructible");
    CMAGIC_VECTOR(T) vector_handle;

    explicit vector(const cmagic_memory_alloc_packet_t *alloc_packet)
    : vector_handle(CMAGIC_VECTOR_NEW(value_type, alloc_packet)) {}

    bool allocate_back() {
        assert(*this);
        return CMAGIC_VECTOR_ALLOCATE_BACK(vector_handle);
    }

public:
    /**
     * @brief   Constructs an empty vector with standard memory allocation.
     * @return  a new empty vector
     */
    vector() : vector(&CMAGIC_MEMORY_ALLOC_PACKET_STD) {}

    /**
     * @brief   Constructs an empty vector using custom @e CMagic memory allocation from @ref
     *          memory.h
     * @return  a new empty vector
     */
    static vector custom_allocation_vector() {
        return vector(&CMAGIC_MEMORY_ALLOC_PACKET_CUSTOM_CMAGIC);
    }

    vector &operator=(const vector &x) {
        assert(*this);
        if (&x == this) {
            return *this;
        }

        clear();
        for (const value_type &val : x) {
            if (!push_back(val)) {
                clear();
                CMAGIC_VECTOR_FREE(vector_handle);
                vector_handle = nullptr;
                return *this;
            }
        }
        return *this;
    }

    vector(const vector &x) : vector(CMAGIC_VECTOR_GET_ALLOC_PACKET(x.vector_handle)) {
        if (*this) {
            operator=(x);
        }
    }

    vector &operator=(vector &&x) {
        assert(*this);
        clear();
        CMAGIC_VECTOR_FREE(vector_handle);
        vector_handle = x.vector_handle;
        x.vector_handle = CMAGIC_VECTOR_NEW(value_type,
                                            CMAGIC_VECTOR_GET_ALLOC_PACKET(x.vector_handle));
    }

    vector(vector &&x) : vector_handle(x.vector_handle) {
        x.vector_handle = CMAGIC_VECTOR_NEW(value_type,
                                            CMAGIC_VECTOR_GET_ALLOC_PACKET(x.vector_handle));
    }

    /**
     * @brief   Returns the number of elements in the vector.
     * @details This is the number of actual objects held in the vector, which is not necessarily
     *          equal to its storage capacity.
     * @return  number of elements in the vector
     */
    size_type size() const {
        assert(*this);
        return CMAGIC_VECTOR_SIZE(vector_handle);
    }

    /**
     * @brief   Returns whether the vector is empty (i.e. whether its size is 0).
     * @details This function does not modify the container in any way. To clear the content of a
     *          vector, see @ref vector::clear.
     * @return  @c true if the container size is 0, @c false otherwise
     */
    bool empty() const {
        return size() == 0;
    }

    /**
     * @brief   Returns a reference to the element at position @p pos in the vector container.
     * @warning Never call this function with an argument n that is out of range, since this causes
     *          undefined behavior.
     * @param   pos Position of an element in the container. Notice that the first element has a
     *          position of 0 (not 1).
     * @return  the element at the specified position in the vector
     */
    value_type &operator[](size_type pos) {
        assert(pos <= size());
        return CMAGIC_VECTOR_DATA(vector_handle)[pos];
    }

    /**
     * @copydoc vector::operator[]
     */
    const value_type &operator[](size_type pos) const {
        assert (pos <= size());
        return CMAGIC_VECTOR_DATA(vector_handle)[pos];
    }

    /**
     * @brief   Add element at the end.
     * @details Adds a new element at the end of the vector, after its current last element. The
     *          content of @p val is copied (or moved) to the new element. This effectively
     *          increases the container size by one, which causes an automatic reallocation of the
     *          allocated storage space if the new vector size surpasses the current vector
     *          capacity.
     * @param   val value to be copied (or moved) to the new element
     */
    bool push_back(const value_type &val) {
        assert(*this);
        if (!allocate_back()) {
            return false;
        }

        new(CMAGIC_VECTOR_BACK(vector_handle)) value_type {val};
        return true;
    }

    /**
     * @copydoc vector::push_back
     */
    bool push_back(value_type &&val) {
        assert(*this);
        if (!allocate_back()) {
            return false;
        }

        new(CMAGIC_VECTOR_BACK(vector_handle)) value_type {std::move(val)};
        return true;
    }

    /**
     * @brief   Construct and insert element at the end
     * @details Inserts a new element at the end of the vector, right after its current last
     *          element. This new element is constructed in place using args as the arguments for
     *          its constructor. This effectively
     *          increases the container size by one, which causes an automatic reallocation of the
     *          allocated storage space if the new vector size surpasses the current vector
     *          capacity.
     * @param   args arguments forwarded to construct the new element
     */
    template<typename... Args>
    bool emplace_back(Args&&... args) {
        assert(*this);
        if (!allocate_back()) {
            return false;
        }

        new(CMAGIC_VECTOR_BACK(vector_handle)) value_type(std::forward<Args>(args)...);
        return true;
    }

    /**
     * @brief   Delete last element
     * @warning Do not use this function if the vector is empty
     * @details Removes the last element in the vector, effectively reducing the container size by
     *          one.
     */
    void pop_back() {
        assert(*this);
        assert(size() > 0);

        CMAGIC_VECTOR_BACK(vector_handle)->~value_type();
        CMAGIC_VECTOR_POP_BACK(vector_handle);
    }

    /**
     * @brief   Removes all elements from the vector (which are destroyed), leaving the container
     *          with a size of 0.
     */
    void clear() {
        while (size() > 0) {
            pop_back();
        }
    }

    /**
     * @brief   Return iterator to beginning
     * @details Returns an iterator pointing to the first element in the vector. If the container is
     *          empty, the returned iterator value shall not be dereferenced.
     * @return  an iterator to the beginning of the sequence container
     */
    value_type *begin() {
        assert(*this);
        return &(*this)[0];
    }

    /**
     * @copydoc vector::begin
     */
    const value_type *begin() const {
        assert(*this);
        return &(*this)[0];
    }

    /**
     * @brief   Return iterator to end
     * @details Returns an iterator referring to the past-the-end element in the vector container.
     *          The past-the-end element is the theoretical element that would follow the last
     *          element in the vector. It does not point to any element, and thus shall not be
     *          dereferenced.
     * @return  an iterator to the element past the end of the sequence
     */
    value_type *end() {
        assert(*this);
        return &(*this)[size()];
    }

    /**
     * @copydoc vector::end
     */
    const value_type *end() const {
        assert(*this);
        return &(*this)[size()];
    }

    /**
     * @brief   Checks if the vector is properly initialized
     * @details Example usage:
     *          @code
     *          cmagic::vector<int> vec;
     *          if (vec) {
     *              vec.push_back(123);
     *          } else {
     *              std::cerr << "Vector allocation failed!\n";
     *          }
     *          @endcode
     * @return  @c true if vector is initialized, @c false if vector allocation has failed and no
     *          operation should be made on it
     */
    operator bool() const {
        return static_cast<bool>(vector_handle);
    }

    ~vector() {
        if (*this) {
            clear();
            CMAGIC_VECTOR_FREE(vector_handle);
        }
    }

};

} // namespace cmagic

#endif /* CMAGIC_VECTOR_HPP */
