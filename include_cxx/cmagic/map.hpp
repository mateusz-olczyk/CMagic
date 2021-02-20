/**
 * @file    map.hpp
 * @brief   Template implementation of a @b map container.
 * @details This is a wrapper over C implementation from @ref map.h
 */

#ifndef CMAGIC_MAP_HPP
#define CMAGIC_MAP_HPP

#include <cassert>
#include <cstddef>
#include <iterator>
#include <new>
#include <type_traits>
#include <utility>
#include "cmagic/map.h"


namespace cmagic {

template<typename Key, typename Value>
class map {

public:
    using key_type = Key;
    using mapped_type = Value;
    using value_type = std::pair<key_type, mapped_type>;
    using size_type = size_t;

    class iterator {

    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using const_pointer = const value_type*;
        using const_reference = const value_type&;

    private:
        cmagic_map_iterator_t internal_iterator;
        value_type adapter;

        constexpr value_type make_adapter() {
            assert(internal_iterator);
            return std::make_pair(*static_cast<const key_type *>(internal_iterator->key),
                                  *static_cast<mapped_type *>(internal_iterator->value));
        }

        void increment() {
            assert(internal_iterator);
            internal_iterator = CMAGIC_MAP_ITERATOR_NEXT(internal_iterator);
            adapter = make_adapter();
        }

        void decrement() {
            assert(internal_iterator);
            internal_iterator = CMAGIC_MAP_ITERATOR_PREV(internal_iterator);
            adapter = make_adapter();
        }

    public:
        const_pointer operator->() const { return &adapter; }
        const_reference operator*() const { return *this->operator->(); }
        bool operator!=(const iterator &other) const { return !(*this == other); }

        iterator(cmagic_map_iterator_t initializer) : internal_iterator(initializer) {
            if (initializer) {
                adapter = make_adapter();
            }
        }

        iterator &operator++() {
            increment();
            return *this;
        }

        iterator operator++(int) {
            iterator to_return = *this;
            ++(*this);
            return to_return;
        }

        iterator &operator--() {
            decrement();
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
    static_assert(std::is_copy_constructible<key_type>(), "key type must be copy-constructible");
    static_assert(std::is_copy_constructible<mapped_type>(),
                  "mapped type must be copy-constructible");
    static_assert(std::is_copy_assignable<mapped_type>(), "mapped type must be copy-assignable");

    CMAGIC_MAP(key_type) map_handle;

    static int key_comparator(const void *void_key1, const void *void_key2) {
        const key_type &key1 = *static_cast<const key_type *>(void_key1);
        const key_type &key2 = *static_cast<const key_type *>(void_key2);
        if (key1 < key2) {
            return -1;
        } else if (key1 > key2) {
            return 1;
        } else {
            return 0;
        }
    }

    explicit map(const cmagic_memory_alloc_packet_t *alloc_packet)
    : map_handle(CMAGIC_MAP_NEW(key_type, mapped_type, key_comparator, alloc_packet)) {}

    template <typename Key_URef, typename Val_URef>
    std::pair<iterator, bool> insert_template(Key_URef &&key, Val_URef &&value) {
        assert(*this);
        cmagic_map_insert_result_t insert_result = CMAGIC_MAP_ALLOCATE(map_handle, &key);
        if (!insert_result.already_exists && insert_result.inserted_or_existing) {
            new(const_cast<void *>(insert_result.inserted_or_existing->key))
                key_type {std::forward<Key_URef>(key)};
            new(insert_result.inserted_or_existing->value)
                mapped_type {std::forward<Val_URef>(value)};
        }
        const bool insert_unique_success =
            insert_result.inserted_or_existing && !insert_result.already_exists;
        return std::make_pair(insert_result.inserted_or_existing, insert_unique_success);
    }

public:

    map() : map(&CMAGIC_MEMORY_ALLOC_PACKET_STD) {}

    static map custom_allocation_map() {
        return map(&CMAGIC_MEMORY_ALLOC_PACKET_CUSTOM_CMAGIC);
    }

    map &operator=(const map &x) {
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
                CMAGIC_MAP_FREE(map_handle);
                map_handle = nullptr;
                return *this;
            }
        }
        return *this;
    }

    map(const map &x) : map(CMAGIC_MAP_GET_ALLOC_PACKET(x.map_handle)) {
        if (*this) {
            operator=(x);
        }
    }

    map &operator=(map &&x) {
        assert(*this);
        clear();
        CMAGIC_MAP_FREE(map_handle);
        map_handle = x.map_handle;
        x.map_handle = CMAGIC_MAP_NEW(key_type, mapped_type, key_comparator,
                                      CMAGIC_MAP_GET_ALLOC_PACKET(x.map_handle));
    }

    map(map &&x) : map_handle(x.map_handle) {
        x.map_handle = CMAGIC_MAP_NEW(key_type, mapped_type, key_comparator,
                                      CMAGIC_MAP_GET_ALLOC_PACKET(x.map_handle));
    }

    operator bool() const {
        return static_cast<bool>(map_handle);
    }

    iterator begin() const {
        assert(map_handle);
        return CMAGIC_MAP_FIRST(map_handle);
    }

    iterator end() const {
        assert(map_handle);
        return nullptr;
    }

    void clear() {
        assert(*this);
        for (cmagic_map_iterator_t it = CMAGIC_MAP_FIRST(map_handle);
             it;
             it = CMAGIC_MAP_ITERATOR_NEXT(it)) {
            const key_type *key_ptr = static_cast<const key_type *>(it->key);
            mapped_type *value_ptr = static_cast<mapped_type *>(it->value);
            key_ptr->~key_type();
            value_ptr->~mapped_type();
        }
        CMAGIC_MAP_CLEAR(map_handle);
    }

    std::pair<iterator, bool> insert(const value_type &val) {
        return insert_template(val.first, val.second);
    }

    std::pair<iterator, bool> insert(value_type &&val) {
        return insert_template(std::move(val.first), std::move(val.second));
    }

    void erase(const key_type &key) {
        assert(*this);
        CMAGIC_MAP_ERASE(map_handle, &key);
    }

    size_type size() const {
        assert(*this);
        return CMAGIC_MAP_SIZE(map_handle);
    }

    bool empty() const {
        return size() == 0;
    }

    iterator find(const key_type &key) const {
        return CMAGIC_MAP_FIND(map_handle, &key);
    }

    ~map() {
        if (*this) {
            clear();
            CMAGIC_MAP_FREE(map_handle);
        }
    }

};

} // namespace cmagic

#endif /* CMAGIC_MAP_HPP */
