/**
 * Store a list of clients using a singly linked list. The two primary goals of this data structure
 * are to provide thread-safe access to client information and quickly retrieve a list of clients
 * in alphabetical (or ASCII) order.
 * 
 * Included in a client's data should be a write stream for transmitting to the client. This stream
 * should be thread-safe. The list itself should be mutex locked as opposed to individual nodes
 * because most operations access every node.
 */

#ifndef LIST_H
#define LIST_H

#include "dynstring.h"
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

// Data about a client
typedef struct
{
    const DynString *name;
    FILE *tx;
    pthread_mutex_t *tx_lock;

    // Number of times received SAY:, KICK:, LIST:
    unsigned int say;
    unsigned int kick;
    unsigned int list;
} ClientData;

// List node
typedef struct ListNode ListNode;
struct ListNode
{
    ClientData data;
    ListNode *next;
};

// List of clients
typedef struct
{
    ListNode *head;
    pthread_mutex_t lock;
} ClientList;

/**
 * Initialize list.
 */
void list_init( ClientList *list );

/**
 * Destroy list.
 */
void list_destroy( ClientList *list );

/**
 * Insert a new client into a list.
 */
void list_insert( ClientList *list, const DynString *name, FILE *tx, pthread_mutex_t *tx_lock );

/**
 * Delete a client identified by name.
 */
void list_delete( ClientList *list, const char *name );

/**
 * Check if name is already being used by a client.
 */
bool check_name_in_use( ClientList *list, const char *name );

/**
 * Retrieve a node reference identified by client name. If not found, NULL is returned.
 */
ListNode *get_node( ClientList *list, const char *name );

/**
 * Increment a client's stat given its node.
 * @param stat	the stat to increment: 's' -> SAY:, 'k' -> KICK:, 'l' -> LIST:
 */
void inc_stat( ClientList *list, ListNode *client, const char stat );

/**
 * Send str to all clients.
 */
void send_to_all( ClientList *list, const char *str );

/**
 * Get a list of client names as a dynstring in comma-separated format. E.g. "name1,name2,name3".
 */
void get_names_list( ClientList *list, DynString *names );

// void show_clients_stats(ClientNode *root, pthread_mutex_t *listLock);

#endif
