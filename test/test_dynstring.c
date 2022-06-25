#include "unity.h"
#include "dynstring.h"

static DynString dstr;
static const char *sentence = "They looked up at the sky and saw a million stars";

void setUp( void )
{
    dynstring_init( &dstr, 10 );
}

void tearDown( void )
{
    dynstring_destroy( &dstr );
    TEST_ASSERT_EQUAL( 0, dstr.size );
}

void test_dynstring_init( void )
{
    TEST_ASSERT_EQUAL( '\0', dstr.str[0] );
    TEST_ASSERT_EQUAL( 10, dstr.size );
    TEST_ASSERT_EQUAL( 0, dstr.length );
}

void test_dynstring_nfrom( void )
{
    const char *name = "Bentley";
    DynString dname;
    dynstring_nfrom( &dname, name, 7, 8 );
    TEST_ASSERT_EQUAL_STRING( name, dname.str );
    TEST_ASSERT_EQUAL( 7, dname.length );
    TEST_ASSERT_EQUAL( 8, dname.size );
    dynstring_destroy( &dname );
}

void test_dynstring_npush_empty( void )
{
    DynString sentence;
    dynstring_nfrom( &sentence, "My favourite color is", 21, 23 );
    TEST_ASSERT_EQUAL_STRING( "My favourite color is", sentence.str );
    TEST_ASSERT_EQUAL( 21, sentence.length );
    TEST_ASSERT_EQUAL( 23, sentence.size );

    dynstring_npush( &sentence, "", 0 );
    TEST_ASSERT_EQUAL_STRING( "My favourite color is", sentence.str );
    TEST_ASSERT_EQUAL( 21, sentence.length );
    TEST_ASSERT_EQUAL( 23, sentence.size );
    dynstring_destroy( &sentence );
}

void test_dynstring_npush0( void )
{
    dynstring_npush( &dstr, sentence, 9 );
    TEST_ASSERT_EQUAL_STRING( "They look", dstr.str );
    TEST_ASSERT_EQUAL( 9, dstr.length );
    TEST_ASSERT_EQUAL( 10, dstr.size );

    dynstring_npush( &dstr, sentence + 9, 33 );
    TEST_ASSERT_EQUAL_STRING( "They looked up at the sky and saw a millio", dstr.str );
    TEST_ASSERT_EQUAL( 42, dstr.length );
    TEST_ASSERT_EQUAL( 80, dstr.size );

    dynstring_npush( &dstr, sentence + 42, 7 );
    TEST_ASSERT_EQUAL_STRING( "They looked up at the sky and saw a million stars", dstr.str );
    TEST_ASSERT_EQUAL( 49, dstr.length );
    TEST_ASSERT_EQUAL( 80, dstr.size );
}

void test_dynstring_npush1( void )
{
    dynstring_npush( &dstr, sentence, 5 );
    TEST_ASSERT_EQUAL_STRING( "They ", dstr.str );
    TEST_ASSERT_EQUAL( 5, dstr.length );
    TEST_ASSERT_EQUAL( 10, dstr.size );

    dynstring_npush( &dstr, sentence + 5, 34 );
    TEST_ASSERT_EQUAL_STRING( "They looked up at the sky and saw a mil", dstr.str );
    TEST_ASSERT_EQUAL( 39, dstr.length );
    TEST_ASSERT_EQUAL( 40, dstr.size );

    dynstring_npush( &dstr, sentence + 39, 10 );
    TEST_ASSERT_EQUAL_STRING( "They looked up at the sky and saw a million stars", dstr.str );
    TEST_ASSERT_EQUAL( 49, dstr.length );
    TEST_ASSERT_EQUAL( 80, dstr.size );
}

void test_dynstring_npush2( void )
{
    dynstring_npush( &dstr, sentence, 5 );
    TEST_ASSERT_EQUAL_STRING( "They ", dstr.str );
    TEST_ASSERT_EQUAL( 5, dstr.length );
    TEST_ASSERT_EQUAL( 10, dstr.size );

    dynstring_npush( &dstr, sentence + 5, 35 );
    TEST_ASSERT_EQUAL_STRING( "They looked up at the sky and saw a mill", dstr.str );
    TEST_ASSERT_EQUAL( 40, dstr.length );
    TEST_ASSERT_EQUAL( 80, dstr.size );

    dynstring_npush( &dstr, sentence + 40, 9 );
    TEST_ASSERT_EQUAL_STRING( "They looked up at the sky and saw a million stars", dstr.str );
    TEST_ASSERT_EQUAL( 49, dstr.length );
    TEST_ASSERT_EQUAL( 80, dstr.size );
}

void test_dynstring_pushc( void )
{
    dynstring_npush( &dstr, "It's no", 7 );
    dynstring_pushc( &dstr, 't' );
    TEST_ASSERT_EQUAL_STRING( "It's not", dstr.str );
    TEST_ASSERT_EQUAL( 8, dstr.length );
    TEST_ASSERT_EQUAL( 10, dstr.size );

    dynstring_pushc( &dstr, ' ' );
    TEST_ASSERT_EQUAL_STRING( "It's not ", dstr.str );
    TEST_ASSERT_EQUAL( 9, dstr.length );
    TEST_ASSERT_EQUAL( 10, dstr.size );

    dynstring_pushc( &dstr, 'o' );
    TEST_ASSERT_EQUAL_STRING( "It's not o", dstr.str );
    TEST_ASSERT_EQUAL( 10, dstr.length );
    TEST_ASSERT_EQUAL( 20, dstr.size );

    dynstring_pushc( &dstr, 'f' );
    TEST_ASSERT_EQUAL_STRING( "It's not of", dstr.str );
    TEST_ASSERT_EQUAL( 11, dstr.length );
    TEST_ASSERT_EQUAL( 20, dstr.size );
}

void test_dynstring_popc( void )
{
    char c;
    dynstring_npush( &dstr, "It", 2 );
    c = dynstring_popc( &dstr );
    TEST_ASSERT_EQUAL( 't', c );
    TEST_ASSERT_EQUAL_STRING( "I", dstr.str );
    TEST_ASSERT_EQUAL( 1, dstr.length );
    TEST_ASSERT_EQUAL( 10, dstr.size );

    c = dynstring_popc( &dstr );
    TEST_ASSERT_EQUAL( 'I', c );
    TEST_ASSERT_EQUAL_STRING( "", dstr.str );
    TEST_ASSERT_EQUAL( 0, dstr.length );
    TEST_ASSERT_EQUAL( 10, dstr.size );

    c = dynstring_popc( &dstr );
    TEST_ASSERT_EQUAL( '\0', c );
    TEST_ASSERT_EQUAL_STRING( "", dstr.str );
    TEST_ASSERT_EQUAL( 0, dstr.length );
    TEST_ASSERT_EQUAL( 10, dstr.size );
}

// Test with EOF being read instantly
void test_dynstring_readline_empty( void )
{
    FILE *input = fopen( "test/testfiles/empty.txt", "r" );
    enum ReadlineResult res = dynstring_readline( &dstr, input );
    TEST_ASSERT_EQUAL( READLINE_EOF, res );
    TEST_ASSERT_EQUAL( '\0', dstr.str[0] );
    TEST_ASSERT_EQUAL( 0, dstr.length );
    TEST_ASSERT_EQUAL( 10, dstr.size );
    fclose( input );
}

// Test read without resize
void test_dynstring_readline_no_resize( void )
{
    FILE *input = fopen( "test/testfiles/dynstring_readline_no_resize.txt", "r" );
    enum ReadlineResult res = dynstring_readline( &dstr, input );
    TEST_ASSERT_EQUAL( READLINE_SUCCESS, res );
    TEST_ASSERT_EQUAL_STRING( "Nobody", dstr.str );
    TEST_ASSERT_EQUAL( 6, dstr.length );
    TEST_ASSERT_EQUAL( 10, dstr.size );
    fclose( input );
}

// Test read with newline appearing in the filled buffer, i.e. '\n' at column 9
void test_dynstring_readline_no_resize_newline( void )
{
    FILE *input = fopen( "test/testfiles/dynstring_readline_no_resize_newline.txt", "r" );
    enum ReadlineResult res = dynstring_readline( &dstr, input );
    TEST_ASSERT_EQUAL( READLINE_SUCCESS, res );
    TEST_ASSERT_EQUAL_STRING( "question", dstr.str );
    TEST_ASSERT_EQUAL( 8, dstr.length );
    TEST_ASSERT_EQUAL( 10, dstr.size );
    fclose( input );
}

// Test with 1 resize
void test_dynstring_readline_resize1( void )
{
    FILE *input = fopen( "test/testfiles/dynstring_readline_resize1.txt", "r" );
    enum ReadlineResult res = dynstring_readline( &dstr, input );
    TEST_ASSERT_EQUAL( READLINE_SUCCESS, res );
    TEST_ASSERT_EQUAL_STRING( "He loved her then", dstr.str );
    TEST_ASSERT_EQUAL( 17, dstr.length );
    TEST_ASSERT_EQUAL( 20, dstr.size );
    fclose( input );
}

// Test with 1 resize and a newline occurring in the filled buffer
void test_dynstring_readline_resize1_newline( void )
{
    FILE *input = fopen( "test/testfiles/dynstring_readline_resize1_newline.txt", "r" );
    enum ReadlineResult res = dynstring_readline( &dstr, input );
    TEST_ASSERT_EQUAL( READLINE_SUCCESS, res );
    TEST_ASSERT_EQUAL_STRING( "He loved her then.", dstr.str );
    TEST_ASSERT_EQUAL( 18, dstr.length );
    TEST_ASSERT_EQUAL( 20, dstr.size );
    fclose( input );
}

// Test with 2 resizes, with only newline being read after second resize.
void test_dynstring_readline_resize2_newline( void )
{
    FILE *input = fopen( "test/testfiles/dynstring_readline_resize2_newline.txt", "r" );
    enum ReadlineResult res = dynstring_readline( &dstr, input );
    TEST_ASSERT_EQUAL( READLINE_SUCCESS, res );
    TEST_ASSERT_EQUAL_STRING( "She loved him then.", dstr.str );
    TEST_ASSERT_EQUAL( 19, dstr.length );
    TEST_ASSERT_EQUAL( 40, dstr.size );
    fclose( input );
}

// Test with 2 resizes with no newline and EOF read instantly after second resize
void test_dynstring_readline_resize2_eof1( void )
{
    FILE *input = fopen( "test/testfiles/dynstring_readline_resize2_eof1.txt", "r" );
    enum ReadlineResult res = dynstring_readline( &dstr, input );
    TEST_ASSERT_EQUAL( READLINE_SUCCESS, res );
    TEST_ASSERT_EQUAL_STRING( "She loved him then.", dstr.str );
    TEST_ASSERT_EQUAL( 19, dstr.length );
    TEST_ASSERT_EQUAL( 40, dstr.size );
    res = dynstring_readline( &dstr, input );
    TEST_ASSERT_EQUAL( READLINE_EOF, res );
    TEST_ASSERT_EQUAL( '\0', dstr.str[0] );
    TEST_ASSERT_EQUAL( 0, dstr.length );
    TEST_ASSERT_EQUAL( 40, dstr.size );
    fclose( input );
}

// Test with 2 resizes with no newline
void test_dynstring_readline_resize2_eof2( void )
{
    FILE *input = fopen( "test/testfiles/dynstring_readline_resize2_eof2.txt", "r" );
    enum ReadlineResult res = dynstring_readline( &dstr, input );
    TEST_ASSERT_EQUAL( READLINE_SUCCESS, res );
    TEST_ASSERT_EQUAL_STRING( "It's Christmas tommorrow.", dstr.str );
    TEST_ASSERT_EQUAL( 25, dstr.length );
    TEST_ASSERT_EQUAL( 40, dstr.size );
    res = dynstring_readline( &dstr, input );
    TEST_ASSERT_EQUAL( READLINE_EOF, res );
    TEST_ASSERT_EQUAL( '\0', dstr.str[0] );
    TEST_ASSERT_EQUAL( 0, dstr.length );
    TEST_ASSERT_EQUAL( 40, dstr.size );
    fclose( input );
}

// Test with 3 resizes
void test_dynstring_readline_resize3( void )
{
    FILE *input = fopen( "test/testfiles/dynstring_readline_resize3.txt", "r" );
    enum ReadlineResult res = dynstring_readline( &dstr, input );
    TEST_ASSERT_EQUAL( READLINE_SUCCESS, res );
    TEST_ASSERT_EQUAL_STRING( "As he dangled from the rope deep inside the crevasse.", dstr.str );
    TEST_ASSERT_EQUAL( 53, dstr.length );
    TEST_ASSERT_EQUAL( 80, dstr.size );
    fclose( input );
}

void test_dynstring_readline_multi( void )
{
    enum ReadlineResult res;
    FILE *input = fopen( "test/testfiles/dynstring_readline_multi.txt", "r" );

    res = dynstring_readline( &dstr, input );
    TEST_ASSERT_EQUAL( READLINE_SUCCESS, res );
    TEST_ASSERT_EQUAL_STRING( "rusted", dstr.str );
    TEST_ASSERT_EQUAL( 6, dstr.length );
    TEST_ASSERT_EQUAL( 10, dstr.size );

    res = dynstring_readline( &dstr, input );
    TEST_ASSERT_EQUAL( READLINE_SUCCESS, res );
    TEST_ASSERT_EQUAL_STRING( "This is a library.", dstr.str );
    TEST_ASSERT_EQUAL( 18, dstr.length );
    TEST_ASSERT_EQUAL( 20, dstr.size );

    res = dynstring_readline( &dstr, input );
    TEST_ASSERT_EQUAL( READLINE_SUCCESS, res );
    TEST_ASSERT_EQUAL_STRING( "Today is the day I'll finally know what brick tastes like", dstr.str );
    TEST_ASSERT_EQUAL( 57, dstr.length );
    TEST_ASSERT_EQUAL( 80, dstr.size );

    res = dynstring_readline( &dstr, input );
    TEST_ASSERT_EQUAL( READLINE_SUCCESS, res );
    TEST_ASSERT_EQUAL_STRING( "She lived on Monkey Jungle Road and that seemed to explain all her strangeness.", dstr.str );
    TEST_ASSERT_EQUAL( 79, dstr.length );
    TEST_ASSERT_EQUAL( 160, dstr.size );

    res = dynstring_readline( &dstr, input );
    TEST_ASSERT_EQUAL( READLINE_SUCCESS, res );
    TEST_ASSERT_EQUAL_STRING( "He decided water-skiing on a frozen lake wasn't a good idea.", dstr.str );
    TEST_ASSERT_EQUAL( 60, dstr.length );
    TEST_ASSERT_EQUAL( 160, dstr.size );

    res = dynstring_readline( &dstr, input );
    TEST_ASSERT_EQUAL( READLINE_SUCCESS, res );
    TEST_ASSERT_EQUAL_STRING( "If you spin around three times, you'll start to feel melancholy.", dstr.str );
    TEST_ASSERT_EQUAL( 64, dstr.length );
    TEST_ASSERT_EQUAL( 160, dstr.size );

    res = dynstring_readline( &dstr, input );
    TEST_ASSERT_EQUAL( READLINE_EOF, res );
    TEST_ASSERT_EQUAL( '\0', dstr.str[0] );
    TEST_ASSERT_EQUAL( 0, dstr.length );
    TEST_ASSERT_EQUAL( 160, dstr.size );

    fclose( input );
}

int main( void )
{
    UNITY_BEGIN();
    RUN_TEST( test_dynstring_init );
    RUN_TEST( test_dynstring_nfrom );
    RUN_TEST( test_dynstring_npush_empty );
    RUN_TEST( test_dynstring_npush0 );
    RUN_TEST( test_dynstring_npush1 );
    RUN_TEST( test_dynstring_npush2 );
    RUN_TEST( test_dynstring_pushc );
    RUN_TEST( test_dynstring_popc );
    RUN_TEST( test_dynstring_readline_empty );
    RUN_TEST( test_dynstring_readline_no_resize );
    RUN_TEST( test_dynstring_readline_no_resize_newline );
    RUN_TEST( test_dynstring_readline_resize1 );
    RUN_TEST( test_dynstring_readline_resize1_newline );
    RUN_TEST( test_dynstring_readline_resize2_newline );
    RUN_TEST( test_dynstring_readline_resize2_eof1 );
    RUN_TEST( test_dynstring_readline_resize2_eof2 );
    RUN_TEST( test_dynstring_readline_resize3 );
    RUN_TEST( test_dynstring_readline_multi );
    return UNITY_END();
}
