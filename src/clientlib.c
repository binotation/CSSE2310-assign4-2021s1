#include "clientlib.h"

enum GetArgsResult get_args( Args *args, int argc, char **argv )
{
    FILE *authfile;
    if( argc != 4 && argc != 5 ) return GET_ARGS_INVALID_ARGS_COUNT;
    else if( !( authfile = fopen( argv[2], "r" ) )) return GET_ARGS_AUTHFILE_NOT_FOUND;

    args->chosen_name = argv[1];
    dynstring_readline( &args->authdstr, authfile );

    switch( argc )
    {
        case 4:
            args->host = "localhost";
            args->port = argv[3];
            break;
        case 5:
            args->host = argv[3];
            args->port = argv[4];
    }

    fclose(authfile);
    return GET_ARGS_SUCCESS;
}
