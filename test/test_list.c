#include "unity.h"
#include "list.h"
#include "dynstring.h"

#define INSERT_DUMMY()										\
{															\
    pthread_mutex_t lock0, lock1, lock2, lock3;				\
    list_insert( &list, &names[1], (FILE*)1, &lock0 );		\
    list_insert( &list, &names[0], (FILE*)2, &lock1 );		\
    list_insert( &list, &names[2], (FILE*)3, &lock2 );		\
    list_insert( &list, &names[3], (FILE*)4, &lock3 );		\
}

// Test permutation of order of inserting names.
#define TEST_INSERT( name0, name1, name2, name3 )											\
{																							\
    ListNode *node;																			\
    pthread_mutex_t lock0, lock1, lock2, lock3;												\
    node = list_insert( &list, &names[name0], (FILE*)1, &lock0 );							\
    TEST_ASSERT_EQUAL_STRING( names[name0].str, node->data.name->str );						\
    node = list_insert( &list, &names[name1], (FILE*)2, &lock1 );							\
    TEST_ASSERT_EQUAL_STRING( names[name1].str, node->data.name->str );						\
    node = list_insert( &list, &names[name2], (FILE*)3, &lock2 );							\
    TEST_ASSERT_EQUAL_STRING( names[name2].str, node->data.name->str );						\
    node = list_insert( &list, &names[name3], (FILE*)4, &lock3 );							\
    TEST_ASSERT_EQUAL_STRING( names[name3].str, node->data.name->str );						\
																							\
    TEST_ASSERT_EQUAL_STRING( names[0].str, list.head->data.name->str );					\
    TEST_ASSERT_EQUAL_STRING( names[1].str, list.head->next->data.name->str );				\
    TEST_ASSERT_EQUAL_STRING( names[2].str, list.head->next->next->data.name->str );		\
    TEST_ASSERT_EQUAL_STRING( names[3].str, list.head->next->next->next->data.name->str );	\
}

// Test permutation of order of deleting names.
#define TEST_DELETE( name0, name1, name2, name3, ord11, ord12, ord13, ord21, ord22, ord31 )	\
{																							\
    INSERT_DUMMY()																			\
																							\
    list_delete( &list, names[name0].str );													\
    TEST_ASSERT_EQUAL_STRING( names[ord11].str, list.head->data.name->str );				\
    TEST_ASSERT_EQUAL_STRING( names[ord12].str, list.head->next->data.name->str );			\
    TEST_ASSERT_EQUAL_STRING( names[ord13].str, list.head->next->next->data.name->str );	\
																							\
    list_delete( &list, names[name1].str );													\
    TEST_ASSERT_EQUAL_STRING( names[ord21].str, list.head->data.name->str );				\
    TEST_ASSERT_EQUAL_STRING( names[ord22].str, list.head->next->data.name->str );			\
																							\
    list_delete( &list, names[name2].str );													\
    TEST_ASSERT_EQUAL_STRING( names[ord31].str, list.head->data.name->str );				\
																							\
    list_delete( &list, names[name3].str );													\
    TEST_ASSERT_EQUAL( 0, list.head );														\
}

static ClientList list;
static DynString names[4];

void setUp( void )
{
    list_init( &list );
}

void tearDown( void )
{
    list_destroy( &list );
}

void test_init( void )
{
    TEST_ASSERT_EQUAL( 0, list.head );
}

// Test every permutation of inserting 4 names.
void test_insert0 ( void ) { TEST_INSERT( 0, 1, 2, 3 ) }
void test_insert1 ( void ) { TEST_INSERT( 0, 1, 3, 2 ) }
void test_insert2 ( void ) { TEST_INSERT( 0, 2, 1, 3 ) }
void test_insert3 ( void ) { TEST_INSERT( 0, 2, 3, 1 ) }
void test_insert4 ( void ) { TEST_INSERT( 0, 3, 1, 2 ) }
void test_insert5 ( void ) { TEST_INSERT( 0, 3, 2, 1 ) }
void test_insert6 ( void ) { TEST_INSERT( 1, 0, 2, 3 ) }
void test_insert7 ( void ) { TEST_INSERT( 1, 0, 3, 2 ) }
void test_insert8 ( void ) { TEST_INSERT( 1, 2, 0, 3 ) }
void test_insert9 ( void ) { TEST_INSERT( 1, 2, 3, 0 ) }
void test_insert10( void ) { TEST_INSERT( 1, 3, 0, 2 ) }
void test_insert11( void ) { TEST_INSERT( 1, 3, 2, 0 ) }
void test_insert12( void ) { TEST_INSERT( 2, 0, 1, 3 ) }
void test_insert13( void ) { TEST_INSERT( 2, 0, 3, 1 ) }
void test_insert14( void ) { TEST_INSERT( 2, 1, 0, 3 ) }
void test_insert15( void ) { TEST_INSERT( 2, 1, 3, 0 ) }
void test_insert16( void ) { TEST_INSERT( 2, 3, 0, 1 ) }
void test_insert17( void ) { TEST_INSERT( 2, 3, 1, 0 ) }
void test_insert18( void ) { TEST_INSERT( 3, 0, 1, 2 ) }
void test_insert19( void ) { TEST_INSERT( 3, 0, 2, 1 ) }
void test_insert20( void ) { TEST_INSERT( 3, 1, 0, 2 ) }
void test_insert21( void ) { TEST_INSERT( 3, 1, 2, 0 ) }
void test_insert22( void ) { TEST_INSERT( 3, 2, 0, 1 ) }
void test_insert23( void ) { TEST_INSERT( 3, 2, 1, 0 ) }

void test_delete_empty( void )
{
    list_delete( &list, names[1].str );
    TEST_ASSERT_EQUAL( 0, list.head );
}
// Test every permutation of deleting 4 names.
void test_delete0 ( void ) { TEST_DELETE( 0, 1, 2, 3, 1, 2, 3, 2, 3, 3 ) }
void test_delete1 ( void ) { TEST_DELETE( 0, 1, 3, 2, 1, 2, 3, 2, 3, 2 ) }
void test_delete2 ( void ) { TEST_DELETE( 0, 2, 1, 3, 1, 2, 3, 1, 3, 3 ) }
void test_delete3 ( void ) { TEST_DELETE( 0, 2, 3, 1, 1, 2, 3, 1, 3, 1 ) }
void test_delete4 ( void ) { TEST_DELETE( 0, 3, 1, 2, 1, 2, 3, 1, 2, 2 ) }
void test_delete5 ( void ) { TEST_DELETE( 0, 3, 2, 1, 1, 2, 3, 1, 2, 1 ) }
void test_delete6 ( void ) { TEST_DELETE( 1, 0, 2, 3, 0, 2, 3, 2, 3, 3 ) }
void test_delete7 ( void ) { TEST_DELETE( 1, 0, 3, 2, 0, 2, 3, 2, 3, 2 ) }
void test_delete8 ( void ) { TEST_DELETE( 1, 2, 0, 3, 0, 2, 3, 0, 3, 3 ) }
void test_delete9 ( void ) { TEST_DELETE( 1, 2, 3, 0, 0, 2, 3, 0, 3, 0 ) }
void test_delete10( void ) { TEST_DELETE( 1, 3, 0, 2, 0, 2, 3, 0, 2, 2 ) }
void test_delete11( void ) { TEST_DELETE( 1, 3, 2, 0, 0, 2, 3, 0, 2, 0 ) }
void test_delete12( void ) { TEST_DELETE( 2, 0, 1, 3, 0, 1, 3, 1, 3, 3 ) }
void test_delete13( void ) { TEST_DELETE( 2, 0, 3, 1, 0, 1, 3, 1, 3, 1 ) }
void test_delete14( void ) { TEST_DELETE( 2, 1, 0, 3, 0, 1, 3, 0, 3, 3 ) }
void test_delete15( void ) { TEST_DELETE( 2, 1, 3, 0, 0, 1, 3, 0, 3, 0 ) }
void test_delete16( void ) { TEST_DELETE( 2, 3, 0, 1, 0, 1, 3, 0, 1, 1 ) }
void test_delete17( void ) { TEST_DELETE( 2, 3, 1, 0, 0, 1, 3, 0, 1, 0 ) }
void test_delete18( void ) { TEST_DELETE( 3, 0, 1, 2, 0, 1, 2, 1, 2, 2 ) }
void test_delete19( void ) { TEST_DELETE( 3, 0, 2, 1, 0, 1, 2, 1, 2, 1 ) }
void test_delete20( void ) { TEST_DELETE( 3, 1, 0, 2, 0, 1, 2, 0, 2, 2 ) }
void test_delete21( void ) { TEST_DELETE( 3, 1, 2, 0, 0, 1, 2, 0, 2, 0 ) }
void test_delete22( void ) { TEST_DELETE( 3, 2, 0, 1, 0, 1, 2, 0, 1, 1 ) }
void test_delete23( void ) { TEST_DELETE( 3, 2, 1, 0, 0, 1, 2, 0, 1, 0 ) }

void test_in_use( void )
{
    INSERT_DUMMY()
    TEST_ASSERT_TRUE( check_name_in_use( &list, names[0].str ));
    TEST_ASSERT_TRUE( check_name_in_use( &list, names[1].str ));
    TEST_ASSERT_TRUE( check_name_in_use( &list, names[2].str ));
    TEST_ASSERT_TRUE( check_name_in_use( &list, names[3].str ));
    TEST_ASSERT_FALSE( check_name_in_use( &list, "Kellan" ));
}

void test_list_send_to_node( void )
{
    const char *str = "It isn't true that my mattress is made of cotton candy.";
    DynString line;
    dynstring_init( &line, 20 );

    FILE *kingston_out = fopen( "build/test_list_send_to_node.out", "w+" );
    pthread_mutex_t lock0, lock1, lock2, lock3;
    pthread_mutex_init( &lock0, 0 );
    list_insert( &list, &names[1], kingston_out, &lock0 );
    list_insert( &list, &names[0], (FILE*)2, &lock1 );
    list_insert( &list, &names[2], (FILE*)3, &lock2 );
    list_insert( &list, &names[3], (FILE*)4, &lock3 );

    list_send_to_node( &list, names[1].str, str );
    rewind( kingston_out );
    dynstring_readline( &line, kingston_out );
    TEST_ASSERT_EQUAL_STRING( str, line.str );

    fclose( kingston_out );
    dynstring_destroy( &line );
}

void test_inc_stat( void )
{
    ListNode *nannie;
    pthread_mutex_t lock0, lock1, lock2, lock3;
    list_insert( &list, &names[1], (FILE*)1, &lock0 );
    list_insert( &list, &names[0], (FILE*)2, &lock1 );
    nannie = list_insert( &list, &names[2], (FILE*)3, &lock2 );
    list_insert( &list, &names[3], (FILE*)4, &lock3 );

    TEST_ASSERT_EQUAL( 0, nannie->data.say );
    TEST_ASSERT_EQUAL( 0, nannie->data.kick );
    TEST_ASSERT_EQUAL( 0, nannie->data.list );

    inc_stat( &list, nannie, 's' );
    TEST_ASSERT_EQUAL( 1, nannie->data.say );
    TEST_ASSERT_EQUAL( 0, nannie->data.kick );
    TEST_ASSERT_EQUAL( 0, nannie->data.list );

    inc_stat( &list, nannie, 'k' );
    TEST_ASSERT_EQUAL( 1, nannie->data.say );
    TEST_ASSERT_EQUAL( 1, nannie->data.kick );
    TEST_ASSERT_EQUAL( 0, nannie->data.list );

    inc_stat( &list, nannie, 'l' );
    TEST_ASSERT_EQUAL( 1, nannie->data.say );
    TEST_ASSERT_EQUAL( 1, nannie->data.kick );
    TEST_ASSERT_EQUAL( 1, nannie->data.list );

    inc_stat( &list, nannie, 'o' );
    TEST_ASSERT_EQUAL( 1, nannie->data.say );
    TEST_ASSERT_EQUAL( 1, nannie->data.kick );
    TEST_ASSERT_EQUAL( 1, nannie->data.list );
}

void test_send_to_all( void )
{
    static const char *test_str = "He liked to play with words in the bathtub.";
    DynString line;
    dynstring_init( &line, 50 );

    pthread_mutex_t lock0, lock1, lock2, lock3;
    pthread_mutex_init( &lock0, 0 );
    pthread_mutex_init( &lock1, 0 );
    pthread_mutex_init( &lock2, 0 );
    pthread_mutex_init( &lock3, 0 );
    FILE *out0 = fopen( "build/test_send_to_all0.out", "w+" );
    FILE *out1 = fopen( "build/test_send_to_all1.out", "w+" );
    FILE *out2 = fopen( "build/test_send_to_all2.out", "w+" );
    FILE *out3 = fopen( "build/test_send_to_all3.out", "w+" );

    list_insert( &list, &names[1], out1, &lock0 );
    list_insert( &list, &names[0], out0, &lock1 );
    list_insert( &list, &names[2], out2, &lock2 );
    list_insert( &list, &names[3], out3, &lock3 );

    send_to_all( &list, test_str );

    rewind( out0 );
    rewind( out1 );
    rewind( out2 );
    rewind( out3 );

    dynstring_readline( &line, out0 );
    TEST_ASSERT_EQUAL_STRING( test_str, line.str );
    dynstring_readline( &line, out1 );
    TEST_ASSERT_EQUAL_STRING( test_str, line.str );
    dynstring_readline( &line, out2 );
    TEST_ASSERT_EQUAL_STRING( test_str, line.str );
    dynstring_readline( &line, out3 );
    TEST_ASSERT_EQUAL_STRING( test_str, line.str );

    fclose( out3 );
    fclose( out2 );
    fclose( out1 );
    fclose( out0 );
    dynstring_destroy( &line );
}

void test_get_names_list0( void )
{
    DynString names;
    dynstring_init( &names, 10 );

    get_names_list( &list, &names );
    TEST_ASSERT_EQUAL_STRING( "", names.str );
    TEST_ASSERT_EQUAL( 0, names.length );
    TEST_ASSERT_EQUAL( 10, names.size );

    dynstring_destroy( &names );
}

void test_get_names_list1( void )
{
    INSERT_DUMMY()
    DynString names;
    dynstring_init( &names, 10 );

    get_names_list( &list, &names );
    TEST_ASSERT_EQUAL_STRING( "Clementine,Kingston,Nannie,Vicki", names.str );
    TEST_ASSERT_EQUAL( 32, names.length );
    TEST_ASSERT_EQUAL( 40, names.size );

    dynstring_destroy( &names );
}

void test_list_print_stats( void )
{
    unsigned int i;
    DynString line;
    dynstring_init( &line, 32 );

    ListNode *clementine, *kingston, *nannie, *vicki;
    pthread_mutex_t lock0, lock1, lock2, lock3;
    kingston = list_insert( &list, &names[1], (FILE*)1, &lock0 );
    clementine = list_insert( &list, &names[0], (FILE*)2, &lock1 );
    nannie = list_insert( &list, &names[2], (FILE*)3, &lock2 );
    vicki = list_insert( &list, &names[3], (FILE*)4, &lock3 );

    for( i = 0; i < 9; i++ ) inc_stat( &list, clementine, 's' );
    for( i = 0; i < 17; i++ ) inc_stat( &list, clementine, 'k' );
    for( i = 0; i < 16; i++ ) inc_stat( &list, clementine, 'l' );

    for( i = 0; i < 16; i++ ) inc_stat( &list, kingston, 's' );
    for( i = 0; i < 14; i++ ) inc_stat( &list, kingston, 'k' );
    for( i = 0; i < 4; i++ ) inc_stat( &list, kingston, 'l' );

    for( i = 0; i < 4; i++ ) inc_stat( &list, nannie, 's' );
    for( i = 0; i < 19; i++ ) inc_stat( &list, nannie, 'k' );
    for( i = 0; i < 17; i++ ) inc_stat( &list, nannie, 'l' );

    for( i = 0; i < 12; i++ ) inc_stat( &list, vicki, 's' );
    for( i = 0; i < 19; i++ ) inc_stat( &list, vicki, 'k' );
    for( i = 0; i < 7; i++ ) inc_stat( &list, vicki, 'l' );

    FILE *err = freopen( "build/test_list_print_stats.err", "w+", stderr );
    list_print_stats( &list );
    rewind( err );

    dynstring_readline( &line, err );
    TEST_ASSERT_EQUAL_STRING( "Clementine:SAY:9:KICK:17:LIST:16", line.str );
    dynstring_readline( &line, err );
    TEST_ASSERT_EQUAL_STRING( "Kingston:SAY:16:KICK:14:LIST:4", line.str );
    dynstring_readline( &line, err );
    TEST_ASSERT_EQUAL_STRING( "Nannie:SAY:4:KICK:19:LIST:17", line.str );
    dynstring_readline( &line, err );
    TEST_ASSERT_EQUAL_STRING( "Vicki:SAY:12:KICK:19:LIST:7", line.str );

    fclose( err );
    dynstring_destroy( &line );
}

int main( void )
{
    UNITY_BEGIN();

    dynstring_nfrom( &names[0], "Clementine", 10, 11 );
    dynstring_nfrom( &names[1], "Kingston", 8, 9 );
    dynstring_nfrom( &names[2], "Nannie", 6, 7 );
    dynstring_nfrom( &names[3], "Vicki", 5, 6 );

    RUN_TEST( test_init );

    RUN_TEST( test_insert0 );
    RUN_TEST( test_insert1 );
    RUN_TEST( test_insert2 );
    RUN_TEST( test_insert3 );
    RUN_TEST( test_insert4 );
    RUN_TEST( test_insert5 );
    RUN_TEST( test_insert6 );
    RUN_TEST( test_insert7 );
    RUN_TEST( test_insert8 );
    RUN_TEST( test_insert9 );
    RUN_TEST( test_insert10 );
    RUN_TEST( test_insert11 );
    RUN_TEST( test_insert12 );
    RUN_TEST( test_insert13 );
    RUN_TEST( test_insert14 );
    RUN_TEST( test_insert15 );
    RUN_TEST( test_insert16 );
    RUN_TEST( test_insert17 );
    RUN_TEST( test_insert18 );
    RUN_TEST( test_insert19 );
    RUN_TEST( test_insert20 );
    RUN_TEST( test_insert21 );
    RUN_TEST( test_insert22 );
    RUN_TEST( test_insert23 );

    RUN_TEST( test_delete_empty );
    RUN_TEST( test_delete0 );
    RUN_TEST( test_delete1 );
    RUN_TEST( test_delete2 );
    RUN_TEST( test_delete3 );
    RUN_TEST( test_delete4 );
    RUN_TEST( test_delete5 );
    RUN_TEST( test_delete6 );
    RUN_TEST( test_delete7 );
    RUN_TEST( test_delete8 );
    RUN_TEST( test_delete9 );
    RUN_TEST( test_delete10 );
    RUN_TEST( test_delete11 );
    RUN_TEST( test_delete12 );
    RUN_TEST( test_delete13 );
    RUN_TEST( test_delete14 );
    RUN_TEST( test_delete15 );
    RUN_TEST( test_delete16 );
    RUN_TEST( test_delete17 );
    RUN_TEST( test_delete18 );
    RUN_TEST( test_delete19 );
    RUN_TEST( test_delete20 );
    RUN_TEST( test_delete21 );
    RUN_TEST( test_delete22 );
    RUN_TEST( test_delete23 );

    RUN_TEST( test_in_use );
    RUN_TEST( test_list_send_to_node );
    RUN_TEST( test_inc_stat );
    RUN_TEST( test_send_to_all );
    RUN_TEST( test_get_names_list0 );
    RUN_TEST( test_get_names_list1 );
    RUN_TEST( test_list_print_stats );

    dynstring_destroy( &names[3] );
    dynstring_destroy( &names[2] );
    dynstring_destroy( &names[1] );
    dynstring_destroy( &names[0] );

    return UNITY_END();
}
