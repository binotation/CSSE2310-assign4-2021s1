/*
 * Store a list of clients using a singly linked list. The two primary goals of this data structure
 * are to provide thread-safe access to client information and quickly retrieve a list of clients
 * in alphabetical (or ASCII) order.
 * 
 * Included in a client's data should be a write stream for transmitting to the client. This stream
 * should be thread-safe. The list itself should be mutex locked as opposed to individual nodes
 * because most operations are O(N). The root node is an empty ClientNode.
 */

#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>

#ifndef CLIENT_LIST_H
#define CLIENT_LIST_H

/* Data about a client */
typedef struct {
    char *name;
    // Transmit stream and lock
    FILE *to;
    pthread_mutex_t *toLock;
    // Number of times received SAY:, KICK:, LIST:
    int say;
    int kick;
    int list;
} ClientData;

typedef struct ClientNode ClientNode;

/* Node for an ordered (by name) linked list of clients */
struct ClientNode {
    ClientData data;
    ClientNode *next;
};

void insert_client(ClientNode *root, char *name, FILE *to, pthread_mutex_t *toLock, pthread_mutex_t *listLock);

void delete_client(ClientNode *root, char *name, pthread_mutex_t *listLock);

bool check_name_exists(ClientNode *root, char *name, pthread_mutex_t *listLock);

ClientNode *get_client_node(ClientNode *root, char *name, pthread_mutex_t *listLock);

void inc_stat(ClientNode *node, char stat, pthread_mutex_t *listLock);

void send_to_all(ClientNode *root, const char *cmd, pthread_mutex_t *listLock);

char *get_names_list(ClientNode *root, pthread_mutex_t *listLock);

void show_clients_stats(ClientNode *root, pthread_mutex_t *listLock);

#endif
