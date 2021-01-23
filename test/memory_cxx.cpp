#include <algorithm>
#include <vector>
#include "cmagic/memory.h"
#include "cmagic/utils.h"
#include "unity.h"

template <typename T>
struct CustomAllocator {
    typedef T value_type;
    CustomAllocator() = default;

    T* allocate(std::size_t n) {
        auto ptr = static_cast<T*>(cmagic_memory_malloc(n * sizeof(T)));
        TEST_ASSERT_NOT_NULL_MESSAGE(ptr, "Allocation fail");
        return ptr;
    }

    void deallocate(T* p, std::size_t n) noexcept {
        (void)n;
        TEST_ASSERT_EQUAL_MESSAGE(CMAGIC_MEMORY_FREE_RESULT_OK, cmagic_memory_free(p), "Deallocation fail");
    }
};

//------------------------------------------------------------------------------

/* Test body */

void setUp(void) {}

void tearDown(void) {}

static void test_Vector(void) {
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocated_bytes());
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocations());
    {
        std::vector<int, CustomAllocator<int>> v;
        v.push_back(1);
        v.push_back(2);
        v.push_back(3);
        v.push_back(4);
        v.pop_back();
        v.pop_back();
        v.push_back(5);
        v.push_back(6);
        v.push_back(7);
        v.push_back(8);
        v.pop_back();
        v.pop_back();
        v.push_back(9);
        v.push_back(10);
        v.push_back(11);
        v.push_back(12);

        TEST_ASSERT_GREATER_THAN_size_t(0, cmagic_memory_get_allocated_bytes());
        TEST_ASSERT_GREATER_THAN_size_t(0, cmagic_memory_get_allocations());

        const int expected[] = {1, 2, 5, 6, 9, 10, 11, 12};
        TEST_ASSERT_EQUAL_size_t(CMAGIC_UTILS_ARRAY_SIZE(expected), v.size());
        TEST_ASSERT_EQUAL_INT_ARRAY(expected, v.data(), v.size());
    }
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocated_bytes());
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocations());
}

static void test_Sort(void) {
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocated_bytes());
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocations());
    {
        std::vector<int, CustomAllocator<int>> v = {
            61, 83, 33, 23, 50, 72, 71, 8,  66, 87, 98, 92, 99, 37, 86, 93,  17, 36, 43, 19,
            31, 80, 10, 51, 75, 5,  97, 52, 24, 70, 26, 25, 6,  39, 64, 65,  29, 74, 18, 79,
            45, 12, 44, 30, 58, 89, 76, 42, 67, 94, 91, 34, 60, 22, 81, 54,  48, 63, 55, 68,
            46, 21, 57, 35, 62, 15, 53, 73, 47, 9,  20, 4,  82, 84, 77, 38,  69, 85, 96, 95,
            3,  88, 2,  7,  11, 32, 1,  13, 49, 41, 90, 28, 27, 59, 78, 100, 16, 14, 56, 40
        };

        TEST_ASSERT_GREATER_THAN_size_t(0, cmagic_memory_get_allocated_bytes());
        TEST_ASSERT_GREATER_THAN_size_t(0, cmagic_memory_get_allocations());

        std::sort(v.begin(), v.end());
        const int expected1[] = {
            1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
            21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
            41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
            61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
            81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100
        };
        TEST_ASSERT_EQUAL_size_t(CMAGIC_UTILS_ARRAY_SIZE(expected1), v.size());
        TEST_ASSERT_EQUAL_INT_ARRAY(expected1, v.data(), v.size());

        v.erase(v.begin() + 50, v.end());
        const int expected2[] = {
            1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
            21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
            41, 42, 43, 44, 45, 46, 47, 48, 49, 50
        };
        TEST_ASSERT_EQUAL_size_t(CMAGIC_UTILS_ARRAY_SIZE(expected2), v.size());
        TEST_ASSERT_EQUAL_INT_ARRAY(expected2, v.data(), v.size());
    }
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocated_bytes());
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocations());
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_Vector);
    RUN_TEST(test_Sort);
    return UNITY_END();
}
