#ifndef CLIENTLIB_H
#define CLIENTLIB_H

#include "dynstring.h"
#include <stdbool.h>

#define USAGE "Usage: client name authfile [host] port\n"
#define AUTHFILE_ERR_MSG "Authfile error\n"
#define HOST_ERR_MSG "Host/port invalid\n"
#define COMM_ERR_MSG "Communications error\n"
#define AUTH_ERR_MSG "Authentication error\n"
#define KICKED_MSG "Kicked\n"

// Client exit codes
#define NO_ERR 0
#define ARGS_ERR 1
#define AUTHFILE_ERR 2
#define HOST_ERR 3
#define COMM_ERR 4
#define AUTH_ERR 5
#define KICKED 6

#define CLIENTNAME_INIT( var, chosen_name ) ClientName var = { .name = chosen_name, .num = -1 }

// The client's name as accepted by the server.
typedef struct
{
    char *name;
    int num;
} ClientName;

// User args
typedef struct
{
    char *chosen_name;
    DynString authdstr;
    char *host;
    char *port;
} Args;

// Server read/write streams.
typedef struct
{
    FILE *read;
    FILE *write;
} ServerStreams;

enum GetArgsResult
{
    GET_ARGS_SUCCESS,
    GET_ARGS_AUTHFILE_NOT_FOUND,
    GET_ARGS_INVALID_ARGS_COUNT,
};

enum GetConnResult
{
    GET_CONN_SUCCESS,
    GET_CONN_HOST_INVALID,
    GET_CONN_COMM_ERR,
};

/**
 * Get args from argv.
 * @param args	Args struct, authdstr must be initialized.
 * @returns if successful, authfile not found or invalid args count
 */
enum GetArgsResult get_args( Args *args, int argc, char **argv );

/**
 * Connect to server at host, port.
 */
enum GetConnResult get_connection( const char *host, const char *port, ServerStreams *server );

/**
 * Auth handshake. First receive AUTH:, reply with AUTH:authstr, and receive OK: if handshake
 * success. Otherwise, repeat.
 */
bool negotiate_auth( const ServerStreams *server, const char *authstr, DynString *line );

/**
 * Send client name over write stream.
 */
void send_name( FILE *write, const ClientName *name );

/**
 * Name handshake. First receive WHO:, reply with NAME:chosen_name, receive OK: if handshake
 * success, or NAME_TAKEN: if name is taken. If name is taken then concatenate uint to the name and
 * increment until handshake succeeds.
 */
bool negotiate_name( const ServerStreams *server, ClientName *name, DynString *line );

/**
 * Thread routine to handle server communication.
 */
void *handle_server_comm( void *arg );

#endif
