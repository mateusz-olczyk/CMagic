#include "cmagic/utils.h"
#include "unity.h"


void setUp(void) {}

void tearDown(void) {}

static void test_Utils(void) {
    TEST_ASSERT_EQUAL_INT(23, CMAGIC_UTILS_DIV_CEIL(1234, 56));

    const uint32_t base_address = 123;
    const size_t alignment = 16;
    const uint32_t expected_aligned_down = 112;
    const uint32_t expected_aligned_up = 128;
    TEST_ASSERT_EQUAL_UINT32(base_address, (uint32_t)(uintptr_t)base_address);
    TEST_ASSERT_EQUAL_UINT32(expected_aligned_down, (uint32_t)(uintptr_t)expected_aligned_down);
    TEST_ASSERT_EQUAL_UINT32(expected_aligned_up, (uint32_t)(uintptr_t)expected_aligned_up);
    TEST_ASSERT_EQUAL_UINT32(expected_aligned_down, (uint32_t)cmagic_utils_align_address_down((uintptr_t)base_address, alignment));
    TEST_ASSERT_EQUAL_UINT32(expected_aligned_up, (uint32_t)cmagic_utils_align_address_up((uintptr_t)base_address, alignment));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_Utils);
    return UNITY_END();
}
