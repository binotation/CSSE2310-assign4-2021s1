#include "unity.h"
#include "util.h"

void setUp( void )
{
}

void tearDown( void )
{
}

void test_get_two_args0( void )
{
    TwoArgs targs;
    bool res;
    res = get_two_args( "MSG:Abdullahi:She is never happy until she finds something", &targs );
    TEST_ASSERT_TRUE( res );

    TEST_ASSERT_EQUAL_STRING_LEN( "Abdullahi", targs.arg1.str, 9);
    TEST_ASSERT_EQUAL( 9, targs.arg1.length );

    TEST_ASSERT_EQUAL_STRING_LEN( "She is never happy until she finds something", targs.arg2.str, 44 );
    TEST_ASSERT_EQUAL( 44, targs.arg2.length );
}

void test_get_two_args1( void )
{
    TwoArgs targs;
    bool res;
    res = get_two_args( "MSG::She is never happy until she finds something", &targs );
    TEST_ASSERT_FALSE( res );
    res = get_two_args( "MSG:Abdullahi:", &targs );
    TEST_ASSERT_FALSE( res );
    res = get_two_args( "MSGAbdullahi::", &targs );
    TEST_ASSERT_FALSE( res );
    res = get_two_args( "MSG:Abdullahi", &targs );
    TEST_ASSERT_FALSE( res );
    res = get_two_args( "MSGAbdullahi:", &targs );
    TEST_ASSERT_FALSE( res );
    res = get_two_args( "MSGAbdullahi", &targs );
    TEST_ASSERT_FALSE( res );
    res = get_two_args( "::", &targs );
    TEST_ASSERT_FALSE( res );
    res = get_two_args( "", &targs );
    TEST_ASSERT_FALSE( res );
}

void test_replace_unprintable( void )
{
    char str[13] = { 69, 12, 39, 47, 38, 89, 97, 60, 85, 57, 25, 82, 0 };
    replace_unprintable( str );
    TEST_ASSERT_EQUAL_STRING( "E?'/&Ya<U9?R", str );
}

int main( void )
{
    UNITY_BEGIN();
    RUN_TEST( test_get_two_args0 );
    RUN_TEST( test_get_two_args1 );
    RUN_TEST( test_replace_unprintable );
    return UNITY_END();
}
