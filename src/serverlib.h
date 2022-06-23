#ifndef SERVERLIB_H
#define SERVERLIB_H

#include "dynstring.h"

#define USAGE "Usage: server authfile [port]\n"
#define AUTHFILE_ERR_MSG "Authfile error\n"
#define PORT_ERR_MSG "Port invalid\n"
#define COMM_ERR_MSG "Communications error\n"

// Server exit codes
#define NO_ERR 0
#define ARGS_ERR 1
#define AUTHFILE_ERR 2
#define PORT_ERR 3
#define COMM_ERR 4

// User args
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

#endif
