#include "serverlib.h"
#include "list.h"
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>

/**
 *  Sends ENTER:name to all clients and prints (name has entered the chat) to
 *  stdout.
 *  Params: 
 *      clients: clients' linked list
 *      name: client's name that entered
 *      clientsLock: lock for clients' linked list
 *      stdoutLock: lock for stdout
 **/
// void send_enter(ClientNode *clients, const char *name, pthread_mutex_t *clientsLock, pthread_mutex_t *stdoutLock) {
//     // allocate total cmd length = 
//     // strlen("ENTER:") + strlen(name) + strlen("\n\0") = strlen(name) + 6 + 2
//     char *cmd = (char*)malloc(sizeof(char) * (strlen(name) + 8));

//     // send to all
//     char *conv = convert_unprintable(name);
//     sprintf(cmd, "ENTER:%s\n", conv);
//     send_to_all(clients, cmd, clientsLock);
//     free(cmd);

//     // print to stdout
//     pthread_mutex_lock(stdoutLock);
//     printf("(%s has entered the chat)\n", conv);
//     fflush(stdout);
//     pthread_mutex_unlock(stdoutLock);
//     free(conv);
// }

/**
 *  Sends LEAVE:name to all clients and prints (name has left the chat) to 
 *  stdout.
 *  Params:
 *      clients: clients' linked list
 *      name: client's name that left
 *      clientsLock: lock for clients' linked list
 *      stdoutLock: lock for stdout
 **/
// void send_leave(ClientNode *clients, const char *name, pthread_mutex_t *clientsLock, pthread_mutex_t *stdoutLock) {
//     // allocate total cmd length = 
//     // strlen("LEAVE:") + strlen(name) + strlen("\n\0") = strlen(name) + 6 + 2
//     char *cmd = (char*)malloc(sizeof(char) * (strlen(name) + 8));
    
//     // send to all
//     char *conv = convert_unprintable(name);
//     sprintf(cmd, "LEAVE:%s\n", conv);
//     send_to_all(clients, cmd, clientsLock);
//     free(cmd);

//     // print to stdout
//     pthread_mutex_lock(stdoutLock);
//     printf("(%s has left the chat)\n", conv);
//     fflush(stdout);
//     pthread_mutex_unlock(stdoutLock);
//     free(conv);
// }

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
// void handle_say(ClientNode *node, pthread_mutex_t *clientsLock, Received *received, pthread_mutex_t *receivedLock,
//         ClientNode *clients, const char *name, const char *msg, pthread_mutex_t *stdoutLock) {
//     // update stats
//     inc_stat(node, 's', clientsLock);
//     log_received(received, receivedLock, SAY);

//     // allocate total cmd length = 
//     // length("MSG:") + length(name) +  length(":") + length(message) + 
//     // length("\n\0") = 4 + length(name) + 1 + length(message) + 2
//     int cmdLength = strlen(name) + strlen(msg) + 7;
//     char *cmd = (char*)malloc(sizeof(char) * cmdLength);

//     char *convName = convert_unprintable(name);
//     char *convMsg = convert_unprintable(msg);
//     sprintf(cmd, "MSG:%s:%s\n", convName, convMsg);
//     send_to_all(clients, cmd, clientsLock);
    
//     free(cmd);

//     pthread_mutex_lock(stdoutLock);
//     printf("%s: %s\n", convName, convMsg);
//     fflush(stdout);
//     pthread_mutex_unlock(stdoutLock);
//     free(convMsg);
//     free(convName);
// }

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
// void kick(ClientNode *node, pthread_mutex_t *clientsLock, pthread_mutex_t *stdoutLock, Received *received, 
//         pthread_mutex_t *receivedLock, ClientNode *clients, char *name) {
//     inc_stat(node, 'k', clientsLock);
//     log_received(received, receivedLock, KICK);

//     if (strlen(name) == 0) {
//         return;
//     }

//     ClientNode *kickedNode = get_client_node(clients, name, clientsLock);
//     if (kickedNode != 0) {
//         send_r(kickedNode->data.to, "KICK:\n", kickedNode->data.toLock);
//     }
// }

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
// void handle_list(ClientNode *node, pthread_mutex_t *clientsLock, Received *received, pthread_mutex_t *receivedLock, 
//         ClientNode *clients) {
//     // update stats
//     inc_stat(node, 'l', clientsLock);
//     log_received(received, receivedLock, LIST);

//     char *names = get_names_list(clients, clientsLock);
//     char *conv = convert_unprintable(names);
   
//     // allocate total cmd length =
//     // strlen("LIST:") + strlen(names) + strlen("\n\0") = 
//     // 5 + strlen(names) + 2
//     char *cmd = (char*)malloc(sizeof(char) * (strlen(names) + 7));
//     sprintf(cmd, "LIST:%s\n", conv);
//     send_to_all(clients, cmd, clientsLock);

//     free(names);
//     free(cmd);
//     free(conv);
// }

int main( int argc, char **argv )
{
    // Get args
    Args args;
    dynstring_init( &args.authdstr, 16  );
    enum GetArgsResult get_args_res = get_args( &args, argc, argv );
    switch( get_args_res )
    {
        case GET_ARGS_INVALID_ARGS_COUNT:
            fputs( USAGE, stderr );
            return ARGS_ERR;
        case GET_ARGS_AUTHFILE_NOT_FOUND:
            fputs( AUTHFILE_ERR_MSG, stderr );
            return AUTHFILE_ERR;
        case GET_ARGS_SUCCESS: break;
    }

    // Listen on port
    int listen_sock;
    unsigned short port;
    enum GetSockResult get_sock_res = get_listening_socket( args.port, &listen_sock, &port );
    switch( get_sock_res )
    {
        case GET_SOCK_PORT_INVALID:
            fputs( PORT_ERR_MSG, stderr );
            return PORT_ERR;
        case GET_SOCK_COMM_ERR:
            fputs( COMM_ERR_MSG, stderr );
            return COMM_ERR;
        case GET_SOCK_SUCCESS:
            fprintf( stderr, "%u\n", port );
    }

    // Initialize clients list and received stats
    ClientList clients;
    ReceivedStats stats;
    list_init( &clients );
    received_stats_init( &stats );

    // Set up signal handler thread
    pthread_t sig_handler;
    SigHandlerArg sig_handler_arg;

    sig_handler_arg.stats = &stats;
    sig_handler_arg.clients = &clients;

    sigemptyset( &sig_handler_arg.set );
    sigaddset( &sig_handler_arg.set, SIGHUP );
    sigaddset( &sig_handler_arg.set, SIGPIPE );
    // Block SIGHUP, SIGPIPE in this thread (main thread)
    pthread_sigmask( SIG_SETMASK, &sig_handler_arg.set, NULL );

    // Create signal handler thread
    pthread_create( &sig_handler, NULL, print_stats_sig_handler, &sig_handler_arg );

    // Start accepting clients
    pthread_mutex_t stdout_lock;
    pthread_mutex_init( &stdout_lock, 0 );

    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(struct sockaddr_in);

    int client_sock;
    ClientHandlerArg *client_handler_arg;
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init( &attr );
    pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );

    while(1)
    {
        client_sock = accept( listen_sock, (struct sockaddr*)&client_addr, &client_addr_size );
        if( client_sock < 0 ) continue;

        client_handler_arg = malloc( sizeof(ClientHandlerArg) );
        *client_handler_arg = (ClientHandlerArg)
        {
            .client_sock = client_sock,
            .authdstr = &args.authdstr,
            .stats = &stats,
            .clients = &clients,
            .stdout_lock = &stdout_lock,
        };

        // Create client handler thread
        pthread_create( &tid, &attr, client_handler, client_handler_arg );
    }

    pthread_cancel( sig_handler );
    pthread_join( sig_handler, 0 );
    dynstring_destroy( &args.authdstr );
    return NO_ERR;
}
