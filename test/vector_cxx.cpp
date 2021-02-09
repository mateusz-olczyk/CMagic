#include <memory>
#include "cmagic/vector.hpp"
#include "unity.h"

void setUp() {}

void tearDown() {}

namespace {

static void test_push_back_int() {
    cmagic::vector<int> vec;
    TEST_ASSERT_TRUE(vec.push_back(1));
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

    object() = delete;
    object &operator=(const object &) = default;
    object(object &&) = delete;
    object &operator=(object &&) = delete;

    object(int val, std::shared_ptr<mem_mgmt> owner) : val(val), owner(owner) {
        owner->allocations++;
    }

    object(const object &x) : val(x.val), owner(x.owner) {
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

} // namespace

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_push_back_int);
    RUN_TEST(test_memory_management);
    return UNITY_END();
}
