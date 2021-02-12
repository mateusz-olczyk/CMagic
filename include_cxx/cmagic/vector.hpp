#ifndef CMAGIC_VECTOR_HPP
#define CMAGIC_VECTOR_HPP

#include <cassert>
#include <type_traits>
#include <new>
#include "cmagic/vector.h"


namespace cmagic {

template<typename T>
class vector {

public:
    using value_type = T;
    using size_type = size_t;

private:
    static_assert(std::is_copy_assignable<T>(), "value type must be copy-assignable");
    static_assert(std::is_copy_constructible<T>(), "value type must be copy-constructible");
    CMAGIC_VECTOR(T) vector_handle;

    explicit vector(const cmagic_memory_alloc_packet_t *alloc_packet)
    : vector_handle(CMAGIC_VECTOR_NEW(value_type, alloc_packet)) {}

public:
    vector() : vector(&CMAGIC_MEMORY_ALLOC_PACKET_STD) {}

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

    size_type size() const {
        assert(*this);
        return CMAGIC_VECTOR_SIZE(vector_handle);
    }

    value_type &operator[](size_type pos) {
        assert(pos <= size());
        return CMAGIC_VECTOR_DATA(vector_handle)[pos];
    }

    const value_type &operator[](size_type pos) const {
        assert (pos <= size());
        return CMAGIC_VECTOR_DATA(vector_handle)[pos];
    }

    bool push_back(const value_type &val) {
        assert(*this);
        if (CMAGIC_VECTOR_PUSH_BACK(vector_handle, &val)) {
            return false;
        }

        new(CMAGIC_VECTOR_BACK(vector_handle)) value_type {val};
        return true;
    }

    void pop_back() {
        assert(*this);
        assert(size() > 0);

        CMAGIC_VECTOR_BACK(vector_handle)->~value_type();
        CMAGIC_VECTOR_POP_BACK(vector_handle);
    }

    void clear() {
        while (size() > 0) {
            pop_back();
        }
    }

    value_type *begin() {
        assert(*this);
        return &(*this)[0];
    }

    const value_type *begin() const {
        assert(*this);
        return &(*this)[0];
    }

    value_type *end() {
        assert(*this);
        return &(*this)[size()];
    }

    const value_type *end() const {
        assert(*this);
        return &(*this)[size()];
    }

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
