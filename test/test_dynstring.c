#include "unity.h"
#include "dynstring.h"

static DynString str;

void setUp(void)
{
    dynstring_init(&str, 10);
}

void tearDown(void)
{
    dynstring_destroy(&str);
}

void test_dynstring_init_destroy(void)
{
    DynString str1;

    dynstring_init(&str1, 34);
    TEST_ASSERT_EQUAL(34, str1.size);
    TEST_ASSERT_EQUAL(0, str1.length);
    dynstring_destroy(&str1);
    TEST_ASSERT_EQUAL(0, str1.size);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_dynstring_init_destroy);
    return UNITY_END();
}
