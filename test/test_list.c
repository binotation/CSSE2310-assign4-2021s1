#include "unity.h"
#include "list.h"

#define INSERT_DUMMY()										\
{															\
    pthread_mutex_t lock0, lock1, lock2, lock3;				\
    list_insert( &list, names[1], (FILE*)1, &lock0 );		\
    list_insert( &list, names[0], (FILE*)2, &lock1 );		\
    list_insert( &list, names[2], (FILE*)3, &lock2 );		\
    list_insert( &list, names[3], (FILE*)4, &lock3 );		\
}

// Test permutation of order of inserting names.
#define TEST_INSERT( name0, name1, name2, name3 )											\
{																							\
    pthread_mutex_t lock0, lock1, lock2, lock3;												\
    list_insert( &list, names[name0], (FILE*)1, &lock0 );									\
    list_insert( &list, names[name1], (FILE*)2, &lock1 );									\
    list_insert( &list, names[name2], (FILE*)3, &lock2 );									\
    list_insert( &list, names[name3], (FILE*)4, &lock3 );									\
																							\
    TEST_ASSERT_EQUAL_STRING( names[0], list.head->data.name );								\
    TEST_ASSERT_EQUAL_STRING( names[1], list.head->next->data.name );						\
    TEST_ASSERT_EQUAL_STRING( names[2], list.head->next->next->data.name );					\
    TEST_ASSERT_EQUAL_STRING( names[3], list.head->next->next->next->data.name );			\
}

// Test permutation of order of deleting names.
#define TEST_DELETE( name0, name1, name2, name3, ord11, ord12, ord13, ord21, ord22, ord31 )	\
{																							\
    INSERT_DUMMY()																			\
																							\
    list_delete( &list, names[name0] );														\
    TEST_ASSERT_EQUAL_STRING( names[ord11], list.head->data.name );							\
    TEST_ASSERT_EQUAL_STRING( names[ord12], list.head->next->data.name );					\
    TEST_ASSERT_EQUAL_STRING( names[ord13], list.head->next->next->data.name );				\
																							\
    list_delete( &list, names[name1] );														\
    TEST_ASSERT_EQUAL_STRING( names[ord21], list.head->data.name );							\
    TEST_ASSERT_EQUAL_STRING( names[ord22], list.head->next->data.name );					\
																							\
    list_delete( &list, names[name2] );														\
    TEST_ASSERT_EQUAL_STRING( names[ord31], list.head->data.name );							\
																							\
    list_delete( &list, names[name3] );														\
    TEST_ASSERT_EQUAL( 0, list.head );														\
}

static ClientList list;
static const char *names[4] = { "Clementine", "Kingston", "Nannie", "Vicki" };

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
    list_delete( &list, names[1] );
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
    TEST_ASSERT_TRUE( check_name_in_use( &list, names[0] ));
    TEST_ASSERT_TRUE( check_name_in_use( &list, names[1] ));
    TEST_ASSERT_TRUE( check_name_in_use( &list, names[2] ));
    TEST_ASSERT_TRUE( check_name_in_use( &list, names[3] ));
    TEST_ASSERT_FALSE( check_name_in_use( &list, "Kellan" ));
}

int main( void )
{
    UNITY_BEGIN();
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

    return UNITY_END();
}
