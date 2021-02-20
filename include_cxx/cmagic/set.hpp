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

template<typename T>
class set {

public:
    using value_type = T;
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

    set() : set(&CMAGIC_MEMORY_ALLOC_PACKET_STD) {}

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
    }

    set(set &&x) : set_handle(x.set_handle) {
        x.set_handle = CMAGIC_SET_NEW(value_type, key_comparator,
                                      CMAGIC_SET_GET_ALLOC_PACKET(x.set_handle));
    }

    operator bool() const {
        return static_cast<bool>(set_handle);
    }

    iterator begin() const {
        assert(set_handle);
        return CMAGIC_SET_FIRST(set_handle);
    }

    iterator end() const {
        assert(set_handle);
        return nullptr;
    }

    void clear() {
        assert(*this);
        for (const value_type &key : *this) {
            key.~value_type();
        }
        CMAGIC_SET_CLEAR(set_handle);
    }

    std::pair<iterator, bool> insert(const value_type &val) {
        return insert_template(val);
    }

    std::pair<iterator, bool> insert(value_type &&val) {
        return insert_template(std::move(val));
    }

    void erase(const value_type &key) {
        assert(*this);
        CMAGIC_SET_ERASE(set_handle, &key);
    }

    size_type size() const {
        assert(*this);
        return CMAGIC_SET_SIZE(set_handle);
    }

    bool empty() const {
        return size() == 0;
    }

    iterator find(const value_type &key) const {
        return CMAGIC_SET_FIND(set_handle, &key);
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
