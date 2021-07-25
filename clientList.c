#include <pthread.h>
#include <strings.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "clientList.h"
#include "helpers.h"

/** 
 *  Inserts a new client into the clients' linked list.
 *  Params:
 *      root: linked list root node
 *      name: new client's name
 *      to: new client's write stream
 *      toLock: new client's write stream lock
 *      listLock: lock for the clients' linked list
 **/
void insert_client(ClientNode *root, char *name, FILE *to, 
        pthread_mutex_t *toLock, pthread_mutex_t *listLock) {
    ClientNode *newNode = (ClientNode*)calloc(1, sizeof(ClientNode));
    // set new client's data
    newNode->data.name = name;
    newNode->data.to = to;
    newNode->data.toLock = toLock;

    pthread_mutex_lock(listLock);

    ClientNode *current = root->next;
    ClientNode *prev = root;
    char *nextName;

    // if empty list, append
    if (current == 0) {
        prev->next = newNode;
        pthread_mutex_unlock(listLock);
        return;
    }

    nextName = current->data.name;
    while (strcasecmp(name, nextName) > 0) {
        // go next until newName < nextName lexicographically
        current = current->next;
        prev = prev->next;

        if (current == 0) { // end of list
            prev->next = newNode; // append
            pthread_mutex_unlock(listLock);
            return;
        }
        nextName = current->data.name;
    }
    // insert newNode: prev->new->current
    newNode->next = current;
    prev->next = newNode;
    pthread_mutex_unlock(listLock);
}

/**
 *  Deletes a client from the clients' linked list.
 *  Params:
 *      root: linked list root node
 *      name: name of client to delete
 *      listLock: lock for the clients' linked list
 **/
void delete_client(ClientNode *root, char *name, pthread_mutex_t *listLock) {
    pthread_mutex_lock(listLock);

    ClientNode *current = root->next;
    ClientNode *prev = root;

    // return if empty
    if (current == 0) {
        pthread_mutex_unlock(listLock);
        return;
    }
    
    // go next until name matches
    while (strcmp(current->data.name, name)) {
        current = current->next;
        prev = prev->next;
        if (current == 0) { // return if at end
            pthread_mutex_unlock(listLock);
            return;
        }
    }
    // delete and free node
    ClientNode *next = current->next;
    free(current);
    prev->next = next;
    pthread_mutex_unlock(listLock);
}

/**
 *  Checks if a name is being used by an existing client.
 *  Params:
 *      root: linked list root node
 *      name: name to check
 *      listLock: lock for the clients' linked list
 *  Returns: true if the name is being used, false otherwise.
 **/
bool check_name_exists(ClientNode *root, char *name, 
        pthread_mutex_t *listLock) {
    bool exists = false;
    pthread_mutex_lock(listLock);

    ClientNode *current = root->next;
    while (current != 0 && !exists) {
        exists = exists || !strcmp(current->data.name, name);
        current = current->next;
    }
    pthread_mutex_unlock(listLock);
    return exists;
}

/**
 *  Gets the client node for the corresponding client name.
 *  Params:
 *      root: linked list root node
 *      name: name of client to get node for
 *      listLock: lock for the clients' linked list
 *  Returns: the client node for the client with name or NULL if not found.
 **/
ClientNode *get_client_node(ClientNode *root, char *name, 
        pthread_mutex_t *listLock) {
    pthread_mutex_lock(listLock);

    ClientNode *current = root->next;
    while (current != 0 && strcmp(name, current->data.name)) {
        current = current->next;
    }
    pthread_mutex_unlock(listLock);
    return current;
}

/**
 *  Increments a client's stat specified by char stat.
 *  Params: 
 *      node: the client's node
 *      stat: the stat to increment - 's' for SAY:, 'k' for KICK:, 
 *      'l' for LIST:
 *      listLock: lock for the clients' linked list
 **/
void inc_stat(ClientNode *node, char stat, pthread_mutex_t *listLock) {
    pthread_mutex_lock(listLock);
    if (stat == 's') {
        node->data.say++;
    } else if (stat == 'k') {
        node->data.kick++;
    } else if (stat == 'l') {
        node->data.list++;
    }
    pthread_mutex_unlock(listLock);
}

/**
 *  Sends a command string to all clients.
 *  Params:
 *      root: linked list root node
 *      cmd: command to send
 *      listLock: lock for the clients' linked list
 **/
void send_to_all(ClientNode *root, const char *cmd,
        pthread_mutex_t *listLock) {
    pthread_mutex_lock(listLock);

    ClientNode *current = root->next;
    while (current != 0) {
        pthread_mutex_lock(current->data.toLock);
        // send
        fputs(cmd, current->data.to);
        fflush(current->data.to);

        pthread_mutex_unlock(current->data.toLock);
        current = current->next;
    }
    pthread_mutex_unlock(listLock);
}

/**
 *  Get a list of client names.
 *  Params:
 *      root: linked list root node
 *      listLock: lock for the clients' linked list
 *  Returns: a list of client names as a string.
 **/
char *get_names_list(ClientNode *root, pthread_mutex_t *listLock) {
    int nameLength;
    // String for list of names e.g. name1,name2,name3
    String names;
    memset(&names, 0, sizeof(String));

    pthread_mutex_lock(listLock);

    ClientNode *current = root->next;
    while (current != 0) {
        nameLength = strlen(current->data.name);
        // allocate memory for name and comma
        names.chars = (char*)realloc(names.chars, sizeof(char) * 
                (names.size += nameLength + 1));

        strncpy(names.chars + names.length, current->data.name, nameLength);
        // update length including comma
        names.length += nameLength + 1;
        names.chars[names.length - 1] = ',';
        current = current->next;
    }
    pthread_mutex_unlock(listLock);
    // replace last comma with null terminator
    names.chars[names.length - 1] = '\0';
    return names.chars;
}

/**
 *  Prints each clients stats to stderr. e.g. clientName:SAY:5:KICK:2:LIST:3 .
 *  Params:
 *      root: linked list root node
 *      listLock: lock for the clients' linked list
 **/
void show_clients_stats(ClientNode *root, pthread_mutex_t *listLock) {
    pthread_mutex_lock(listLock);

    ClientNode *current = root->next;
    while (current != 0) {
        char *conv = convert_unprintable(current->data.name);
        fprintf(stderr, "%s:SAY:%d:KICK:%d:LIST:%d\n", conv,
                current->data.say, current->data.kick, current->data.list);
        free(conv);
        current = current->next;
    }
    pthread_mutex_unlock(listLock);
}
