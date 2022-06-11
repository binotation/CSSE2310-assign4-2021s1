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

// Test with 1 resize and a newline occurring in the maxed buffer
void test_dynstring_readline_resize1( void )
{
    FILE *input = fopen( "test/testfiles/dynstring_readline_resize1.txt", "r" );
    enum ReadlineResult res = dynstring_readline( &str, input );
    TEST_ASSERT_EQUAL( SUCCESS, res );
    TEST_ASSERT_EQUAL_STRING( "He loved her then.", str.chars );
    TEST_ASSERT_EQUAL( 18, str.length );
    TEST_ASSERT_EQUAL( 20, str.size );
    fclose( input );
}

// Test with 2 resizes, with only newline being read after second resize.
void test_dynstring_readline_resize2_newline( void )
{
    FILE *input = fopen( "test/testfiles/dynstring_readline_resize2_newline.txt", "r" );
    enum ReadlineResult res = dynstring_readline( &str, input );
    TEST_ASSERT_EQUAL( SUCCESS, res );
    TEST_ASSERT_EQUAL_STRING( "She loved him then.", str.chars );
    TEST_ASSERT_EQUAL( 19, str.length );
    TEST_ASSERT_EQUAL( 40, str.size );
    fclose( input );
}

// Test with 2 resizes with no newline
void test_dynstring_readline_resize2_eof( void )
{
    FILE *input = fopen( "test/testfiles/dynstring_readline_resize2_eof.txt", "r" );
    enum ReadlineResult res = dynstring_readline( &str, input );
    TEST_ASSERT_EQUAL( EOF_REACHED, res );
    TEST_ASSERT_EQUAL_STRING( "She loved him then.", str.chars );
    TEST_ASSERT_EQUAL( 19, str.length );
    TEST_ASSERT_EQUAL( 40, str.size );
    res = dynstring_readline( &str, input );
    TEST_ASSERT_EQUAL( EOF_REACHED, res );
    TEST_ASSERT_EQUAL( '\0', str.chars[0] );
    TEST_ASSERT_EQUAL( 0, str.length );
    TEST_ASSERT_EQUAL( 40, str.size );
    fclose( input );
}

// Test with 3 resizes
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
    RUN_TEST( test_dynstring_readline_resize1 );
    RUN_TEST( test_dynstring_readline_resize2_newline );
    RUN_TEST( test_dynstring_readline_resize2_eof );
    RUN_TEST( test_dynstring_readline_resize3 );
    return UNITY_END();
}
