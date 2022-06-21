#include "unity.h"
#include "list.h"

#define ASSERT_LIST()																		\
{																							\
    TEST_ASSERT_EQUAL_STRING( names[0], list.head->data.name );								\
    TEST_ASSERT_EQUAL_STRING( names[1], list.head->next->data.name );						\
    TEST_ASSERT_EQUAL_STRING( names[2], list.head->next->next->data.name );					\
    TEST_ASSERT_EQUAL_STRING( names[3], list.head->next->next->next->data.name );			\
}

#define TEST_INSERT( name0, name1, name2, name3 )				\
{																\
    pthread_mutex_t lock0, lock1, lock2, lock3;					\
    list_insert( &list, names[name0], (FILE*)1, &lock0 );		\
    list_insert( &list, names[name1], (FILE*)2, &lock1 );		\
    list_insert( &list, names[name2], (FILE*)3, &lock2 );		\
    list_insert( &list, names[name3], (FILE*)4, &lock3 );		\
    ASSERT_LIST()												\
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
    return UNITY_END();
}
