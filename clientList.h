#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>

#ifndef CLIENT_LIST_H
#define CLIENT_LIST_H

/* Data about a client */
typedef struct {
    char *name; // client's name
    FILE *to; // write stream
    pthread_mutex_t *toLock; // write stream lock
    int say; // no. of times received SAY:
    int kick; // no. of times received KICK:
    int list; // no. of times received LIST:
} ClientData;

typedef struct ClientNode ClientNode;

/* Node for an ordered (by name) linked list of clients */
struct ClientNode {
    ClientData data; // client data
    ClientNode *next; // pointer to the next node
};

void insert_client(ClientNode *root, char *name, FILE *to, 
        pthread_mutex_t *toLock, pthread_mutex_t *listLock);

void delete_client(ClientNode *root, char *name, pthread_mutex_t *listLock);

bool check_name_exists(ClientNode *root, char *name, 
        pthread_mutex_t *listLock);

ClientNode *get_client_node(ClientNode *root, char *name, 
        pthread_mutex_t *listLock);

void inc_stat(ClientNode *node, char stat, pthread_mutex_t *listLock);

void send_to_all(ClientNode *root, const char *cmd, pthread_mutex_t *listLock);

char *get_names_list(ClientNode *root, pthread_mutex_t *listLock);

void show_clients_stats(ClientNode *root, pthread_mutex_t *listLock);

#endif
