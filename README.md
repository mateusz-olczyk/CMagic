# CMagic

[![Cmake](https://github.com/mateusz-olczyk/CMagic/workflows/CMake/badge.svg)](https://github.com/mateusz-olczyk/CMagic/actions?query=workflow%3ACMake)

## What is CMagic?
CMagic is a portable no-exception C library with C++ bindings. It contains utilities and containers
which are not included in C Standard Library. CMagic is purely written in C11 (with possible
fallback to C99) and C++11. It doesn't depend on any other library and doesn't require custom
compiler extensions. Works both on PC and embedded systems.

[Click here for the full documentation.](https://mateusz-olczyk.github.io/CMagic/)

## Features
- **Custom memory allocation** (*cmagic/memory.h*)
  - With `cmagic_memory_malloc()` and `cmagic_memory_free()` you're able to perform dynamic memory
    allocations on systems which do not support it natively.
  - You can specify "dynamic" memory pool size in argument of `cmagic_memory_init()`
  - CMagic doesn't perform any platform dependent syscalls. It maintains these allocations
    internally using static memory buffer.
  - You can use functions like `cmagic_memory_is_allocated()` or
    `cmagic_memory_get_allocated_bytes()` to debug your applications.
- **Utilities** (*cmagic/utils.h*)
  - Provides macros for common C expressions like `CMAGIC_UTILS_ARRAY_SIZE` for checking size of an
    array
- **Containers**
  - **Vector** (*cmagic/vector.h* and *cmagic/vector.hpp*)
  - **Map** (*cmagic/map.h* and *cmagic/vector.h*)
  - **Set** (*cmagic/set.h* and *cmagic/vector.h*)
  - The containers behave similarly as their equivalents known from C++ STL.
  - Allow to specify allocators: standard `malloc()`/`free()` or custom CMagic allocation.
  - Can hold any primitive or custom type elements. Special macros provide basic type checking when
    using C API.
  - Never throw exceptions. Allocation failures are indicated by return values of functions.

## Dependencies
To use CMagic you will need:
  - C compiler supporting at least C99 standard
  - C++ compiler supporting at least C++11 standard
  - [CMake](https://cmake.org/) 3.16
  - Optionally [Doxygen](https://www.doxygen.nl/index.html) if you want do build the documentation
    locally
  - Local unit tests use [Unity Test](https://github.com/ThrowTheSwitch/Unity) framework. It's
    already included here as a Git submodule, so you only need to ensure the submodule is updated.

## Quickstart
Prepare a directory for your project:
```bash
> mkdir cmagic_helloworld
> cd cmagic_helloworld
> git clone --recurse-submodules https://github.com/mateusz-olczyk/CMagic.git cmagic
```
Prepare sources **hello.c** and **hello.cpp** and put them in **cmagic_helloworld** directory:
```c
// hello.c

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
    for (int i = 0; i < 10; i++) {
        printf(" %d", CMAGIC_VECTOR_DATA(vector)[i]);
    }
    puts("");

    CMAGIC_VECTOR_FREE(vector);
    return 0;
}
```
```c++
// hello.cpp

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
```
Eventually prepare a **CMakeLists.txt** file and compile the sources:
```cmake
cmake_minimum_required(VERSION 3.16)
project(cmagic_helloworld)
add_subdirectory(cmagic)
add_executable(hello_c hello.c)
add_executable(hello_cpp hello.cpp)
target_link_libraries(hello_c PRIVATE cmagic)
target_link_libraries(hello_cpp PRIVATE cmagic)
```
```bash
cmake -S . -B build
cmake --build build
```
Both programs should generate the same output:
```
Vector size is 10
Elements: 1 2 3 4 5 6 7 8 9 10
```
