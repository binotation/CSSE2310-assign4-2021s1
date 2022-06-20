#include "unity.h"
#include "clientlib.h"

static Args args;

void setUp( void )
{
    dynstring_init( &args.authdstr, 16 );
}

void tearDown( void )
{
    dynstring_destroy( &args.authdstr );
}

void test_get_args4( void )
{
    char *argv[4] = { "client", "Austin", "test/testfiles/auth.txt", "20097" };
    enum GetArgsResult res = get_args( &args, 4, argv );
    TEST_ASSERT_EQUAL( GET_ARGS_SUCCESS, res );
    TEST_ASSERT_EQUAL_STRING( "Austin", args.chosen_name );
    TEST_ASSERT_EQUAL_STRING( "dcmkl 13niojhkdc juwnkxsaFG# j112df", args.authdstr.str );
    TEST_ASSERT_EQUAL_STRING( "localhost", args.host );
    TEST_ASSERT_EQUAL_STRING( "20097", args.port );
}

void test_get_args5( void )
{
    char *argv[5] = { "client", "Patryk", "test/testfiles/auth.txt", "127.0.0.1", "6952" };
    enum GetArgsResult res = get_args( &args, 5, argv );
    TEST_ASSERT_EQUAL( GET_ARGS_SUCCESS, res );
    TEST_ASSERT_EQUAL_STRING( "Patryk", args.chosen_name );
    TEST_ASSERT_EQUAL_STRING( "dcmkl 13niojhkdc juwnkxsaFG# j112df", args.authdstr.str );
    TEST_ASSERT_EQUAL_STRING( "127.0.0.1", args.host );
    TEST_ASSERT_EQUAL_STRING( "6952", args.port );
}

void test_get_args_authfile_not_found( void )
{
    char *argv[5] = { "client", "Patryk", "auth.txt", "192.168.1.222", "46253" };
    enum GetArgsResult res = get_args( &args, 5, argv );
    TEST_ASSERT_EQUAL( GET_ARGS_AUTHFILE_NOT_FOUND, res );
}

void test_get_args_invalid_args_count( void )
{
    char *argv[1] = { "client" };
    enum GetArgsResult res = get_args( &args, 1, argv );
    TEST_ASSERT_EQUAL( GET_ARGS_INVALID_ARGS_COUNT, res );
}

void test_get_connection( void )
{
    ServerStreams server;
    enum GetConnResult res = get_connection( "127.0.0.1", "19224", &server );
    TEST_ASSERT_EQUAL( GET_CONN_SUCCESS, res );
    fclose( server.read );
    fclose( server.write );
}

// Reachable blocks caused by getaddrinfo()
// https://stackoverflow.com/questions/13229913/getaddrinfo-memory-leak/
void test_get_connection_invalid_host( void )
{
    TEST_IGNORE();
    enum GetConnResult res = get_connection( "jdkcj3eo", "6660", 0 );
    TEST_ASSERT_EQUAL( GET_CONN_HOST_INVALID, res );
}

void test_get_connection_bad_host( void )
{
    enum GetConnResult res = get_connection( "localhost", "17086", 0 );
    TEST_ASSERT_EQUAL( GET_CONN_COMM_ERR, res );
}

int main( void )
{
    UNITY_BEGIN();
    RUN_TEST( test_get_args4 );
    RUN_TEST( test_get_args5 );
    RUN_TEST( test_get_args_authfile_not_found );
    RUN_TEST( test_get_args_invalid_args_count );
    RUN_TEST( test_get_connection );
    RUN_TEST( test_get_connection_invalid_host );
    RUN_TEST( test_get_connection_bad_host );
    return UNITY_END();
}
