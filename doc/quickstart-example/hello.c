#include <stdio.h>
#include "cmagic/memory.h"
#include "cmagic/vector.h"

int main(void) {
    static uint8_t memory_pool[1000];
    cmagic_memory_init(memory_pool, sizeof(memory_pool));

    CMAGIC_VECTOR(int) vector = CMAGIC_VECTOR_NEW(int, &CMAGIC_MEMORY_ALLOC_PACKET_CUSTOM_CMAGIC);
    for (int i = 1; i <= 10; i++) {
        if (!CMAGIC_VECTOR_PUSH_BACK(vector, &i)) {
            fputs("Insufficient memory!\n", stderr);
            CMAGIC_VECTOR_FREE(vector);
            return -1;
        }
    }

    printf("Vector size is %zu\nElements:", CMAGIC_VECTOR_SIZE(vector));
    for (int i = 0; i < CMAGIC_VECTOR_SIZE(vector); i++) {
        printf(" %d", CMAGIC_VECTOR_DATA(vector)[i]);
    }
    puts("");

    CMAGIC_VECTOR_FREE(vector);
    return 0;
}
