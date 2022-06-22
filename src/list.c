#include "list.h"
#include <stdlib.h>
#include <string.h>
#include <strings.h>

// If curr is tail -> append, else if ordered -> insert.
#define INSERT_NODE( prev_next, curr )					\
{														\
    if( curr == 0 )										\
    {													\
        prev_next = new_node;							\
        new_node->next = 0; /* God bless valgrind */	\
        pthread_mutex_unlock( &list->lock );			\
        return;											\
    }													\
    else if( strcasecmp( name, curr->data.name ) <= 0 )	\
    {													\
        new_node->next = curr;							\
        prev_next = new_node;							\
        pthread_mutex_unlock( &list->lock );			\
        return;											\
    }													\
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

void list_insert( ClientList *list, const char *name, FILE *tx, pthread_mutex_t *tx_lock )
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
    else if( !strcmp( name, list->head->data.name ))
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
            else if( !strcmp( name, curr->data.name ))
            {
                DELETE_NODE( prev->next, curr->next, curr )
            }
            prev = prev->next;
            curr = curr->next;
        } while(1);
    }
}

/**
 *  Checks if a name is being used by an existing client.
 *  Params:
 *      root: linked list root node
 *      name: name to check
 *      listLock: lock for the clients' linked list
 *  Returns: true if the name is being used, false otherwise.
 **/
// bool check_name_exists(ClientNode *root, char *name, pthread_mutex_t *listLock) {
//     bool exists = false;
//     pthread_mutex_lock(listLock);

//     ClientNode *current = root->next;
//     while (current != 0 && !exists) {
//         exists = exists || !strcmp(current->data.name, name);
//         current = current->next;
//     }
//     pthread_mutex_unlock(listLock);
//     return exists;
// }

/**
 *  Gets the client node for the corresponding client name.
 *  Params:
 *      root: linked list root node
 *      name: name of client to get node for
 *      listLock: lock for the clients' linked list
 *  Returns: the client node for the client with name or NULL if not found.
 **/
// ClientNode *get_client_node(ClientNode *root, char *name, pthread_mutex_t *listLock) {
//     pthread_mutex_lock(listLock);

//     ClientNode *current = root->next;
//     while (current != 0 && strcmp(name, current->data.name)) {
//         current = current->next;
//     }
//     pthread_mutex_unlock(listLock);
//     return current;
// }

/**
 *  Increments a client's stat specified by char stat.
 *  Params: 
 *      node: the client's node
 *      stat: the stat to increment - 's' for SAY:, 'k' for KICK:, 
 *      'l' for LIST:
 *      listLock: lock for the clients' linked list
 **/
// void inc_stat(ClientNode *node, char stat, pthread_mutex_t *listLock) {
//     pthread_mutex_lock(listLock);
//     if (stat == 's') {
//         node->data.say++;
//     } else if (stat == 'k') {
//         node->data.kick++;
//     } else if (stat == 'l') {
//         node->data.list++;
//     }
//     pthread_mutex_unlock(listLock);
// }

/**
 *  Sends a command string to all clients.
 *  Params:
 *      root: linked list root node
 *      cmd: command to send
 *      listLock: lock for the clients' linked list
 **/
// void send_to_all(ClientNode *root, const char *cmd, pthread_mutex_t *listLock) {
//     pthread_mutex_lock(listLock);

//     ClientNode *current = root->next;
//     while (current != 0) {
//         pthread_mutex_lock(current->data.toLock);
//         // send
//         fputs(cmd, current->data.to);
//         fflush(current->data.to);

//         pthread_mutex_unlock(current->data.toLock);
//         current = current->next;
//     }
//     pthread_mutex_unlock(listLock);
// }

/**
 *  Get a list of client names.
 *  Params:
 *      root: linked list root node
 *      listLock: lock for the clients' linked list
 *  Returns: a list of client names as a string.
 **/
// char *get_names_list(ClientNode *root, pthread_mutex_t *listLock) {
//     int nameLength;
//     // String for list of names e.g. name1,name2,name3
//     String names;
//     memset(&names, 0, sizeof(String));

//     pthread_mutex_lock(listLock);

//     ClientNode *current = root->next;
//     while (current != 0) {
//         nameLength = strlen(current->data.name);
//         // allocate memory for name and comma
//         names.chars = (char*)realloc(names.chars, sizeof(char) * (names.size += nameLength + 1));

//         strncpy(names.chars + names.length, current->data.name, nameLength);
//         // update length including comma
//         names.length += nameLength + 1;
//         names.chars[names.length - 1] = ',';
//         current = current->next;
//     }
//     pthread_mutex_unlock(listLock);
//     // replace last comma with null terminator
//     names.chars[names.length - 1] = '\0';
//     return names.chars;
// }

/**
 *  Prints each clients stats to stderr. e.g. clientName:SAY:5:KICK:2:LIST:3 .
 *  Params:
 *      root: linked list root node
 *      listLock: lock for the clients' linked list
 **/
// void show_clients_stats(ClientNode *root, pthread_mutex_t *listLock) {
//     pthread_mutex_lock(listLock);

//     ClientNode *current = root->next;
//     while (current != 0) {
//         char *conv = convert_unprintable(current->data.name);
//         fprintf(stderr, "%s:SAY:%d:KICK:%d:LIST:%d\n", conv, current->data.say, current->data.kick, current->data.list);
//         free(conv);
//         current = current->next;
//     }
//     pthread_mutex_unlock(listLock);
// }
