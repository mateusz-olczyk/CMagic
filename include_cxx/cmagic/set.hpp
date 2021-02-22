/**
 * @file    set.hpp
 * @brief   Template implementation of a @b set container.
 * @details This is a wrapper over C implementation from @ref set.h
 */

#ifndef CMAGIC_SET_HPP
#define CMAGIC_SET_HPP

#include <cassert>
#include <cstddef>
#include <iterator>
#include <new>
#include <type_traits>
#include "cmagic/set.h"


namespace cmagic {

/**
 * @brief   A container that stores unique elements following a specific order.
 * @details Each value in the set is unique. The value of the elements in a set cannot be modified
 *          once in the container but they can be inserted or removed from the container. Set is
 *          implemented as an AVL tree.
 */
template<typename T>
class set {

public:
    /**
     * @brief   Type of set elements.
     */
    using value_type = T;

    /**
     * @brief   Type used to measure element size.
     */
    using size_type = size_t;

    class iterator {

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using const_pointer = const value_type*;
        using const_reference = const value_type&;

    private:
        cmagic_set_iterator_t internal_iterator;
    
    public:
        iterator(cmagic_set_iterator_t initializer) : internal_iterator(initializer) {}
        const_reference operator*() const { return *this->operator->(); }
        bool operator!=(const iterator &other) const { return !(*this == other); }

        const_pointer operator->() const {
            return static_cast<const_pointer>(internal_iterator->key);
        }

        iterator &operator++() {
            assert(internal_iterator);
            internal_iterator = CMAGIC_SET_ITERATOR_NEXT(internal_iterator);
            return *this;
        }

        iterator operator++(int) {
            iterator to_return = *this;
            ++(*this);
            return to_return;
        }

        iterator &operator--() {
            assert(internal_iterator);
            internal_iterator = CMAGIC_SET_ITERATOR_PREV(internal_iterator);
            return *this;
        }

        iterator operator--(int) {
            iterator to_return = *this;
            --(*this);
            return to_return;
        }

        bool operator==(const iterator &other) const {
            return this->internal_iterator == other.internal_iterator;
        }

    };

private:
    static_assert(std::is_copy_assignable<T>(), "value type must be copy-assignable");
    static_assert(std::is_copy_constructible<T>(), "value type must be copy-constructible");
    CMAGIC_SET(value_type) set_handle;

    static int key_comparator(const void *void_key1, const void *void_key2) {
        const value_type &key1 = *static_cast<const value_type *>(void_key1);
        const value_type &key2 = *static_cast<const value_type *>(void_key2);
        if (key1 < key2) {
            return -1;
        } else if (key1 > key2) {
            return 1;
        } else {
            return 0;
        }
    }

    explicit set(const cmagic_memory_alloc_packet_t *alloc_packet)
    : set_handle(CMAGIC_SET_NEW(value_type, key_comparator, alloc_packet)) {}

    template <typename URef>
    std::pair<iterator, bool> insert_template(URef &&val) {
        assert(*this);
        cmagic_set_insert_result_t insert_result = CMAGIC_SET_ALLOCATE(set_handle, &val);
        if (!insert_result.already_exists && insert_result.inserted_or_existing) {
            new(const_cast<void *>(insert_result.inserted_or_existing->key))
                value_type {std::forward<URef>(val)};
        }
        const bool insert_unique_success =
            insert_result.inserted_or_existing && !insert_result.already_exists;
        return std::make_pair(insert_result.inserted_or_existing, insert_unique_success);
    }

public:
    /**
     * @brief   Constructs an empty set with standard memory allocation.
     * @return  a new empty set
     */
    set() : set(&CMAGIC_MEMORY_ALLOC_PACKET_STD) {}

    /**
     * @brief   Constructs an empty set using custom @e CMagic memory allocation from @ref memory.h
     * @return  a new empty set
     */
    static set custom_allocation_set() {
        return set(&CMAGIC_MEMORY_ALLOC_PACKET_CUSTOM_CMAGIC);
    }

    set &operator=(const set &x) {
        assert(*this);
        if (&x == this) {
            return *this;
        }

        clear();
        for (const value_type &val : x) {
            auto insert_result = insert(val);
            assert(insert_result.first == end() || insert_result.second);
            if (insert_result.first == end()) {
                clear();
                CMAGIC_SET_FREE(set_handle);
                set_handle = nullptr;
                return *this;
            }
        }
        return *this;
    }

    set(const set &x) : set(CMAGIC_SET_GET_ALLOC_PACKET(x.set_handle)) {
        if (*this) {
            operator=(x);
        }
    }

    set &operator=(set &&x) {
        assert(*this);
        clear();
        CMAGIC_SET_FREE(set_handle);
        set_handle = x.set_handle;
        x.set_handle = CMAGIC_SET_NEW(value_type, key_comparator,
                                      CMAGIC_SET_GET_ALLOC_PACKET(x.set_handle));
        return *this;
    }

    set(set &&x) : set_handle(x.set_handle) {
        x.set_handle = CMAGIC_SET_NEW(value_type, key_comparator,
                                      CMAGIC_SET_GET_ALLOC_PACKET(x.set_handle));
    }

    /**
     * @brief   Checks if the set is properly initialized
     * @details Example usage:
     *          @code
     *          cmagic::set<int> set;
     *          if (set) {
     *              set.insert(123);
     *          } else {
     *              std::cerr << "Set allocation failed!\n";
     *          }
     *          @endcode
     * @return  @c true if set is initialized, @c false if set allocation has failed and no
     *          operation should be made on it
     */
    operator bool() const {
        return static_cast<bool>(set_handle);
    }

    /**
     * @brief   Return iterator to beginning
     * @details Returns an iterator pointing to the first element in the set. If the container is
     *          empty, the returned iterator value shall not be dereferenced.
     * @return  an iterator to the beginning of the container
     */
    iterator begin() const {
        assert(set_handle);
        return CMAGIC_SET_FIRST(set_handle);
    }

    /**
     * @brief   Return iterator to end
     * @details It does not point to any element, and thus shall not be dereferenced.
     * @return  an iterator to the element past the end of the sequence
     */
    iterator end() const {
        assert(set_handle);
        return nullptr;
    }

    /**
     * @brief   Removes all elements from the set (which are destroyed), leaving the container
     *          with a size of 0.
     */
    void clear() {
        assert(*this);
        for (const value_type &key : *this) {
            key.~value_type();
        }
        CMAGIC_SET_CLEAR(set_handle);
    }

    /**
     * @brief   Inserts a new element to the set if it is not equivalent to any element already
     *          contained in the set.
     * @details Because elements in a set are unique, the insertion operation checks whether an
     *          inserted element is equivalent to an element already in the container, and if so,
     *          the element is not inserted, returning an iterator to this existing element.
     * @param   val value to be copied (or moved) to the set
     * @return  a pair, with its member @c pair::first set to an iterator pointing to either the
     *          newly inserted element or to the equivalent element already in the set or @ref end
     *          if allocation of the new element has failed. The @c pair::second element in the pair
     *          is set to @c true if a new element was inserted or @c false if an equivalent element
     *          already existed (or could not be inserted due to allocation failure).
     */
    std::pair<iterator, bool> insert(const value_type &val) {
        return insert_template(val);
    }

    /**
     * @copydoc set::insert
     */
    std::pair<iterator, bool> insert(value_type &&val) {
        return insert_template(std::move(val));
    }

    /**
     * @brief   Removes a single element from the set
     * @param   val value to be removed from the set. Function does nothing if the element doesn't
     *          exist in the set.
     */
    void erase(const value_type &val) {
        assert(*this);
        iterator val_to_delete = find(val);
        if (val_to_delete != end()) {
            CMAGIC_SET_ERASE(set_handle, &val);
            val_to_delete->~value_type();
        }
    }

    /**
     * @brief   Returns the number of elements in the set.
     * @return  number of elements in the set
     */
    size_type size() const {
        assert(*this);
        return CMAGIC_SET_SIZE(set_handle);
    }

    /**
     * @brief   Returns whether the set is empty (i.e. whether its size is 0).
     * @details This function does not modify the container in any way. To clear the content of a
     *          set, see @ref set::clear.
     * @return  @c true if the container size is 0, @c false otherwise
     */
    bool empty() const {
        return size() == 0;
    }

    /**
     * @brief   Searches the container for an element equivalent to @p val and returns an iterator
     *          to it if found, otherwise it returns @ref set::end.
     * @param   val value to be searched for
     * @return  an iterator to the element, if @p val is found, or @ref set::end otherwise
     */
    iterator find(const value_type &val) const {
        return CMAGIC_SET_FIND(set_handle, &val);
    }

    ~set() {
        if (*this) {
            clear();
            CMAGIC_SET_FREE(set_handle);
        }
    }

};

} // namespace cmagic

#endif /* CMAGIC_SET_HPP */
