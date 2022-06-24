#include "clientlib.h"
#include <string.h>
#include <pthread.h>

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

    // Connect to server
    ServerStreams server;
    enum GetConnResult get_conn_res = get_connection( args.host, args.port, &server );
    switch( get_conn_res )
    {
        case GET_CONN_HOST_INVALID:
            fputs( HOST_ERR_MSG, stderr );
            return HOST_ERR;
        case GET_CONN_COMM_ERR:
            fputs( COMM_ERR_MSG, stderr );
            return COMM_ERR;
        case GET_CONN_SUCCESS: break;
    }

    DynString line;
    dynstring_init( &line, 20 );

    // Authentication handshake
    if( !negotiate_auth( &server, args.authdstr.str, &line ))
    {
        fputs( AUTH_ERR_MSG, stderr );
        return AUTH_ERR;
    }
    dynstring_destroy( &args.authdstr );

    CLIENTNAME_INIT( name, args.chosen_name );
    if( !negotiate_name( &server, &name, &line ))
    {
        fputs( COMM_ERR_MSG, stderr );
        return COMM_ERR;
    }

    bool exited = false;
    pthread_t server_handler;
    pthread_create( &server_handler, 0, handle_server_comm, (void*)server.read );

    enum ReadlineResult readline_res;

    do {
        readline_res = dynstring_readline( &line, stdin );
        if( readline_res == READLINE_ERROR || readline_res == READLINE_EOF ) break;
        if ( line.str[0] == '*' )
        {
            fprintf( server.write, "%s\n", line.str + 1 );
            if ( !strncmp( line.str + 1, "LEAVE:", 6 )) exited = true;
        }
        else
        {
            fprintf( server.write, "SAY:%s\n", line.str );
        }
        fflush(server.write);
    } while ( !exited );

    pthread_cancel( server_handler );
    pthread_join( server_handler, 0 );

    dynstring_destroy( &line );
    fclose( server.read );
    fclose( server.write );

    return NO_ERR;
}
