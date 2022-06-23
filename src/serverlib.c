#include "serverlib.h"

enum GetArgsResult get_args( Args *args, int argc, char **argv )
{
    FILE *authfile;
    if( argc != 2 && argc != 3 ) return GET_ARGS_INVALID_ARGS_COUNT;
    else if( !( authfile = fopen( argv[1], "r" ) )) return GET_ARGS_AUTHFILE_NOT_FOUND;

    dynstring_readline( &args->authdstr, authfile );

    switch( argc )
    {
        case 2:
            args->port = "0";
            break;
        case 3:
            args->port = argv[2];
    }

    fclose( authfile );
    return GET_ARGS_SUCCESS;
}
