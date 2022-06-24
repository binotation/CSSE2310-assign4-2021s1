#include "list.h"
#include <stdlib.h>
#include <string.h>
#include <strings.h>

// If curr is tail -> append, else if ordered -> insert.
#define INSERT_NODE( prev_next, curr )								\
{																	\
    if( curr == 0 )													\
    {																\
        prev_next = new_node;										\
        new_node->next = 0; /* God bless valgrind */				\
        pthread_mutex_unlock( &list->lock );						\
        return new_node;											\
    }																\
    else if( strcasecmp( name->str, curr->data.name->str ) <= 0 )	\
    {																\
        new_node->next = curr;										\
        prev_next = new_node;										\
        pthread_mutex_unlock( &list->lock );						\
        return new_node;											\
    }																\
}

#define DELETE_NODE( prev_next, curr_next, target )	\
{													\
    prev_next = curr_next;							\
    free( target );									\
    pthread_mutex_unlock( &list->lock );			\
    return;											\
}

void list_init( ClientList *list )
{
    list->head = 0;
    pthread_mutex_init( &list->lock, 0 );
}

void list_destroy( ClientList *list )
{
    ListNode *next, *curr;
    pthread_mutex_lock( &list->lock );
    curr = list->head;

    if( curr == 0 )
    {
    }
    else if( curr->next == 0 )
    {
        free( curr );
    }
    else
    {
        do
        {
            next = curr->next;
            free( curr );
            curr = next;
        } while ( curr != 0 );
    }

    pthread_mutex_unlock( &list->lock );
    pthread_mutex_destroy( &list->lock );
}

ListNode *list_insert( ClientList *list, const DynString *name, FILE *tx, pthread_mutex_t *tx_lock )
{
    // Create new node
    ListNode *new_node = malloc( sizeof(ListNode) );
    new_node->data = (ClientData)
    {
        .name = name,
        .tx = tx,
        .tx_lock = tx_lock,
        .say = 0,
        .kick = 0,
        .list = 0,
    };

    pthread_mutex_lock( &list->lock );

    // Head node edge-case
    INSERT_NODE( list->head, list->head )
    ListNode *curr = list->head->next;
    ListNode *prev = list->head;

    do
    {
        INSERT_NODE( prev->next, curr )
        curr = curr->next;
        prev = prev->next;
    } while(1);
}

void list_delete( ClientList *list, const char *name )
{
    ListNode *curr, *prev, *target;
    pthread_mutex_lock( &list->lock );

    if( list->head == 0 )
    {
        pthread_mutex_unlock( &list->lock );
        return;
    }
    else if( !strcmp( name, list->head->data.name->str ))
    {
        target = list->head;
        DELETE_NODE( list->head, list->head->next, target )
    }
    else
    {
        prev = list->head;
        curr = list->head->next;
        do
        {
            if( curr == 0 )
            {
                pthread_mutex_unlock( &list->lock );
                return;
            }
            else if( !strcmp( name, curr->data.name->str ))
            {
                DELETE_NODE( prev->next, curr->next, curr )
            }
            prev = prev->next;
            curr = curr->next;
        } while(1);
    }
}

bool check_name_in_use( ClientList *list, const char *name )
{
    bool in_use = false;
    pthread_mutex_lock( &list->lock );

    ListNode *curr = list->head;
    while( curr != 0 && !in_use )
    {
        in_use = in_use || !strcmp( curr->data.name->str, name );
        curr = curr->next;
    }

    pthread_mutex_unlock( &list->lock );
    return in_use;
}

ListNode *get_node( ClientList *list, const char *name )
{
    pthread_mutex_lock( &list->lock );

    ListNode *curr = list->head;
    while( curr != 0 && strcmp( name, curr->data.name->str ))
    {
        curr = curr->next;
    }

    pthread_mutex_unlock( &list->lock );
    return curr;
}

void inc_stat( ClientList *list, ListNode *client, const char stat )
{
    pthread_mutex_lock( &list->lock );
    switch( stat )
    {
        case 's':
            client->data.say++;
            break;
        case 'k':
            client->data.kick++;
            break;
        case 'l':
            client->data.list++;
    }
    pthread_mutex_unlock( &list->lock );
}

// Avoid deadlock by ensuring nothing holds a tx_lock while trying to lock a list->lock.
void send_to_all( ClientList *list, const char *str )
{
    pthread_mutex_lock( &list->lock );

    ListNode *curr = list->head;
    while( curr != 0 )
    {
        pthread_mutex_lock( curr->data.tx_lock );

        fputs( str, curr->data.tx );
        fflush( curr->data.tx );

        pthread_mutex_unlock( curr->data.tx_lock );
        curr = curr->next;
    }

    pthread_mutex_unlock( &list->lock );
}

void get_names_list( ClientList *list, DynString *names )
{
    dynstring_clear( names );
    pthread_mutex_lock( &list->lock );

    ListNode *curr = list->head;
    while( curr != 0 )
    {
        dynstring_npush( names, curr->data.name->str, curr->data.name->length );
        dynstring_pushc( names, ',' );
        curr = curr->next;
    }
    pthread_mutex_unlock( &list->lock );
    dynstring_popc( names );
}

void list_print_stats( ClientList *list )
{
    pthread_mutex_lock( &list->lock );

    ListNode *curr = list->head;
    while( curr != 0 )
    {
        fprintf( stderr, "%s:SAY:%u:KICK:%u:LIST:%u\n",
            curr->data.name->str, curr->data.say, curr->data.kick, curr->data.list );
        curr = curr->next;
    }

    pthread_mutex_unlock( &list->lock );
}
