#include "unity.h"
#include "clientlib.h"

static Args args;
static DynString line;

void setUp( void )
{
    dynstring_init( &args.authdstr, 16 );
    dynstring_init( &line, 50 );
}

void tearDown( void )
{
    dynstring_destroy( &line );
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
    fclose( server.rx );
    fclose( server.tx );
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

void test_negotiate_auth_success( void )
{
    bool res;
    dynstring_npush( &args.authdstr, "lemonade", 8 );
    ServerStreams server;
    server.rx = fopen( "test/testfiles/clientlib_negotiate_auth_success.txt", "r" );
    server.tx = fopen( "build/test_clientlib_negotiate_auth.out", "w+" );

    res = negotiate_auth( &server, args.authdstr.str, &line );
    TEST_ASSERT_TRUE( res );

    rewind( server.tx );
    dynstring_readline( &line, server.tx );
    TEST_ASSERT_EQUAL_STRING( "AUTH:lemonade", line.str );

    fclose( server.tx );
    fclose( server.rx );
}

void test_negotiate_auth_fail( void )
{
    bool res;
    dynstring_npush( &args.authdstr, "lemonade", 8 );
    ServerStreams server;
    server.rx = fopen( "test/testfiles/clientlib_negotiate_auth_fail.txt", "r" );
    server.tx = fopen( "build/test_clientlib_negotiate_auth.out", "w+" );

    res = negotiate_auth( &server, args.authdstr.str, &line );
    TEST_ASSERT_FALSE( res );

    rewind( server.tx );
    dynstring_readline( &line, server.tx );
    TEST_ASSERT_EQUAL_STRING( "AUTH:lemonade", line.str );

    fclose( server.tx );
    fclose( server.rx );
}

void test_negotiate_name_success( void )
{
    bool res;
    CLIENTNAME_INIT( name, "Joey" );
    ServerStreams server;
    server.rx = fopen( "test/testfiles/clientlib_negotiate_name_success.txt", "r" );
    server.tx = fopen( "build/test_clientlib_negotiate_name.out", "w+" );

    res = negotiate_name( &server, &name, &line );
    TEST_ASSERT_TRUE( res );

    rewind( server.tx );
    dynstring_readline( &line, server.tx );
    TEST_ASSERT_EQUAL_STRING( "NAME:Joey", line.str );
    dynstring_readline( &line, server.tx );
    TEST_ASSERT_EQUAL_STRING( "NAME:Joey0", line.str );
    dynstring_readline( &line, server.tx );
    TEST_ASSERT_EQUAL_STRING( "NAME:Joey1", line.str );

    fclose( server.tx );
    fclose( server.rx );
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
    RUN_TEST( test_negotiate_auth_success );
    RUN_TEST( test_negotiate_auth_fail );
    RUN_TEST( test_negotiate_name_success );
    return UNITY_END();
}
