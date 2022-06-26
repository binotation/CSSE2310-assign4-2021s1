/**
 * Store a list of clients using a singly linked list. There are 5 functions the list must implement.
 * 1. list_send_to_node():		Allow a client_handler thread to send data to a target client.
 * 2. list_send_to_all():		Allow a client_handler thread to send data to all clients at once.
 * 3. list_get_names_list():	Allow a client_handler thread to retrieve a list of all clients' names.
 * 4. list_inc_stat():			Allow a client_handler thread to update its own client's stats.
 * 5. list_print_stats():		Allow the print_stats_sig_handler thread to print all clients' stats to stderr.
 * 
 * A client_handler thread is coupled with its own client_node; in fact, the lifetime of a client's
 * list node should be a subset of the client_handler thread's lifetime. Only the dedicated
 * client_handler thread can create and destroy it's own list node.
 * 
 * The list itself should be mutex protected as opposed to individual nodes because many operations
 * access every node.
 */

#ifndef LIST_H
#define LIST_H

#include "dynstring.h"
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

/**
 * Data about a client. This list is not responsible for freeing up allocated memory pointed to by
 * members of this struct, i.e. *name and *tx. Below, I describe how the variable is shared by its
 * client_handler thread.
 * @param name		name dynstring, shared read-only
 * @param tx		stream for transmitting to client, shared writable
 * @param tx_lock	pointer to transmission stream mutex, shared mutex (atomic)
 * @param say		number of times received SAY: from client, not shared
 * @param kick		number of times received KICK: from client, not shared
 * @param list		number of times received LIST: from client, not shared
 */
typedef struct
{
    const DynString *name;
    FILE *tx;
    pthread_mutex_t *tx_lock;
    unsigned int say;
    unsigned int kick;
    unsigned int list;
} ClientData;

/**
 * A node in the linked list.
 * @param data	client data
 * @param next	pointer to the next node
 */
typedef struct ListNode ListNode;
struct ListNode
{
    ClientData data;
    ListNode *next;
};

/**
 * Represents the list of clients.
 * @param head	the head node of the linked list
 * @param lock	a mutex which protects the list
 */
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
 * Initialize list node.
 * @returns list node
 */
ListNode *list_node_init( const DynString *name, FILE *tx, pthread_mutex_t *tx_lock );

/**
 * Insert a new client into a list.
 * @returns if successful or not, insert fails if the node to be inserted contains a name that is
 * in use by another client
 */
bool list_insert( ClientList *list, ListNode *node );

/**
 * Delete a client identified by name. Only the node is free'd, its members must be free'd by the
 * client_handler thread.
 */
void list_delete( ClientList *list, const char *name );

// Specific-purpose functions
/**
 * Write str to node tx stream.
 */
void list_send_to_node( ClientList *list, const char *name, const char *str );

/**
 * Send str to all clients. This function could cause a deadlock if a thread holds a tx_lock and
 * tries to take a list lock.
 */
void list_send_to_all( ClientList *list, const char *str );

/**
 * Get a list of client names as a dynstring in comma-separated format, e.g. "name1,name2,name3".
 */
void list_get_names_list( ClientList *list, DynString *names );

/**
 * Increment a client's stat given its node. This is the only function where non-shared data is
 * accessed so it is safe to not lock the list mutex.
 * @param stat	the stat to increment: 's' -> SAY:, 'k' -> KICK:, 'l' -> LIST:
 */
void list_inc_stat( ListNode *client, const char stat );

/**
 * Print client stats to stderr, e.g. for each client print "name:SAY:n:KICK:n:LIST:n".
 * WARNING! this function does not write to stderr in a thread-safe manner because stderr is not
 * always a shared resource in the server.
 */
void list_print_stats( ClientList *list );

#endif
