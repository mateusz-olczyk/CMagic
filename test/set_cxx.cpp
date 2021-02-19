#include <algorithm>
#include <string>
#include <vector>
#include "cmagic/memory.h"
#include "cmagic/set.hpp"
#include "unity.h"

//remove later!!!!
#include<iostream>

void setUp() {
    static uint8_t memory_pool[5000];
    cmagic_memory_init(memory_pool, sizeof(memory_pool));
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocations());
}

void tearDown() {
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocations());
}

namespace {

void test_Sorting() {
    cmagic::set<std::string> str_set {cmagic::set<std::string>::custom_allocation_set()};
    std::vector<std::string> words {
        "Oliver", "Jake", "Noah", "James", "Jack", "Connor", "Liam", "John", "Harry", "Callum"
    };

    TEST_ASSERT_TRUE(str_set.empty());

    for (const std::string &word : words) {
        auto result = str_set.insert(word);
        TEST_ASSERT_FALSE(result.first == str_set.end());
        TEST_ASSERT_TRUE(result.second);
        TEST_ASSERT_FALSE(str_set.empty());
    }

    TEST_ASSERT_EQUAL_size_t(words.size(), str_set.size());

    {
        auto result = str_set.insert("James");
        TEST_ASSERT_FALSE(result.first == str_set.end());
        TEST_ASSERT_FALSE(result.second);
    }

    std::sort(words.begin(), words.end());
    {
        size_t iteration = 0;
        for (const std::string &word : str_set) {
            TEST_ASSERT_LESS_THAN_size_t(words.size(), iteration);
            TEST_ASSERT_EQUAL_STRING(words[iteration].c_str(), word.c_str());
            TEST_ASSERT_TRUE(words[iteration] == word);
            iteration++;
        }
    }
}

void test_Erase() {
    cmagic::set<int> str_set {cmagic::set<int>::custom_allocation_set()};
    std::vector<int> numbers {
        16, 3, 11, 6, 5, 17, 4, 19, 15, 10, 20, 12, 9, 13, 2, 18, 1, 8, 14, 7
    };

    TEST_ASSERT_TRUE(str_set.empty());

    for (int number : numbers) {
        auto result = str_set.insert(number);
        TEST_ASSERT_FALSE(result.first == str_set.end());
        TEST_ASSERT_TRUE(result.second);
        TEST_ASSERT_FALSE(str_set.empty());
    }
    
    TEST_ASSERT_EQUAL_size_t(numbers.size(), str_set.size());

    str_set.erase(10);
    TEST_ASSERT_EQUAL_size_t(numbers.size() - 1, str_set.size());
    {
        size_t iteration = 1;
        for (int number : str_set) {
            if (iteration < 10) {
                TEST_ASSERT_EQUAL_INT(iteration, number);
            } else {
                TEST_ASSERT_EQUAL_INT(iteration + 1, number);
            }
            iteration++;
        }
    }
}

} // namespace

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_Sorting);
    RUN_TEST(test_Erase);
    return UNITY_END();
}
