#include "clientlib.h"
#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

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

    fclose( authfile );
    return GET_ARGS_SUCCESS;
}

enum GetConnResult get_connection( const char *host, const char *port, ServerStreams *server )
{
    struct addrinfo *res;
    struct addrinfo hints;
    memset( &hints, 0, sizeof(struct addrinfo) );
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if( getaddrinfo( host, port, &hints, &res )) return GET_CONN_HOST_INVALID;
    int server_fd = socket( AF_INET, SOCK_STREAM, 0 );
    if( connect( server_fd, (struct sockaddr*)res->ai_addr, sizeof(struct sockaddr) ))
    {
        freeaddrinfo( res );
        return GET_CONN_COMM_ERR;
    }
    int write_fd = dup( server_fd );
    server->rx = fdopen( server_fd, "r" );
    server->tx = fdopen( write_fd, "w" );
    freeaddrinfo( res );
    return GET_CONN_SUCCESS;
}

bool negotiate_auth( const ServerStreams *server, const char *authstr, DynString *line )
{
    enum ReadlineResult readline_res;
    bool authenticated = false;

    do
    {
        readline_res = dynstring_readline( line, server->rx );
        if( readline_res == READLINE_ERROR || readline_res == READLINE_EOF ) return false;
        if( !strncmp( line->str, "AUTH:", 5 ))
        {
            fprintf( server->tx, "AUTH:%s\n", authstr );
            fflush( server->tx );
            do
            {
                readline_res = dynstring_readline( line, server->rx );
                if( readline_res == READLINE_ERROR || readline_res == READLINE_EOF ) return false;
                if( !strncmp( line->str, "AUTH:", 5 ))
                {
                    fprintf( server->tx, "AUTH:%s\n", authstr );
                    fflush( server->tx );
                }
                else if( !strncmp(line->str, "OK:", 3 ))
                {
                    authenticated = true;
                }
            } while( !authenticated );
        }
    } while( !authenticated );

    return true;
}

/**
 * Send NAME:client_name%d to the server.
 */
static void send_name( FILE *tx, const ClientName *name )
{
    switch( name->num )
    {
        case -1:
            fprintf( tx, "NAME:%s\n", name->name );
            break;
        default:
            fprintf( tx, "NAME:%s%d\n", name->name, name->num );
    }
    fflush( tx );
}

bool negotiate_name( const ServerStreams *server, ClientName *name, DynString *line )
{
    enum ReadlineResult readline_res;
    bool accepted = false;

    do
    {
        readline_res = dynstring_readline( line, server->rx );
        if( readline_res == READLINE_ERROR || readline_res == READLINE_EOF ) return false;
        if( !strncmp( line->str, "WHO:", 4 ))
        {
            send_name( server->tx, name );
            do
            {
                readline_res = dynstring_readline( line, server->rx );
                if( readline_res == READLINE_ERROR || readline_res == READLINE_EOF ) return false;
                if( !strncmp( line->str, "WHO:", 4 ))
                {
                    send_name( server->tx, name );
                }
                else if( !strncmp( line->str, "NAME_TAKEN:", 11 ))
                {
                    name->num++;
                }
                else if( !strncmp( line->str, "OK:", 3 ))
                {
                    accepted = true;
                }
            } while ( !accepted );
        }
    } while ( !accepted );

    return true;
}

/**
 * Print "name: message" from arg1 and arg2 of targs.
 */
static void handle_msg( TwoArgs *targs )
{
    fwrite( targs->arg1.str, targs->arg1.length, sizeof(char), stdout );
    fwrite( ": ", 2, sizeof(char), stdout );
    fwrite( targs->arg2.str, targs->arg2.length, sizeof(char), stdout );
    fwrite( "\n", 1, sizeof(char), stdout );
}

void *handle_server_comm( void *arg )
{
    FILE *rx = (FILE*)arg;
    DynString line;
    dynstring_init( &line, 20 );
    enum ReadlineResult readline_res;
    TwoArgs targs;
    int exit_status = 0;

    while ( exit_status == 0 )
    {
        readline_res = dynstring_readline( &line, rx );
        if( readline_res == READLINE_ERROR || readline_res == READLINE_EOF )
        {
            exit_status = COMM_ERR;
        }
        else if( line.length > 6 && !strncmp( line.str, "ENTER:", 6 ))
        {
            printf( "(%s has entered the chat)\n", line.str + 6 );
        }
        else if( line.length > 6 && !strncmp( line.str, "LEAVE:", 6 ))
        {
            printf( "(%s has left the chat)\n", line.str + 6 );
        }
        else if( !strncmp(line.str, "MSG:", 4) && get_two_args( line.str, &targs ))
        {
            handle_msg( &targs );
        }
        else if(!strncmp(line.str, "KICK:", 5 ))
        {
            exit_status = KICKED;
        }
        else if( line.length > 5 && !strncmp( line.str, "LIST:", 5 ))
        {
            printf( "(current chatters: %s)\n", line.str + 5 );
        }
        fflush(stdout);
    }
    dynstring_destroy( &line );
    switch( exit_status )
    {
        case COMM_ERR:
            fputs(COMM_ERR_MSG, stderr);
            break;
        case KICKED:
            fputs(KICKED_MSG, stderr);
    }
    exit( exit_status );
}
