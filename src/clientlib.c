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

bool get_connection( const char *host, const char *port, ServerStreams *server )
{
    struct addrinfo *res;
    struct addrinfo hints;
    memset( &hints, 0, sizeof(struct addrinfo) );
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if( getaddrinfo( host, port, &hints, &res )) return false;
    int server_fd = socket( AF_INET, SOCK_STREAM, 0 );
    if( connect( server_fd, (struct sockaddr*)res->ai_addr, sizeof(struct sockaddr))) return false;
    int write_fd = dup( server_fd );
    server->read = fdopen( server_fd, "r" );
    server->write = fdopen( write_fd, "w" );
    freeaddrinfo( res );
    return true;
}
