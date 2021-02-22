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
    /**
     * @brief   Type of map keys
     */
    using key_type = Key;

    /**
     * @brief   Type of map values
     */
    using mapped_type = Value;

    /**
     * @brief   Type of map elements
     */
    using value_type = std::pair<key_type, mapped_type>;

    /**
     * @brief   Type used to measure element size
     */
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

        void update_adapter() {
            if (internal_iterator) {
                adapter = make_adapter();
            }
        }

        void increment() {
            assert(internal_iterator);
            internal_iterator = CMAGIC_MAP_ITERATOR_NEXT(internal_iterator);
            update_adapter();
        }

        void decrement() {
            assert(internal_iterator);
            internal_iterator = CMAGIC_MAP_ITERATOR_PREV(internal_iterator);
            update_adapter();
        }

    public:
        const_pointer operator->() const { return &adapter; }
        const_reference operator*() const { return *this->operator->(); }
        bool operator!=(const iterator &other) const { return !(*this == other); }

        iterator(cmagic_map_iterator_t initializer) : internal_iterator(initializer) {
            update_adapter();
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
    /**
     * @brief   Constructs an empty map with standard memory allocation.
     * @return  a new empty map
     */
    map() : map(&CMAGIC_MEMORY_ALLOC_PACKET_STD) {}

    /**
     * @brief   Constructs an empty map using custom @e CMagic memory allocation from @ref memory.h
     * @return  a new empty map
     */
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
        return *this;
    }

    map(map &&x) : map_handle(x.map_handle) {
        x.map_handle = CMAGIC_MAP_NEW(key_type, mapped_type, key_comparator,
                                      CMAGIC_MAP_GET_ALLOC_PACKET(x.map_handle));
    }

    /**
     * @brief   Checks if the map is properly initialized
     * @details Example usage:
     *          @code
     *          cmagic::map<std::string, int> map;
     *          if (map) {
     *              map.insert({ "key", 123 });
     *          } else {
     *              std::cerr << "Map allocation failed!\n";
     *          }
     *          @endcode
     * @return  @c true if map is initialized, @c false if map allocation has failed and no
     *          operation should be made on it
     */
    operator bool() const {
        return static_cast<bool>(map_handle);
    }

    /**
     * @brief   Return iterator to beginning
     * @details Returns an iterator pointing to the first element in the map. If the container is
     *          empty, the returned iterator value shall not be dereferenced.
     * @return  an iterator to the beginning of the container
     */
    iterator begin() const {
        assert(map_handle);
        return CMAGIC_MAP_FIRST(map_handle);
    }

    /**
     * @brief   Return iterator to end
     * @details It does not point to any element, and thus shall not be dereferenced.
     * @return  an iterator to the element past the end of the sequence
     */
    iterator end() const {
        assert(map_handle);
        return nullptr;
    }

    /**
     * @brief   Removes all elements from the map, leaving the container with a size of 0.
     */
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

    /**
     * @brief   Inserts a new element to the map if its key is not equivalent to any element already
     *          contained in the map.
     * @details Because keys in a map are unique, the insertion operation checks whether an
     *          inserted element is equivalent to an element already in the container, and if so,
     *          the element is not inserted, returning an iterator to this existing element.
     * @param   val value to be copied (or moved) to the map
     * @return  a pair, with its member @c pair::first map to an iterator pointing to either the
     *          newly inserted element or to the equivalent element already in the map or @ref end
     *          if allocation of the new element has failed. The @c pair::second element in the pair
     *          is set to @c true if a new element was inserted or @c false if an equivalent element
     *          already existed (or could not be inserted due to allocation failure).
     */
    std::pair<iterator, bool> insert(const value_type &val) {
        return insert_template(val.first, val.second);
    }

    /**
     * @copydoc map::insert
     */
    std::pair<iterator, bool> insert(value_type &&val) {
        return insert_template(std::move(val.first), std::move(val.second));
    }

    /**
     * @brief   Removes a single element from the map
     * @param   key key of the value to be removed from the map. Function does nothing if the key
     *          doesn't exist in the map.
     */
    void erase(const key_type &key) {
        assert(*this);
        CMAGIC_MAP_ERASE_EXT(map_handle, &key, [](void *key, void *value) {
            static_cast<key_type *>(key)->~key_type();
            static_cast<mapped_type *>(value)->~mapped_type();
        });
    }

    /**
     * @brief   Returns the number of elements in the map
     * @return  number of elements in the map
     */
    size_type size() const {
        assert(*this);
        return CMAGIC_MAP_SIZE(map_handle);
    }

    /**
     * @brief   Returns whether the map is empty (i.e. whether its size is 0).
     * @details This function does not modify the container in any way. To clear the content of a
     *          map, see @ref map::clear.
     * @return  @c true if the container size is 0, @c false otherwise
     */
    bool empty() const {
        return size() == 0;
    }

    /**
     * @brief   Searches the container for an element with a key equivalent to @p key and returns an
     *          iterator to it if found, otherwise it returns @ref map::end.
     * @param   key key to be searched for
     * @return  an iterator to the element, if @p key is found, or @ref map::end otherwise
     */
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
