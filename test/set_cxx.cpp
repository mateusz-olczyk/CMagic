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

} // namespace

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_Sorting);
    return UNITY_END();
}
