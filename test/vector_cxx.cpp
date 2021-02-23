#include <algorithm>
#include <iterator>
#include <memory>
#include <string>
#include <vector>
#include "cmagic/vector.hpp"
#include "unity.h"

void setUp() {}

void tearDown() {}

namespace {

static void test_push_back_int() {
    cmagic::vector<int> vec;
    TEST_ASSERT_TRUE(vec);
    TEST_ASSERT_TRUE(vec.empty());
    TEST_ASSERT_TRUE(vec.push_back(1));
    TEST_ASSERT_FALSE(vec.empty());
    TEST_ASSERT_TRUE(vec.push_back(2));
    TEST_ASSERT_TRUE(vec.push_back(3));
    TEST_ASSERT_TRUE(vec.push_back(5));
    TEST_ASSERT_TRUE(vec.push_back(8));
    TEST_ASSERT_TRUE(vec.push_back(13));
    TEST_ASSERT_TRUE(vec.push_back(21));
    TEST_ASSERT_TRUE(vec.push_back(34));
    TEST_ASSERT_TRUE(vec.push_back(55));
    TEST_ASSERT_TRUE(vec.push_back(89));
    TEST_ASSERT_TRUE(vec.push_back(144));
    TEST_ASSERT_TRUE(vec.push_back(233));
    TEST_ASSERT_TRUE(vec.push_back(377));
    TEST_ASSERT_TRUE(vec.push_back(610));
    TEST_ASSERT_TRUE(vec.push_back(987));
    TEST_ASSERT_TRUE(vec.push_back(1597));
    TEST_ASSERT_TRUE(vec.push_back(2584));
    TEST_ASSERT_TRUE(vec.push_back(4181));
    TEST_ASSERT_EQUAL_size_t(18, vec.size());

    int previous1 {};
    int previous2 {};
    for (const int &val : vec) {
        if (previous1 && previous2) {
            TEST_ASSERT_EQUAL_INT(previous1 + previous2, val);
            previous1 = previous2;
            previous2 = val;
        } else if (previous1) {
            previous2 = val;
        } else {
            previous1 = val;
        }
    }
}

struct mem_mgmt {
    int allocations {};
    int deallocations {};
};

struct object {
    int val;
    std::shared_ptr<mem_mgmt> owner;

    object &operator=(const object &) = default;

    // Dummy move assignment just copies
    object &operator=(object &&x) {
        return operator=(x);
    }

    object(const object &x) : val(x.val), owner(x.owner) {
        owner->allocations++;
    }

    // Dummy move constructor just copies
    object(object &&x) : val(x.val), owner(x.owner) {
        owner->allocations++;
    }

    object(int val_, std::shared_ptr<mem_mgmt> owner_) : val(val_), owner(owner_) {
        owner->allocations++;
    }

    ~object() {
        owner->deallocations++;
    }
};

void test_memory_management() {
    auto mgmt {std::make_shared<mem_mgmt>()}; 
    {
        cmagic::vector<object> vec;
        TEST_ASSERT_TRUE(vec);
        TEST_ASSERT_EQUAL_INT(0, mgmt->allocations);
        TEST_ASSERT_EQUAL_INT(0, mgmt->deallocations);

        TEST_ASSERT_TRUE(vec.push_back(object(123, mgmt)));
        // One temporary object, one stored in the vector
        TEST_ASSERT_EQUAL_INT(2, mgmt->allocations); 
        TEST_ASSERT_EQUAL_INT(1, mgmt->deallocations);

        TEST_ASSERT_TRUE(vec.push_back(object(456, mgmt)));
        // Similarly again
        TEST_ASSERT_EQUAL_INT(4, mgmt->allocations); 
        TEST_ASSERT_EQUAL_INT(2, mgmt->deallocations);

        TEST_ASSERT_TRUE(vec.push_back(object(789, mgmt)));
        // And again
        TEST_ASSERT_EQUAL_INT(6, mgmt->allocations); 
        TEST_ASSERT_EQUAL_INT(3, mgmt->deallocations);

        // One element deleted
        vec.pop_back();
        TEST_ASSERT_EQUAL_INT(6, mgmt->allocations); 
        TEST_ASSERT_EQUAL_INT(4, mgmt->deallocations);

        TEST_ASSERT_EQUAL_size_t(2, vec.size());
        const int expected_values[] {123, 456};
        const int actual_values[] {vec[0].val, vec[1].val};
        TEST_ASSERT_EQUAL_INT_ARRAY(expected_values, actual_values, 2);
    }

    // Rest of the vector deleted
    TEST_ASSERT_EQUAL_INT(6, mgmt->allocations); 
    TEST_ASSERT_EQUAL_INT(6, mgmt->deallocations);
}

void test_copy() {
    auto mgmt {std::make_shared<mem_mgmt>()}; 
    {
        cmagic::vector<object> vec;
        TEST_ASSERT_TRUE(vec);
        TEST_ASSERT_EQUAL_INT(0, mgmt->allocations);
        TEST_ASSERT_EQUAL_INT(0, mgmt->deallocations);

        TEST_ASSERT_TRUE(vec.push_back(object(123, mgmt)));
        // One temporary object, one stored in the vector
        TEST_ASSERT_EQUAL_INT(2, mgmt->allocations); 
        TEST_ASSERT_EQUAL_INT(1, mgmt->deallocations);

        TEST_ASSERT_TRUE(vec.push_back(object(456, mgmt)));
        // Similarly again
        TEST_ASSERT_EQUAL_INT(4, mgmt->allocations); 
        TEST_ASSERT_EQUAL_INT(2, mgmt->deallocations);

        cmagic::vector<object> vec_copy {vec};
        TEST_ASSERT_TRUE(vec);
        TEST_ASSERT_TRUE(vec_copy);
        // mgmt->allocations += 2 because we've copied 2 elements
        TEST_ASSERT_EQUAL_INT(6, mgmt->allocations); 
        TEST_ASSERT_EQUAL_INT(2, mgmt->deallocations);
        TEST_ASSERT_EQUAL_size_t(vec.size(), vec_copy.size());

        vec[0] = object(0xA, mgmt);
        vec[1] = object(0xB, mgmt);
        vec_copy[0] = object(0xC, mgmt);
        vec_copy[1] = object(0xD, mgmt);
        // mgmt->allocations += 4, mgmt->deallocations += 4 because we've used 4 temporary elements
        TEST_ASSERT_EQUAL_INT(10, mgmt->allocations); 
        TEST_ASSERT_EQUAL_INT(6, mgmt->deallocations);

        // Check if copies are independent
        TEST_ASSERT_EQUAL_INT(0xA, vec[0].val);
        TEST_ASSERT_EQUAL_INT(0xB, vec[1].val);
        TEST_ASSERT_EQUAL_INT(0xC, vec_copy[0].val);
        TEST_ASSERT_EQUAL_INT(0xD, vec_copy[1].val);
    }

    TEST_ASSERT_EQUAL_INT(10, mgmt->allocations); 
    TEST_ASSERT_EQUAL_INT(10, mgmt->deallocations);
}

void test_moving_semantics() {
    auto mgmt {std::make_shared<mem_mgmt>()}; 
    {
        cmagic::vector<object> vec;
        TEST_ASSERT_TRUE(vec);
        TEST_ASSERT_EQUAL_INT(0, mgmt->allocations);
        TEST_ASSERT_EQUAL_INT(0, mgmt->deallocations);

        TEST_ASSERT_TRUE(vec.push_back(object(123, mgmt)));
        // One temporary object, one stored in the vector
        TEST_ASSERT_EQUAL_INT(2, mgmt->allocations); 
        TEST_ASSERT_EQUAL_INT(1, mgmt->deallocations);

        TEST_ASSERT_TRUE(vec.push_back(object(456, mgmt)));
        // Similarly again
        TEST_ASSERT_EQUAL_INT(4, mgmt->allocations); 
        TEST_ASSERT_EQUAL_INT(2, mgmt->deallocations);

        cmagic::vector<object> vec_moved {std::move(vec)};
        TEST_ASSERT_TRUE(vec);
        TEST_ASSERT_TRUE(vec_moved);
        TEST_ASSERT_EQUAL_size_t(0, vec.size());
        TEST_ASSERT_EQUAL_size_t(2, vec_moved.size());
        TEST_ASSERT_EQUAL_INT(4, mgmt->allocations); 
        TEST_ASSERT_EQUAL_INT(2, mgmt->deallocations);
        
        TEST_ASSERT_EQUAL_INT(123, vec_moved[0].val);
        TEST_ASSERT_EQUAL_INT(456, vec_moved[1].val);
    }

    TEST_ASSERT_EQUAL_INT(4, mgmt->allocations); 
    TEST_ASSERT_EQUAL_INT(4, mgmt->deallocations);
}

void test_custom_alloc_vector() {
    uint8_t memory_pool[600]; 
    cmagic_memory_init(memory_pool, sizeof(memory_pool));
    {
        auto str_vector = cmagic::vector<std::string>::custom_allocation_vector();
        TEST_ASSERT_TRUE(str_vector);

        // One allocation for metadata, one for effective data
        TEST_ASSERT_EQUAL_size_t(2, cmagic_memory_get_allocations());

        constexpr const char* strings[] {
            "Lorem", "ipsum", "dolor", "sit", "amet,", "consectetur", "adipiscing", "elit,", "sed",
            "do", "eiusmod", "tempor", "incididunt", "ut", "labore", "et", "dolore", "magna",
            "aliqua.", "Ut", "enim", "ad", "minim", "veniam,", "quis", "nostrud", "exercitation",
            "ullamco", "laboris", "nisi", "ut", "aliquip", "ex", "ea", "commodo", "consequat.",
            "Duis", "aute", "irure", "dolor", "in", "reprehenderit", "in", "voluptate", "velit",
            "esse", "cillum", "dolore", "eu", "fugiat", "nulla", "pariatur.", "Excepteur", "sint",
            "occaecat", "cupidatat", "non", "proident,", "sunt", "in", "culpa", "qui", "officia",
            "deserunt", "mollit", "anim", "id", "est", "laborum."
        };

        for (const char *str : strings) {
            str_vector.push_back(str);
        }

        for (unsigned i = 0; i < str_vector.size(); i++) {
            TEST_ASSERT_EQUAL_STRING(strings[i], str_vector[i].c_str());
        }
    }

    TEST_ASSERT_EQUAL_size_t(0, cmagic_memory_get_allocations());
}

void test_emplace_back() {
    const char *cstr = "Hello World";
    cmagic::vector<std::string> vec;
    TEST_ASSERT_TRUE(vec);
    TEST_ASSERT_TRUE(vec.emplace_back(static_cast<size_t>(5), '*'));
    TEST_ASSERT_TRUE(vec.emplace_back(&cstr[0], &cstr[5]));
    TEST_ASSERT_TRUE(vec.emplace_back(static_cast<size_t>(10), '$'));

    TEST_ASSERT_EQUAL_size_t(3, vec.size());

    int iteration = 0;
    for (const auto &str : vec) {
        switch (iteration) {
        case 0:
            TEST_ASSERT_TRUE(str == "*****");
            break;
        case 1:
            TEST_ASSERT_TRUE(str == "Hello");
            break;
        case 2:
            TEST_ASSERT_TRUE(str == "$$$$$$$$$$");
            break;
        default:
            TEST_FAIL();
        }
        iteration++;
    }

}

void test_back_inserter() {
    std::vector<std::string> std_vec { "A", "B", "C" };
    cmagic::vector<std::string> c_vec;
    std::copy(std_vec.begin(), std_vec.end(), std::back_inserter(c_vec));
    TEST_ASSERT_EQUAL_size_t(3, c_vec.size());
    TEST_ASSERT_EQUAL_STRING("A", c_vec[0].c_str());
    TEST_ASSERT_EQUAL_STRING("B", c_vec[1].c_str());
    TEST_ASSERT_EQUAL_STRING("C", c_vec[2].c_str());
}

} // namespace

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_push_back_int);
    RUN_TEST(test_memory_management);
    RUN_TEST(test_copy);
    RUN_TEST(test_moving_semantics);
    RUN_TEST(test_custom_alloc_vector);
    RUN_TEST(test_emplace_back);
    RUN_TEST(test_back_inserter);
    return UNITY_END();
}
