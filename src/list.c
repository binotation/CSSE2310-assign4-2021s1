#include "list.h"
#include <strings.h>
#include <stdlib.h>

// If tail node, append new_node.
#define APPEND_IF_TAIL( tail )						\
{													\
    if( tail == 0 )									\
    {												\
        tail = new_node;							\
        new_node->next = 0; /* God bless valgrind */\
        pthread_mutex_unlock( &list->lock );		\
        return;										\
    }												\
}

// If new_node < curr->data.name, insert before curr node.
#define INSERT_NODE_IF_ORDERED( prev_next, curr )	\
{													\
    if( strcasecmp( name, curr->data.name ) <= 0 )	\
    {												\
        new_node->next = curr;						\
        prev_next = new_node;						\
        pthread_mutex_unlock( &list->lock );		\
        return;										\
    }												\
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

    // If empty list, append
    APPEND_IF_TAIL( list->head )

    // Head node exists
    INSERT_NODE_IF_ORDERED( list->head, list->head )

    ListNode *curr = list->head->next;
    ListNode *prev = list->head;

    do
    {
        APPEND_IF_TAIL( prev->next )
        INSERT_NODE_IF_ORDERED( prev->next, curr )
        curr = curr->next;
        prev = prev->next;
    } while(1);
}

/**
 *  Deletes a client from the clients' linked list.
 *  Params:
 *      root: linked list root node
 *      name: name of client to delete
 *      listLock: lock for the clients' linked list
 **/
// void delete_client(ClientNode *root, char *name, pthread_mutex_t *listLock) {
//     pthread_mutex_lock(listLock);

//     ClientNode *current = root->next;
//     ClientNode *prev = root;

//     // return if empty
//     if (current == 0) {
//         pthread_mutex_unlock(listLock);
//         return;
//     }
    
//     // go next until name matches
//     while (strcmp(current->data.name, name)) {
//         current = current->next;
//         prev = prev->next;
//         if (current == 0) { // return if at end
//             pthread_mutex_unlock(listLock);
//             return;
//         }
//     }
//     // delete and free node
//     ClientNode *next = current->next;
//     free(current);
//     prev->next = next;
//     pthread_mutex_unlock(listLock);
// }

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
