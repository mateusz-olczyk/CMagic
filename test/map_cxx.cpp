#include <string>
#include "cmagic/memory.h"
#include "cmagic/map.hpp"
#include "unity.h"

void setUp() {
    static uint8_t memory_pool[5000];
    cmagic_memory_init(memory_pool, sizeof(memory_pool));
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocations());
}

void tearDown() {
    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocations());
}

namespace {

void test_Insert() {
    using map_type = cmagic::map<std::string, int>;
    using val_type = map_type::value_type;
    map_type str_int_map;

    auto a_result = str_int_map.insert(val_type("Alex", 100));
    auto b_result = str_int_map.insert(val_type("Barbara", 200));
    auto c_result = str_int_map.insert(val_type("Claudia", 300));
    auto d_result = str_int_map.insert(val_type("David", 400));

    TEST_ASSERT_TRUE(a_result.second);
    TEST_ASSERT_TRUE(b_result.second);
    TEST_ASSERT_TRUE(c_result.second);
    TEST_ASSERT_TRUE(d_result.second);

    TEST_ASSERT_EQUAL_STRING("Alex", a_result.first->first.c_str());
    TEST_ASSERT_EQUAL_INT(100, a_result.first->second);
    TEST_ASSERT_EQUAL_STRING("Barbara", b_result.first->first.c_str());
    TEST_ASSERT_EQUAL_INT(200, b_result.first->second);
    TEST_ASSERT_EQUAL_STRING("Claudia", c_result.first->first.c_str());
    TEST_ASSERT_EQUAL_INT(300, c_result.first->second);
    TEST_ASSERT_EQUAL_STRING("David", d_result.first->first.c_str());
    TEST_ASSERT_EQUAL_INT(400, d_result.first->second);

    TEST_ASSERT_EQUAL_size_t(4, str_int_map.size());
}

void test_Erase() {
    using map_type = cmagic::map<std::string, int>;
    using val_type = map_type::value_type;
    map_type str_int_map;

    str_int_map.insert(val_type("Alex", 100));
    str_int_map.insert(val_type("Barbara", 200));
    str_int_map.insert(val_type("Claudia", 300));
    str_int_map.insert(val_type("David", 400));
    TEST_ASSERT_EQUAL_size_t(4, str_int_map.size());
    TEST_ASSERT_FALSE(str_int_map.find("Claudia") == str_int_map.end());

    str_int_map.erase("Claudia");
    TEST_ASSERT_EQUAL_size_t(3, str_int_map.size());
    TEST_ASSERT_TRUE(str_int_map.find("Claudia") == str_int_map.end());
}

void test_RangeLoop() {
    using map_type = cmagic::map<std::string, int>;
    using val_type = map_type::value_type;
    map_type str_int_map = map_type::custom_allocation_map();
    TEST_ASSERT_TRUE(str_int_map);

    str_int_map.insert(val_type("Alex", 100));
    str_int_map.insert(val_type("Barbara", 200));
    str_int_map.insert(val_type("Claudia", 300));
    str_int_map.insert(val_type("David", 400));
    TEST_ASSERT_EQUAL_size_t(4, str_int_map.size());

    char iteration_id {'a'};
    for (auto &&e : str_int_map) {
        switch (iteration_id) {
        case 'a':
            TEST_ASSERT_EQUAL_STRING("Alex", e.first.c_str());
            TEST_ASSERT_EQUAL_INT(100, e.second);
            break;
        case 'b':
            TEST_ASSERT_EQUAL_STRING("Barbara", e.first.c_str());
            TEST_ASSERT_EQUAL_INT(200, e.second);
            break;
        case 'c':
            TEST_ASSERT_EQUAL_STRING("Claudia", e.first.c_str());
            TEST_ASSERT_EQUAL_INT(300, e.second);
            break;
        case 'd':
            TEST_ASSERT_EQUAL_STRING("David", e.first.c_str());
            TEST_ASSERT_EQUAL_INT(400, e.second);
            break;
        default:
            TEST_FAIL();
        }
        iteration_id++;
    }
}

void test_CopyAndMove() {
    using map_type = cmagic::map<std::string, int>;
    using val_type = map_type::value_type;
    map_type str_int_map = map_type::custom_allocation_map();
    TEST_ASSERT_TRUE(str_int_map);

    str_int_map.insert(val_type("Alex", 100));
    str_int_map.insert(val_type("Barbara", 200));
    str_int_map.insert(val_type("Claudia", 300));
    str_int_map.insert(val_type("David", 400));
    TEST_ASSERT_EQUAL_size_t(4, str_int_map.size());

    map_type map_copy {str_int_map};
    TEST_ASSERT_EQUAL_size_t(4, str_int_map.size());
    TEST_ASSERT_EQUAL_size_t(4, map_copy.size());
    map_copy.insert(val_type("Ellen", 500));
    TEST_ASSERT_EQUAL_size_t(4, str_int_map.size());
    TEST_ASSERT_EQUAL_size_t(5, map_copy.size());

    TEST_ASSERT_TRUE(str_int_map.find("Ellen") == str_int_map.end());
    TEST_ASSERT_FALSE(map_copy.find("Ellen") == map_copy.end());

    str_int_map = std::move(map_copy);
    TEST_ASSERT_EQUAL_size_t(5, str_int_map.size());
    TEST_ASSERT_EQUAL_size_t(0, map_copy.size());
    TEST_ASSERT_FALSE(str_int_map.find("Ellen") == str_int_map.end());
}

} // namespace

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_Insert);
    RUN_TEST(test_Erase);
    RUN_TEST(test_RangeLoop);
    RUN_TEST(test_CopyAndMove);
    return UNITY_END();
}
