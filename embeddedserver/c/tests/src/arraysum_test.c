#include "unity.h"
#include "arraysum.h"

void test_sum_empty(void) {
    int arr[] = {};
    int size = sizeof(arr) / sizeof(int);
    TEST_ASSERT_EQUAL_INT(0, sum(arr, size));
}

void test_sum_negative_size(void) {
    int arr[] = {1, 2, 3};
    int size = -1;
    TEST_ASSERT_EQUAL_INT(0, sum(arr, size));
}

void test_sum_array_size1(void) {
    int arr[] = {42};
    int size = sizeof(arr) / sizeof(int);
    TEST_ASSERT_EQUAL_INT(42, sum(arr, size));
}

void test_sum_array_size10(void) {
    int arr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int size = sizeof(arr) / sizeof(int);
    TEST_ASSERT_EQUAL_INT(55, sum(arr, size));
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_sum_empty);
    RUN_TEST(test_sum_negative_size);
    RUN_TEST(test_sum_array_size1);
    RUN_TEST(test_sum_array_size10);
    return UNITY_END();
}
