#ifndef CLIENTLIB_H
#define CLIENTLIB_H

#include "dynstring.h"

#define USAGE "Usage: client name authfile [host] port\n"

// Client return values
#define NO_ERR 0;
#define ARGS_ERR 1;
#define COMM_ERR 2;
#define KICKED 3;
#define AUTH_ERR 4;

// The client's name as accepted by the server.
typedef struct
{
    char *name;
    int num;
} AcceptedName;

// Parsed args.
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

enum ParseArgsResult
{
    PARSE_ARGS_SUCCESS,
    PARSE_ARGS_AUTHFILE_NOT_FOUND,
    PARSE_ARGS_INVALID_ARGS_COUNT,
};

/**
 * Parse argv into args.
 * @param args	Args struct, authdstr must be initialized.
 * @returns if successful, authfile not found or invalid args count
 */
enum ParseArgsResult parse_args( Args *args, int argc, char **argv );

#endif
