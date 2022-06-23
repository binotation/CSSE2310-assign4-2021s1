#ifndef SERVERLIB_H
#define SERVERLIB_H

#include "dynstring.h"

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

/**
 * Get args from argv.
 * @param args	Args struct, authdstr must be initialized.
 * @returns if successful, authfile not found or invalid args count
 */
enum GetArgsResult get_args( Args *args, int argc, char **argv );

#endif
