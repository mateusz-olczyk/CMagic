#ifndef CMAGIC_UTILS_H
#define CMAGIC_UTILS_H

#define CMAGIC_DIV_CEIL(dividend, divisor) (((dividend) + (divisor) - 1) / (divisor))
#define CMAGIC_UTILS_ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

#endif /* CMAGIC_UTILS_H */
