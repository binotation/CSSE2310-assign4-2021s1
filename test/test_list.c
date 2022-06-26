#include "unity.h"
#include "list.h"
#include "dynstring.h"

// Test permutation of order of inserting names.
#define TEST_INSERT( name0, name1, name2, name3 )											\
{																							\
    ClientList list;																		\
    list_init( &list );																		\
    bool available;																			\
    pthread_mutex_t lock0, lock1, lock2, lock3;												\
    FILE *tx0, *tx1, *tx2, *tx3;															\
    tx0 = (FILE*)0; tx1 = (FILE*)1; tx2 = (FILE*)2; tx3 = (FILE*)3;							\
    ListNode *node0, *node1, *node2, *node3;												\
																							\
    node0 = list_node_init( &names[name0], tx0, &lock0 );									\
    available = list_insert( &list, node0 );												\
    TEST_ASSERT_TRUE( available );															\
																							\
    node1 = list_node_init( &names[name1], tx1, &lock1 );									\
    available = list_insert( &list, node1 );												\
    TEST_ASSERT_TRUE( available );															\
																							\
    node2 = list_node_init( &names[name2], tx2, &lock2 );									\
    available = list_insert( &list, node2 );												\
    TEST_ASSERT_TRUE( available );															\
																							\
    node3 = list_node_init( &names[name3], tx3, &lock3 );									\
    available = list_insert( &list, node3 );												\
    TEST_ASSERT_TRUE( available );															\
																							\
    available = list_insert( &list, node0 );												\
    TEST_ASSERT_FALSE( available );															\
    available = list_insert( &list, node1 );												\
    TEST_ASSERT_FALSE( available );															\
    available = list_insert( &list, node2 );												\
    TEST_ASSERT_FALSE( available );															\
    available = list_insert( &list, node3 );												\
    TEST_ASSERT_FALSE( available );															\
																							\
    TEST_ASSERT_EQUAL_STRING( names[0].str, list.head->data.name->str );					\
    TEST_ASSERT_EQUAL_STRING( names[1].str, list.head->next->data.name->str );				\
    TEST_ASSERT_EQUAL_STRING( names[2].str, list.head->next->next->data.name->str );		\
    TEST_ASSERT_EQUAL_STRING( names[3].str, list.head->next->next->next->data.name->str );	\
    list_destroy( &list );																	\
}

// Test permutation of order of deleting names.
#define TEST_DELETE( name0, name1, name2, name3, ord11, ord12, ord13, ord21, ord22, ord31 )	\
{																							\
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
static FILE *tx[4];
static pthread_mutex_t tx_lock[4];
static ListNode *nodes[4];
static DynString line;

void setUp( void )
{
    tx[0] = fopen( "build/test_list_clementine.out", "w+" );
    tx[1] = fopen( "build/test_list_kingston.out", "w+" );
    tx[2] = fopen( "build/test_list_nannie.out", "w+" );
    tx[3] = fopen( "build/test_list_vicki.out", "w+" );
    pthread_mutex_init( &tx_lock[0], 0 );
    pthread_mutex_init( &tx_lock[1], 0 );
    pthread_mutex_init( &tx_lock[2], 0 );
    pthread_mutex_init( &tx_lock[3], 0 );
    list_init( &list );
    nodes[1] = list_node_init( &names[1], tx[1], &tx_lock[1] );
    list_insert( &list, nodes[1] );
    nodes[0] = list_node_init( &names[0], tx[0], &tx_lock[0] );
    list_insert( &list, nodes[0] );
    nodes[2] = list_node_init( &names[2], tx[2], &tx_lock[2] );
    list_insert( &list, nodes[2] );
    nodes[3] = list_node_init( &names[3], tx[3], &tx_lock[3] );
    list_insert( &list, nodes[3] );
    dynstring_init( &line, 50 );
}

void tearDown( void )
{
    dynstring_destroy( &line );
    list_destroy( &list );
    pthread_mutex_destroy( &tx_lock[0] );
    pthread_mutex_destroy( &tx_lock[1] );
    pthread_mutex_destroy( &tx_lock[2] );
    pthread_mutex_destroy( &tx_lock[3] );
    fclose( tx[0] );
    fclose( tx[1] );
    fclose( tx[2] );
    fclose( tx[3] );
}

void test_init( void )
{
    ClientList list;
    list_init( &list );
    TEST_ASSERT_EQUAL( 0, list.head );
    list_destroy( &list );
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
    ClientList list;
    list_init( &list );
    list_delete( &list, names[1].str );
    TEST_ASSERT_EQUAL( 0, list.head );
    list_destroy( &list );
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

void test_list_send_to_node( void )
{
    static const char *str = "It isn't true that my mattress is made of cotton candy.";

    list_send_to_node( &list, names[1].str, str );
    rewind( tx[1] );
    dynstring_readline( &line, tx[1] );
    TEST_ASSERT_EQUAL_STRING( str, line.str );
}

void test_list_inc_stat( void )
{
    TEST_ASSERT_EQUAL( 0, nodes[2]->data.say );
    TEST_ASSERT_EQUAL( 0, nodes[2]->data.kick );
    TEST_ASSERT_EQUAL( 0, nodes[2]->data.list );

    list_inc_stat( nodes[2], 's' );
    TEST_ASSERT_EQUAL( 1, nodes[2]->data.say );
    TEST_ASSERT_EQUAL( 0, nodes[2]->data.kick );
    TEST_ASSERT_EQUAL( 0, nodes[2]->data.list );

    list_inc_stat( nodes[2], 'k' );
    TEST_ASSERT_EQUAL( 1, nodes[2]->data.say );
    TEST_ASSERT_EQUAL( 1, nodes[2]->data.kick );
    TEST_ASSERT_EQUAL( 0, nodes[2]->data.list );

    list_inc_stat( nodes[2], 'l' );
    TEST_ASSERT_EQUAL( 1, nodes[2]->data.say );
    TEST_ASSERT_EQUAL( 1, nodes[2]->data.kick );
    TEST_ASSERT_EQUAL( 1, nodes[2]->data.list );

    list_inc_stat( nodes[2], 'o' );
    TEST_ASSERT_EQUAL( 1, nodes[2]->data.say );
    TEST_ASSERT_EQUAL( 1, nodes[2]->data.kick );
    TEST_ASSERT_EQUAL( 1, nodes[2]->data.list );
}

void test_list_send_to_all( void )
{
    static const char *test_str = "He liked to play with words in the bathtub.";

    list_send_to_all( &list, test_str );

    rewind( tx[0] );
    rewind( tx[1] );
    rewind( tx[2] );
    rewind( tx[3] );

    dynstring_readline( &line, tx[0] );
    TEST_ASSERT_EQUAL_STRING( test_str, line.str );
    dynstring_readline( &line, tx[1] );
    TEST_ASSERT_EQUAL_STRING( test_str, line.str );
    dynstring_readline( &line, tx[2] );
    TEST_ASSERT_EQUAL_STRING( test_str, line.str );
    dynstring_readline( &line, tx[3] );
    TEST_ASSERT_EQUAL_STRING( test_str, line.str );
}

void test_list_get_names_list0( void )
{
    ClientList list;
    list_init( &list );

    list_get_names_list( &list, &line );
    TEST_ASSERT_EQUAL_STRING( "", line.str );
    TEST_ASSERT_EQUAL( 0, line.length );
    TEST_ASSERT_EQUAL( 50, line.size );

    list_destroy( &list );
}

void test_list_get_names_list1( void )
{
    list_get_names_list( &list, &line );
    TEST_ASSERT_EQUAL_STRING( "Clementine,Kingston,Nannie,Vicki", line.str );
    TEST_ASSERT_EQUAL( 32, line.length );
    TEST_ASSERT_EQUAL( 50, line.size );
}

void test_list_print_stats( void )
{
    unsigned int i;

    for( i = 0; i < 9; i++ ) list_inc_stat( nodes[0], 's' );
    for( i = 0; i < 17; i++ ) list_inc_stat( nodes[0], 'k' );
    for( i = 0; i < 16; i++ ) list_inc_stat( nodes[0], 'l' );

    for( i = 0; i < 16; i++ ) list_inc_stat( nodes[1], 's' );
    for( i = 0; i < 14; i++ ) list_inc_stat( nodes[1], 'k' );
    for( i = 0; i < 4; i++ ) list_inc_stat( nodes[1], 'l' );

    for( i = 0; i < 4; i++ ) list_inc_stat( nodes[2], 's' );
    for( i = 0; i < 19; i++ ) list_inc_stat( nodes[2], 'k' );
    for( i = 0; i < 17; i++ ) list_inc_stat( nodes[2], 'l' );

    for( i = 0; i < 12; i++ ) list_inc_stat( nodes[3], 's' );
    for( i = 0; i < 19; i++ ) list_inc_stat( nodes[3], 'k' );
    for( i = 0; i < 7; i++ ) list_inc_stat( nodes[3], 'l' );

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

    RUN_TEST( test_list_send_to_node );
    RUN_TEST( test_list_inc_stat );
    RUN_TEST( test_list_send_to_all );
    RUN_TEST( test_list_get_names_list0 );
    RUN_TEST( test_list_get_names_list1 );
    RUN_TEST( test_list_print_stats );

    dynstring_destroy( &names[3] );
    dynstring_destroy( &names[2] );
    dynstring_destroy( &names[1] );
    dynstring_destroy( &names[0] );

    return UNITY_END();
}
