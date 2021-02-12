#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "cmagic/vector.h"

static const size_t VECTOR_MIN_CAPACITY = 5;

#ifndef NDEBUG
static const int_least32_t VECTOR_MAGIC_VALUE = 'V' << 24 | 'E' << 16 | 'C' << 8 | 'T';
#endif

typedef struct {
#ifndef NDEBUG
    int_least32_t magic_value;
#endif
    const cmagic_memory_alloc_packet_t *alloc_packet;
    size_t size;
    size_t capacity;
    size_t member_size;
    void *data_begin;
} vector_descriptor_t;

void **
cmagic_vector_new(size_t member_size, const cmagic_memory_alloc_packet_t *alloc_packet) {
    vector_descriptor_t *vector_descriptor =
        (vector_descriptor_t *) alloc_packet->malloc_function(sizeof(vector_descriptor_t));
    if (!vector_descriptor) {
        return NULL;
    }

    void *data_begin = alloc_packet->malloc_function(VECTOR_MIN_CAPACITY * member_size);
    if (!data_begin) {
        alloc_packet->free_function(vector_descriptor);
        return NULL;
    }

    *vector_descriptor = (vector_descriptor_t) {
#ifndef NDEBUG
        .magic_value = VECTOR_MAGIC_VALUE,
#endif
        .alloc_packet = alloc_packet,
        .size = 0,
        .capacity = VECTOR_MIN_CAPACITY,
        .member_size = member_size,
        .data_begin = data_begin
    };

    return &vector_descriptor->data_begin;
}

static vector_descriptor_t *_get_vector_descriptor(void **vector_ptr) {
    assert(vector_ptr);
    vector_descriptor_t *result = (vector_descriptor_t *)(
        (const char *)vector_ptr - offsetof(vector_descriptor_t, data_begin));
    assert(result->magic_value == VECTOR_MAGIC_VALUE);
    return result;
}

void
cmagic_vector_free(void **vector_ptr) {
    vector_descriptor_t *vector_descriptor = _get_vector_descriptor(vector_ptr);
    vector_descriptor->alloc_packet->free_function(vector_descriptor->data_begin);
    vector_descriptor->alloc_packet->free_function(vector_descriptor);
}

static int _change_capacity(vector_descriptor_t *vector_descriptor,
                            size_t new_capacity) {
    assert(vector_descriptor->size <= new_capacity);
    void *new_data_begin = vector_descriptor->alloc_packet->realloc_function(
        vector_descriptor->data_begin, new_capacity * vector_descriptor->member_size);
    if (!new_data_begin) {
        return -1;
    }

    vector_descriptor->capacity = new_capacity;
    vector_descriptor->data_begin = new_data_begin;
    return 0;
}

int
cmagic_vector_allocate_back(void **vector_ptr) {
    vector_descriptor_t *vector_descriptor = _get_vector_descriptor(vector_ptr);

    if (vector_descriptor->size == vector_descriptor->capacity) {
        int change_capacity_result = _change_capacity(vector_descriptor,
            2 * vector_descriptor->capacity);
        if (change_capacity_result) {
            return change_capacity_result;
        }
    }

    assert(vector_descriptor->size < vector_descriptor->capacity);
    vector_descriptor->size++;
    return 0;
}

int
cmagic_vector_push_back(void **vector_ptr, const void *new_element_ptr) {
    int allocate_result = cmagic_vector_allocate_back(vector_ptr);
    if (allocate_result) {
        return allocate_result;
    }

    vector_descriptor_t *vector_descriptor = _get_vector_descriptor(vector_ptr);
    void *last_element = (char *)vector_descriptor->data_begin
        + (vector_descriptor->size - 1) * vector_descriptor->member_size;
    memcpy(last_element, new_element_ptr, vector_descriptor->member_size);

    return 0;
}

void
cmagic_vector_pop_back(void **vector_ptr) {
    vector_descriptor_t *vector_descriptor = _get_vector_descriptor(vector_ptr);
    assert(vector_descriptor->size > 0);

    vector_descriptor->size--;
    if (vector_descriptor->capacity > VECTOR_MIN_CAPACITY
        && vector_descriptor->size <= vector_descriptor->capacity / 4) {
        (void) _change_capacity(vector_descriptor, vector_descriptor->capacity / 2);
    }
}

size_t
cmagic_vector_size(void **vector_ptr) {
    return _get_vector_descriptor(vector_ptr)->size;
}

const cmagic_memory_alloc_packet_t *
cmagic_vector_get_alloc_packet(void **vector_ptr) {
    return _get_vector_descriptor(vector_ptr)->alloc_packet;
}
