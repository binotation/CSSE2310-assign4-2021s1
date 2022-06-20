#include "clientlib.h"
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

    fclose(authfile);
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
    if( connect( server_fd, (struct sockaddr*)res->ai_addr, sizeof(struct sockaddr)))
    {
        freeaddrinfo( res );
        return GET_CONN_COMM_ERR;
    }
    int write_fd = dup( server_fd );
    server->read = fdopen( server_fd, "r" );
    server->write = fdopen( write_fd, "w" );
    freeaddrinfo( res );
    return GET_CONN_SUCCESS;
}

bool negotiate_auth( const ServerStreams *server, const char *authstr, DynString *line )
{
    enum ReadlineResult readline_res;
    bool authenticated = false;

    do
    {
        readline_res = dynstring_readline( line, server->read );
        if( readline_res == READLINE_ERROR || readline_res == READLINE_EOF ) return false;
        if( !strncmp( line->str, "AUTH:", 5 ) )
        {
            fprintf( server->write, "AUTH:%s\n", authstr );
            fflush( server->write );
            do
            {
                readline_res = dynstring_readline( line, server->read );
                if( readline_res == READLINE_ERROR || readline_res == READLINE_EOF ) return false;
                if( !strncmp( line->str, "AUTH:", 5 ) )
                {
                    fprintf( server->write, "AUTH:%s\n", authstr );
                    fflush( server->write );
                }
                else if( !strncmp(line->str, "OK:", 3) ) authenticated = true;
            } while( !authenticated );
        }
    } while( !authenticated );

    return true;
}

void send_name( FILE *write, const ClientName *name )
{
    switch( name->num )
    {
        case -1:
            fprintf( write, "NAME:%s\n", name->name );
            break;
        default:
            fprintf( write, "NAME:%s%d\n", name->name, name->num );
    }
    fflush( write );
}

bool negotiate_name( const ServerStreams *server, ClientName *name, DynString *line )
{
    enum ReadlineResult readline_res;
    bool accepted = false;

    do
    {
        readline_res = dynstring_readline( line, server->read );
        if( readline_res == READLINE_ERROR || readline_res == READLINE_EOF ) return false;
        if( !strncmp( line->str, "WHO:", 4 ) )
        {
            send_name( server->write, name );
            do
            {
                readline_res = dynstring_readline( line, server->read );
                if( readline_res == READLINE_ERROR || readline_res == READLINE_EOF ) return false;
                if( !strncmp( line->str, "WHO:", 4 )) send_name( server->write, name );
                else if( !strncmp( line->str, "NAME_TAKEN:", 11 )) name->num++;
                else if( !strncmp( line->str, "OK:", 3 )) accepted = true;
            } while ( !accepted );
        }
    } while ( !accepted );

    return true;
}
