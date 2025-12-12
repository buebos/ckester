#include "../../../../include/ckester.c"

CKESTER_SUITE(macros_test, CKESTER_FUNCTIONAL_UNIT) {
    CKESTER_TEST(test_1) {
        CKESTER_TRUE(1 == 1, "hello world %d did not equal %d", 1, 1);
    }
}
