#include "unity.h"
#include "dynstring.h"

static DynString str;

void setUp( void )
{
    dynstring_init( &str, 10 );
}

void tearDown( void )
{
    dynstring_destroy( &str );
    TEST_ASSERT_EQUAL( 0, str.size );
}

void test_dynstring_init( void )
{
    TEST_ASSERT_EQUAL( '\0', str.chars[0] );
    TEST_ASSERT_EQUAL( 10, str.size );
    TEST_ASSERT_EQUAL( 0, str.length );
}

void test_dynstring_readline_resize3( void )
{
    FILE *input = fopen( "test/testfiles/dynstring_readline_resize3.txt", "r" );
    enum ReadlineResult res = dynstring_readline( &str, input );
    TEST_ASSERT_EQUAL( SUCCESS, res );
    TEST_ASSERT_EQUAL_STRING( "As he dangled from the rope deep inside the crevasse.", str.chars );
    TEST_ASSERT_EQUAL( 53, str.length );
    TEST_ASSERT_EQUAL( 80, str.size );
    fclose( input );
}

int main( void )
{
    UNITY_BEGIN();
    RUN_TEST( test_dynstring_init );
    RUN_TEST( test_dynstring_readline_resize3 );
    return UNITY_END();
}
