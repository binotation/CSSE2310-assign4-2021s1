#include "serverlib.h"
#include <string.h>
#include <netdb.h>

void received_stats_init( ReceivedStats *received_stats )
{
    *received_stats = (ReceivedStats)
    {
        .auth	= 0,
        .name	= 0,
        .say	= 0,
        .kick	= 0,
        .list	= 0,
        .leave	= 0,
    };
    pthread_mutex_init( &received_stats->lock, 0 );
}

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

enum GetSockResult get_listening_socket( const char *port, int *sock_fd, unsigned short *port_int )
{
    struct addrinfo *res;
    struct addrinfo hints;
    memset( &hints, 0, sizeof(struct addrinfo) );
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Get address info
    if( getaddrinfo( NULL, port, &hints, &res )) return GET_SOCK_PORT_INVALID;
    *sock_fd = socket( AF_INET, SOCK_STREAM, 0 );

    // Bind socket to address and port
    if( bind( *sock_fd, (struct sockaddr*)res->ai_addr, sizeof(struct sockaddr) ))
    {
        freeaddrinfo( res );
        return GET_SOCK_COMM_ERR;
    }
    freeaddrinfo( res );

    // Identify port
    struct sockaddr_in ad;
    socklen_t len = sizeof(struct sockaddr_in);
    if( getsockname( *sock_fd, (struct sockaddr*)&ad, &len )) return GET_SOCK_COMM_ERR;
    *port_int = ntohs( ad.sin_port );

    // Begin listening
    if( listen( *sock_fd, 3 ) < 0 ) return GET_SOCK_COMM_ERR;

    return GET_SOCK_SUCCESS;
}
