#ifndef CLIENTLIB_H
#define CLIENTLIB_H

#include "dynstring.h"
#include <stdbool.h>

#define USAGE "Usage: client name authfile [host] port\n"
#define AUTHFILE_ERR_MSG "Authfile error\n"
#define COMM_ERR_MSG "Communications error\n"
#define AUTH_ERR_MSG "Authentication error\n"
#define KICKED_MSG "Kicked\n"

// Client exit codes
#define NO_ERR 0;
#define ARGS_ERR 1;
#define AUTHFILE_ERR 2;
#define COMM_ERR 3;
#define AUTH_ERR 4;
#define KICKED 5;

// The client's name as accepted by the server.
typedef struct
{
    char *name;
    int num;
} AcceptedName;

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

/**
 * Get args from argv.
 * @param args	Args struct, authdstr must be initialized.
 * @returns if successful, authfile not found or invalid args count
 */
enum GetArgsResult get_args( Args *args, int argc, char **argv );

/**
 * Connect to server at host, port.
 */
bool get_connection( const char *host, const char *port, ServerStreams *server );

/**
 * Auth handshake. First receive AUTH:, reply with AUTH:authstr, and receive OK: if handshake
 * success. Otherwise, repeat.
 */
bool negotiate_auth( const ServerStreams *server, const char *authstr, DynString *line );

/**
 * Name handshake. First receive WHO:, reply with NAME:chosen_name, receive OK: if handshake
 * success, or NAME_TAKEN: if name is taken. If name is taken then concatenate uint to the name and
 * increment until handshake succeeds.
 */
bool negotiate_name( const ServerStreams *server, AcceptedName *name );

#endif
