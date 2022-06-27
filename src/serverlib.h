#ifndef SERVERLIB_H
#define SERVERLIB_H

#include "dynstring.h"
#include "list.h"
#include <stdbool.h>
#include <pthread.h>
#include <signal.h>

// Error messages
#define USAGE "Usage: server authfile [port]\n"
#define AUTHFILE_ERR_MSG "Authfile error\n"
#define PORT_ERR_MSG "Port invalid\n"
#define COMM_ERR_MSG "Communications error\n"

// Exit codes
#define NO_ERR 0
#define ARGS_ERR 1
#define AUTHFILE_ERR 2
#define PORT_ERR 3
#define COMM_ERR 4

/**
 * User args
 * @param .authdstr	Auth string as dynstring
 * @param .port		port as string
 */
typedef struct
{
    DynString authdstr;
    char *port;
} Args;

enum GetArgsResult
{
    GET_ARGS_SUCCESS,
    GET_ARGS_AUTHFILE_NOT_FOUND,
    GET_ARGS_INVALID_ARGS_COUNT,
};

enum GetSockResult
{
    GET_SOCK_SUCCESS,
    GET_SOCK_PORT_INVALID,
    GET_SOCK_COMM_ERR,
};

/**
 * Number of times each command has been received. Singleton. This struct is read/write shared
 * between threads.
 */
typedef struct
{
    pthread_mutex_t lock;
    unsigned int auth;
    unsigned int name;
    unsigned int say;
    unsigned int kick;
    unsigned int list;
    unsigned int leave;
} ReceivedStats;

// Type of command received.
enum ReceivedType
{
    RECV_AUTH,
    RECV_NAME,
    RECV_SAY,
    RECV_KICK,
    RECV_LIST,
    RECV_LEAVE,
};

/**
 * Arg for signal handler routine.
 */
typedef struct
{
    ReceivedStats *stats;
    ClientList *clients;
    sigset_t set; // For which signals to handle
} SigHandlerArg;

/**
 * Arg for client handler routine.
 */
typedef struct
{
    int client_sock;
    const DynString *authdstr;
    ReceivedStats *stats;
    ClientList *clients;
    pthread_mutex_t *stdout_lock;
} ClientHandlerArg;

/**
 * Client read/write streams. Read/write shared between client_handler threads.
 */
typedef struct
{
    FILE *rx;
    FILE *tx;
    pthread_mutex_t tx_lock;
} ClientStreams;

/**
 * Initialize ReceivedStats.
 */
void received_stats_init( ReceivedStats *received_stats );

/**
 * Get args from argv.
 * @param args	Args struct, authdstr must be initialized.
 * @returns if successful, authfile not found or invalid args count
 */
enum GetArgsResult get_args( Args *args, int argc, char **argv );

/**
 * Get listening socket in sock_fd and listening port in port_int.
 * @returns	if success, invalid port or other error e.g. port in use.
 */
enum GetSockResult get_listening_socket( const char *port, int *sock_fd, unsigned short *port_int );

/**
 * Signal handler thread routine. SIGHUP and SIGPIPE are blocked in the main thread
 * and handled here instead. SIGPIPE should be ignored and SIGHUP should trigger
 * the received stats to be printed to stderr. This is the only context where
 * stderr can be written to.
 */
void *print_stats_sig_handler( void *temp );

/**
 * Auth handshake. See architecture.md.
 */
bool negotiate_auth( const DynString *authdstr, ClientStreams *streams, ReceivedStats *stats,
    DynString *line );

/**
 * Name handshake. See architecture.md.
 */
ListNode *negotiate_name( DynString *name, ClientStreams *streams, ReceivedStats *stats, ClientList *clients,
    DynString *line );

/**
 * Thread routine for handling communication with clients.
 */
void *client_handler( void *temp );

#endif
