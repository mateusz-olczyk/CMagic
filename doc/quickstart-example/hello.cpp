#include <iostream>
#include "cmagic/memory.h"
#include "cmagic/vector.hpp"

int main() {
    static uint8_t memory_pool[1000];
    cmagic_memory_init(memory_pool, sizeof(memory_pool));

    cmagic::vector<int> vector {cmagic::vector<int>::custom_allocation_vector()};
    for (int i = 1; i <= 10; i++) {
        if (!vector.push_back(i)) {
            std::cerr << "Insufficient memory!\n";
            return -1;
        }
    }

    std::cout << "Vector size is " << vector.size() << "\nElements:";
    for (int e : vector) {
        std::cout << " " << e;
    }
    std::cout << "\n";
}
