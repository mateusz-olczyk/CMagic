#ifndef CMAGIC_UTILS_H
#define CMAGIC_UTILS_H

#include <stddef.h>
#include <stdint.h>

#define CMAGIC_UTILS_DIV_CEIL(dividend, divisor) \
    (((dividend) + (divisor) - 1) / (divisor))

#define CMAGIC_UTILS_ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

#define CMAGIC_ASSERT_SAME_TYPE(expr1, expr2) ((void)sizeof(&(expr1)-&(expr2)))

#define CMAGIC_MIN(val1, val2) ((val1) < (val2) ? (val1) : (val2))

uintptr_t
cmagic_utils_align_address_up(uintptr_t unaligned_addr,
                              size_t required_alignment);

uintptr_t
cmagic_utils_align_address_down(uintptr_t unaligned_addr,
                                size_t required_alignment);

#endif /* CMAGIC_UTILS_H */
