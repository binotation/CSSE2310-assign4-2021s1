#include "unity.h"
#include "serverlib.h"

static Args args;

void setUp( void )
{
    dynstring_init( &args.authdstr, 17 );
}

void tearDown( void )
{
    dynstring_destroy( &args.authdstr );
}

void test_get_args2( void )
{
    char *argv[2] = { "server", "test/testfiles/auth.txt" };
    enum GetArgsResult res = get_args( &args, 2, argv );
    TEST_ASSERT_EQUAL( GET_ARGS_SUCCESS, res );
    TEST_ASSERT_EQUAL_STRING( "dcmkl 13niojhkdc juwnkxsaFG# j112df", args.authdstr.str );
    TEST_ASSERT_EQUAL_STRING( "0", args.port );
}

void test_get_args3( void )
{
    char *argv[3] = { "server", "test/testfiles/auth.txt", "28094" };
    enum GetArgsResult res = get_args( &args, 3, argv );
    TEST_ASSERT_EQUAL( GET_ARGS_SUCCESS, res );
    TEST_ASSERT_EQUAL_STRING( "dcmkl 13niojhkdc juwnkxsaFG# j112df", args.authdstr.str );
    TEST_ASSERT_EQUAL_STRING( "28094", args.port );
}

void test_get_args_authfile_not_found( void )
{
    char *argv[3] = { "server", "auth.txt", "46253" };
    enum GetArgsResult res = get_args( &args, 3, argv );
    TEST_ASSERT_EQUAL( GET_ARGS_AUTHFILE_NOT_FOUND, res );
}

void test_get_args_invalid_args_count( void )
{
    char *argv[1] = { "server" };
    enum GetArgsResult res = get_args( &args, 1, argv );
    TEST_ASSERT_EQUAL( GET_ARGS_INVALID_ARGS_COUNT, res );
}

int main( void )
{
    UNITY_BEGIN();

    RUN_TEST( test_get_args2 );
    RUN_TEST( test_get_args3 );
    RUN_TEST( test_get_args_authfile_not_found );
    RUN_TEST( test_get_args_invalid_args_count );

    return UNITY_END();
}
