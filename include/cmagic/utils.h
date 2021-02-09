/**
 * @file    utils.h
 * @brief   General purpose utilities.
 */

#ifndef CMAGIC_UTILS_H
#define CMAGIC_UTILS_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Performs division of two integers and returns ceiling of the result.
 */
#define CMAGIC_UTILS_DIV_CEIL(dividend, divisor) (((dividend) + (divisor) - 1) / (divisor))

/**
 * @brief   Returns the capacity of the array.
 * @warning Use only when have access to the array definition. This won't work for an array passed
 *          via a pointer.
 */
#define CMAGIC_UTILS_ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

/**
 * @brief   Checks if two L-value expressions have the same type.
 * @details The expressions must be L-values because this macro needs addresses of them. Subtraction
 *          of two pointers without any casting causes compilation error if the types don't match.
 */
#define CMAGIC_UTILS_ASSERT_SAME_TYPE(expr1, expr2) ((void)sizeof(&(expr1)-&(expr2)))

/**
 * @brief Returns smaller of two values.
 */
#define CMAGIC_UTILS_MIN(val1, val2) ((val1) < (val2) ? (val1) : (val2))

/**
 * @brief   Increases the address if it's not aligned.
 * @param   unaligned_addr      original address
 * @param   required_alignment  required alignment of the resulting address
 * @return  @p unaligned_addr rounded UP so it's divisible by @p required_alignment
 */
uintptr_t
cmagic_utils_align_address_up(uintptr_t unaligned_addr,
                              size_t required_alignment);

/**
 * @brief   Decreases the address if it's not aligned.
 * @param   unaligned_addr      original address
 * @param   required_alignment  required alignment of the resulting address
 * @return  @p unaligned_addr rounded DOWN so it's divisible by @p required_alignment
 */
uintptr_t
cmagic_utils_align_address_down(uintptr_t unaligned_addr,
                                size_t required_alignment);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CMAGIC_UTILS_H */
