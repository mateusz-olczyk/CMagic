#ifndef CMAGIC_UTILS_H
#define CMAGIC_UTILS_H

#include <stddef.h>
#include <stdint.h>

#define CMAGIC_UTILS_DIV_CEIL(dividend, divisor) \
    (((dividend) + (divisor) - 1) / (divisor))

#define CMAGIC_UTILS_ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

uintptr_t
cmagic_utils_align_address_up(uintptr_t unaligned_addr,
                              size_t required_alignment);

uintptr_t
cmagic_utils_align_address_down(uintptr_t unaligned_addr,
                                size_t required_alignment);

#endif /* CMAGIC_UTILS_H */
