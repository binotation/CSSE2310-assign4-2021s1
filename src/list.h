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

#include <stdio.h>
#include <pthread.h>

// Data about a client
typedef struct
{
    const char *name;
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
void list_insert( ClientList *list, const char *name, FILE *tx, pthread_mutex_t *tx_lock );

/**
 * Delete a client identified by name.
 */
// void list_delete( ClientList *list, char *name );

// bool check_name_exists(ClientNode *root, char *name, pthread_mutex_t *listLock);

// ClientNode *get_client_node(ClientNode *root, char *name, pthread_mutex_t *listLock);

// void inc_stat(ClientNode *node, char stat, pthread_mutex_t *listLock);

// void send_to_all(ClientNode *root, const char *cmd, pthread_mutex_t *listLock);

// char *get_names_list(ClientNode *root, pthread_mutex_t *listLock);

// void show_clients_stats(ClientNode *root, pthread_mutex_t *listLock);

#endif
