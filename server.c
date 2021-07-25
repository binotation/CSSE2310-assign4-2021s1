#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "helpers.h"
#include "clientList.h"

/* The server's number of commands received stats. */
typedef struct {
    int auth;
    int name;
    int say;
    int kick;
    int list;
    int leave;
} Received;

/* Types of commands that can be received by the server. */
typedef enum {
    AUTH, NAME, SAY, KICK, LIST, LEAVE
} ReceivedType;

/* Arg for signal handler routine. */
typedef struct {
    Received *received; // received stats
    pthread_mutex_t *receivedLock; // lock for received stats
    ClientNode *clients; // clients' linked list root node
    pthread_mutex_t *clientsLock; // lock for clients' linked list
    sigset_t *set; // sigset for which signals to handle
} SigHandlerArg;

/* Arg for client handler routine. */
typedef struct {
    int *clientFd; // client file descriptor
    char *auth; // auth string
    Received *received; // received stats
    pthread_mutex_t *receivedLock; // lock for received stats
    ClientNode *clients; // clients' linked list root node
    pthread_mutex_t *clientsLock; // lock for clients' linked list
    pthread_mutex_t *stdoutLock; // lock for stdout
} HandleClientArg;

/**
 *  Creates a listening socket binded to port. The port number gets printed.
 *  Exits with status 2 if there is a failure.
 *  Params:
 *      port: the port number, if 0 then an arbitrary available port is 
 *      chosen.
 *  Returns: the file descriptor for the socket.
 **/
int get_listening_socket(const char *port) {
    struct addrinfo *res = 0;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (getaddrinfo(NULL, port, &hints, &res)) {
        freeaddrinfo(res);
        fputs(COMM_ERR_MSG, stderr);
        exit(2);
    }
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);

    if (bind(listenFd, (struct sockaddr*)res->ai_addr, 
            sizeof(struct sockaddr))) {
        freeaddrinfo(res);
        fputs(COMM_ERR_MSG, stderr);
        exit(2);
    }
    freeaddrinfo(res);
    // identify port number
    socklen_t len = sizeof(struct sockaddr_in);
    struct sockaddr_in ad;
    if (getsockname(listenFd, (struct sockaddr*)&ad, &len)) {
        fputs(COMM_ERR_MSG, stderr);
        exit(2);
    }
    fprintf(stderr, "%u\n", ntohs(ad.sin_port));

    if (listen(listenFd, 10) < 0) {
        fputs(COMM_ERR_MSG, stderr);
        exit(2);
    }

    return listenFd;
}

/**
 *  Thread safe stream writer.
 *  Params:
 *      to: write stream
 *      line: line to write
 *      toLock: write stream lock
 **/
void send_r(FILE *to, const char *line, pthread_mutex_t *toLock) {
    pthread_mutex_lock(toLock);
    fputs(line, to);
    fflush(to);
    pthread_mutex_unlock(toLock);
}

/**
 *  Log a received message by updating the received stats.
 *  Params:
 *      received: pointer to received stats
 *      receivedLock: received stats lock
 *      type: the type of message received
 **/
void log_received(Received *received, pthread_mutex_t *receivedLock,
        ReceivedType type) {
    pthread_mutex_lock(receivedLock);
    switch (type) {
        case AUTH:
            received->auth++;
            break;
        case NAME:
            received->name++;
            break;
        case SAY:
            received->say++;
            break;
        case KICK:
            received->kick++;
            break;
        case LIST:
            received->list++;
            break;
        case LEAVE:
            received->leave++;
    }
    pthread_mutex_unlock(receivedLock);
}

/**
 *  Cleans up client by closing streams, destroying the write stream lock,
 *  freeing the file descriptor and thread routine arg.
 *  Params:
 *      to: write stream
 *      from: read stream
 *      toLock: write stream lock
 *      clientFd: the client's file descriptor
 *      arg: the client's thread arg
 **/
void clean_up_client(FILE *to, FILE *from, pthread_mutex_t *toLock,
        int *clientFd, HandleClientArg *arg) {
    fclose(to);
    fclose(from);
    pthread_mutex_destroy(toLock);
    free(clientFd);
    free(arg);
}

/**
 *  Authenticates the client.
 *  Params:
 *      to: client write stream
 *      from: client read stream
 *      auth: auth string
 *      toLock: write stream lock
 *      received: received stats
 *      receivedLock: lock for received stats
 *  Returns: true if client was authenticated, false otherwise
 **/
bool authenticate_client(FILE *to, FILE *from, const char *auth, 
        pthread_mutex_t *toLock, Received *received, 
        pthread_mutex_t *receivedLock) {
    send_r(to, "AUTH:\n", toLock);
    int readsBeforeEof;
    String line = get_line(from, &readsBeforeEof);
    
    while (readsBeforeEof == -1) {
        if (!strncmp(line.chars, "AUTH:", 5)) {
            log_received(received, receivedLock, AUTH);
            if (!strcmp(line.chars + 5, auth)) {
                send_r(to, "OK:\n", toLock);
                free(line.chars);
                return true;
            } else {
                free(line.chars);
                return false;
            }
        }
        free(line.chars);
        if (readsBeforeEof == -1) {
            send_r(to, "AUTH:\n", toLock);
            line = get_line(from, &readsBeforeEof);
        }
    }
    free(line.chars);
    return false;
}

/**
 *  Identifies the client's name.
 *  Params:
 *      to: client write stream
 *      from: client read stream
 *      toLock: write stream lock
 *      received: received stats
 *      receivedLock: lock for received stats
 *      clients: clients' linked list root node
 *      clientsLock: clients' list lock
 *  Returns: client's name if name negotiation was completed, NULL if client
 *  disconnected.
 **/
char *identify_client(FILE *to, FILE *from, pthread_mutex_t *toLock,
        Received *received, pthread_mutex_t *receivedLock,
        ClientNode *clients, pthread_mutex_t *clientsLock) {
    send_r(to, "WHO:\n", toLock);
    int readsBeforeEof;

    String line = get_line(from, &readsBeforeEof);
    while (readsBeforeEof == -1) {
        if (!strncmp(line.chars, "NAME:", 5)) {
            log_received(received, receivedLock, NAME);
            if (line.length == 5 || 
                    check_name_exists(clients, line.chars + 5, clientsLock)) {
                send_r(to, "NAME_TAKEN:\n", toLock);
            } else {
                send_r(to, "OK:\n", toLock);
                int nameLength = strlen(line.chars + 5);
                char *name = (char*)malloc(sizeof(char) * (nameLength + 1));
                strcpy(name, line.chars + 5);
                insert_client(clients, name, to, toLock, clientsLock);
                free(line.chars);
                return name;
            }
        }
        free(line.chars);
        if (readsBeforeEof == -1) {
            send_r(to, "WHO:\n", toLock);
            line = get_line(from, &readsBeforeEof);
        }
    }
    free(line.chars);
    return NULL;
}

/**
 *  Sends ENTER:name to all clients and prints (name has entered the chat) to
 *  stdout.
 *  Params: 
 *      clients: clients' linked list
 *      name: client's name that entered
 *      clientsLock: lock for clients' linked list
 *      stdoutLock: lock for stdout
 **/
void send_enter(ClientNode *clients, const char *name, 
        pthread_mutex_t *clientsLock, pthread_mutex_t *stdoutLock) {
    // allocate total cmd length = 
    // strlen("ENTER:") + strlen(name) + strlen("\n\0") = strlen(name) + 6 + 2
    char *cmd = (char*)malloc(sizeof(char) * (strlen(name) + 8));

    // send to all
    char *conv = convert_unprintable(name);
    sprintf(cmd, "ENTER:%s\n", conv);
    send_to_all(clients, cmd, clientsLock);
    free(cmd);

    // print to stdout
    pthread_mutex_lock(stdoutLock);
    printf("(%s has entered the chat)\n", conv);
    fflush(stdout);
    pthread_mutex_unlock(stdoutLock);
    free(conv);
}

/**
 *  Sends LEAVE:name to all clients and prints (name has left the chat) to 
 *  stdout.
 *  Params:
 *      clients: clients' linked list
 *      name: client's name that left
 *      clientsLock: lock for clients' linked list
 *      stdoutLock: lock for stdout
 **/
void send_leave(ClientNode *clients, const char *name, 
        pthread_mutex_t *clientsLock, pthread_mutex_t *stdoutLock) {
    // allocate total cmd length = 
    // strlen("LEAVE:") + strlen(name) + strlen("\n\0") = strlen(name) + 6 + 2
    char *cmd = (char*)malloc(sizeof(char) * (strlen(name) + 8));
    
    // send to all
    char *conv = convert_unprintable(name);
    sprintf(cmd, "LEAVE:%s\n", conv);
    send_to_all(clients, cmd, clientsLock);
    free(cmd);

    // print to stdout
    pthread_mutex_lock(stdoutLock);
    printf("(%s has left the chat)\n", conv);
    fflush(stdout);
    pthread_mutex_unlock(stdoutLock);
    free(conv);
}

/**
 *  Handles receiving SAY:NAME:MESSAGE - increments the client's stat, logs 
 *  received, sends MSG:NAME:MESSAGE to all clients and prints NAME: MESSAGE 
 *  to stdout.
 *  Params:
 *      node: the node of the client that sent the SAY: message
 *      clientsLock: lock for the clients' linked list
 *      received: received stats
 *      receivedLock: received stats lock
 *      clients: clients' linked list root node
 *      name: name of client that sent the SAY: message
 *      msg: contents of the SAY: msg
 *      stdoutLock: lock for stdout
 **/
void handle_say(ClientNode *node, pthread_mutex_t *clientsLock, 
        Received *received, pthread_mutex_t *receivedLock,
        ClientNode *clients, const char *name, const char *msg, 
        pthread_mutex_t *stdoutLock) {
    // update stats
    inc_stat(node, 's', clientsLock);
    log_received(received, receivedLock, SAY);

    // allocate total cmd length = 
    // length("MSG:") + length(name) +  length(":") + length(message) + 
    // length("\n\0") = 4 + length(name) + 1 + length(message) + 2
    int cmdLength = strlen(name) + strlen(msg) + 7;
    char *cmd = (char*)malloc(sizeof(char) * cmdLength);

    char *convName = convert_unprintable(name);
    char *convMsg = convert_unprintable(msg);
    sprintf(cmd, "MSG:%s:%s\n", convName, convMsg);
    send_to_all(clients, cmd, clientsLock);
    
    free(cmd);

    pthread_mutex_lock(stdoutLock);
    printf("%s: %s\n", convName, convMsg);
    fflush(stdout);
    pthread_mutex_unlock(stdoutLock);
    free(convMsg);
    free(convName);
}

/**
 *  Handles receiving KICK:NAME - increments the client's stat, logs received,
 *  and sends KICK: to the kicked client.
 *  Params:
 *      node: the node of the client that sent the KICK: message
 *      clientsLock: clients' linked list lock
 *      received: received stats
 *      receivedLock: received stats lock
 *      clients: clients' linked list root node
 *      name: name of the client to kick.
 **/
void kick(ClientNode *node, pthread_mutex_t *clientsLock, 
        pthread_mutex_t *stdoutLock, Received *received, 
        pthread_mutex_t *receivedLock, ClientNode *clients, char *name) {
    inc_stat(node, 'k', clientsLock);
    log_received(received, receivedLock, KICK);

    if (strlen(name) == 0) {
        return;
    }

    ClientNode *kickedNode = get_client_node(clients, name, clientsLock);
    if (kickedNode != 0) {
        send_r(kickedNode->data.to, "KICK:\n", kickedNode->data.toLock);
    }
}

/**
 *  Handles receiving LIST: - increments the client's stat, logs received,
 *  and sends LIST:name1,name2,... back to the client.
 *  Params:
 *      node: the node of the client that send the LIST: message
 *      clientsLock: clients' linked list lock
 *      received: received stats
 *      receivedLock: received stats lock
 *      clients: clients' linked list root node
 **/
void handle_list(ClientNode *node, pthread_mutex_t *clientsLock,
        Received *received, pthread_mutex_t *receivedLock, 
        ClientNode *clients) {
    // update stats
    inc_stat(node, 'l', clientsLock);
    log_received(received, receivedLock, LIST);

    char *names = get_names_list(clients, clientsLock);
    char *conv = convert_unprintable(names);
   
    // allocate total cmd length =
    // strlen("LIST:") + strlen(names) + strlen("\n\0") = 
    // 5 + strlen(names) + 2
    char *cmd = (char*)malloc(sizeof(char) * (strlen(names) + 7));
    sprintf(cmd, "LIST:%s\n", conv);
    send_to_all(clients, cmd, clientsLock);

    free(names);
    free(cmd);
    free(conv);
}

/**
 *  Sig handler thread routine. Ignores SIGPIPE. Upon receiving SIGHUP, 
 *  prints out a table of stats for messages received by server and sent by 
 *  clients.
 *  Params:
 *      tempArg: pointer to SigHandlerArg
 *  Returns: NULL
 **/
void *show_stats(void *tempArg) {
    SigHandlerArg *arg = (SigHandlerArg*)tempArg;
    int sig;
    
    while (1) {
        sigwait(arg->set, &sig);
        if (sig == SIGPIPE) {
            continue;
        }
        fputs("@CLIENTS@\n", stderr);
        show_clients_stats(arg->clients, arg->clientsLock);
        
        fputs("@SERVER@\n", stderr);
        pthread_mutex_lock(arg->receivedLock);
        
        fprintf(stderr, "server:AUTH:%d:NAME:%d:SAY:%d:KICK:%d:LIST:%d:LEAVE:"
                "%d\n", arg->received->auth, arg->received->name, 
                arg->received->say, arg->received->kick, arg->received->list,
                arg->received->leave);
        
        pthread_mutex_unlock(arg->receivedLock);
    }

    return 0;
}

/**
 *  Client handling thread routine. Handles communication with the client.
 *  Params:
 *      tempArg: HandleClientArg containing received stats, clients list, 
 *      locks, auth string, and file descriptor.
 *  Returns: NULL
 **/
void *handle_client(void *tempArg) {
    pthread_mutex_t toLock; // initialise write stream lock
    pthread_mutex_init(&toLock, 0);
    HandleClientArg *arg = (HandleClientArg*)tempArg;
    FILE *from = fdopen(dup(*(arg->clientFd)), "r");
    FILE *to = fdopen(*(arg->clientFd), "w");
    if (!authenticate_client(to, from, arg->auth, &toLock, arg->received, 
            arg->receivedLock)) {
        clean_up_client(to, from, &toLock, arg->clientFd, arg);
        return 0;
    }
    char *name;
    if (!(name = identify_client(to, from, &toLock, arg->received, 
            arg->receivedLock, arg->clients, arg->clientsLock))) {
        clean_up_client(to, from, &toLock, arg->clientFd, arg);
        return 0;
    }
    ClientNode *node = get_client_node(arg->clients, name, arg->clientsLock);
    send_enter(arg->clients, name, arg->clientsLock, arg->stdoutLock); 
    int readsBeforeEof;
    String line;
    bool left = false;
    while (!left) {
        line = get_line(from, &readsBeforeEof);
        if (readsBeforeEof > -1) {
            free(line.chars);
            break;
        }
        if (!strncmp(line.chars, "SAY:", 4)) {
            handle_say(node, arg->clientsLock, arg->received, 
                    arg->receivedLock, arg->clients, name, line.chars + 4,
                    arg->stdoutLock);
        } else if (!strncmp(line.chars, "KICK:", 5)) {
            kick(node, arg->clientsLock, arg->stdoutLock, arg->received, 
                    arg->receivedLock, arg->clients, line.chars + 5);
        } else if (!strncmp(line.chars, "LIST:", 5)) {
            handle_list(node, arg->clientsLock, arg->received, 
                    arg->receivedLock, arg->clients);
        } else if (!strncmp(line.chars, "LEAVE:", 6)) {
            log_received(arg->received, arg->receivedLock, LEAVE);
            left = true;
        }
        free(line.chars);
        usleep(100000);
    }
    delete_client(arg->clients, name, arg->clientsLock);
    send_leave(arg->clients, name, arg->clientsLock, arg->stdoutLock);
    free(name);
    clean_up_client(to, from, &toLock, arg->clientFd, arg);
    return 0;
}

/**
 *  Accepts clients' connections to the listening socket.
 *  Params:
 *      listenFd: the listening socket
 *      auth: auth string
 *      received:  received stats
 *      receivedLock: received lock
 *      clients: clients' linked list root node
 *      clientsLock: lock for clients' list
 **/
void accept_clients(int listenFd, char *auth, Received *received, 
        pthread_mutex_t *receivedLock, ClientNode *clients, 
        pthread_mutex_t *clientsLock) {
    struct sockaddr_in fromAddr;
    socklen_t fromAddrSize = sizeof(struct sockaddr_in);
    
    // initialise stdout lock
    pthread_mutex_t stdoutLock;
    pthread_mutex_init(&stdoutLock, 0);
    HandleClientArg *arg;

    while (1) {
        int *clientFd = (int*)malloc(sizeof(int));
        *clientFd = accept(listenFd, (struct sockaddr*)&fromAddr, 
                &fromAddrSize);
        if (*clientFd < 0) {
            free(clientFd);
            continue;
        }
        // set arg
        arg = (HandleClientArg*)malloc(sizeof(HandleClientArg));
        arg->clientFd = clientFd;
        arg->auth = auth;
        arg->received = received;
        arg->clients = clients;
        arg->receivedLock = receivedLock;
        arg->clientsLock = clientsLock;
        arg->stdoutLock = &stdoutLock;

        // create client handler thread
        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, arg);
        pthread_detach(tid);
    }    
}

int main(int argc, char **argv) {
    FILE *authFile;
    if (argc < 2 || !(authFile = fopen(argv[1], "r")) || argc > 3) {
        fputs("Usage: server authfile [port]\n", stderr);
        return 1;
    }
    int readsBeforeEof;
    String auth = get_line(authFile, &readsBeforeEof);
    fclose(authFile);

    int listenFd = argc == 2 ? get_listening_socket("0") : 
            get_listening_socket(argv[2]);

    // initialise received stats, clients list and locks
    Received *received = (Received*)calloc(1, sizeof(Received));
    ClientNode *clients = (ClientNode*)calloc(1, sizeof(ClientNode));
    pthread_mutex_t receivedLock;
    pthread_mutex_t clientsLock;
    pthread_mutex_init(&receivedLock, 0);
    pthread_mutex_init(&clientsLock, 0);
    // set up sig handler thread
    SigHandlerArg sigHandlerArg = {
            .received = received,
            .clients = clients,
            .receivedLock = &receivedLock,
            .clientsLock = &clientsLock
            };

    sigset_t set;
    pthread_t sigHandler;
    sigemptyset(&set);
    sigaddset(&set, SIGHUP);
    sigaddset(&set, SIGPIPE);
    pthread_sigmask(SIG_SETMASK, &set, NULL);
    sigHandlerArg.set = &set;
    pthread_create(&sigHandler, NULL, show_stats, &sigHandlerArg);

    accept_clients(listenFd, auth.chars, received, &receivedLock, clients,
            &clientsLock);
    return 0;
}
