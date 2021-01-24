#include "cmagic/utils.h"

uintptr_t
cmagic_utils_align_address_up(uintptr_t unaligned_addr,
                              size_t required_alignment) {
    return (unaligned_addr / required_alignment + 1) * required_alignment;
}

uintptr_t
cmagic_utils_align_address_down(uintptr_t unaligned_addr,
                                size_t required_alignment) {
    return unaligned_addr / required_alignment * required_alignment;
}
