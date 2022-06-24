#include "serverlib.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

// Length of each message type including ':'.
#define AUTH_LEN 5
#define NAME_LEN 5
#define SAY_LEN 4
#define KICK_LEN 5
#define LIST_LEN 5
#define LEAVE_LEN 6

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

void *print_stats_sig_handler( void *temp )
{
    SigHandlerArg *arg = temp;
    int sig;

    while(1)
    {
        sigwait( &arg->set, &sig );
        if( sig == SIGPIPE ) continue;
        else
        {
            // SIGHUP
            fputs( "@CLIENTS@\n", stderr );
            list_print_stats( arg->clients );

            fputs( "@SERVER@\n", stderr );
            pthread_mutex_lock( &arg->stats->lock );
            fprintf( stderr,
                "server:AUTH:%u:NAME:%u:SAY:%u:KICK:%u:LIST:%u:LEAVE:%u\n",
                arg->stats->auth,
                arg->stats->name,
                arg->stats->say,
                arg->stats->kick,
                arg->stats->list,
                arg->stats->leave
            );
            pthread_mutex_unlock( &arg->stats->lock );
        }
    }
    return 0;
}

static void update_stats( ReceivedStats *stats, enum ReceivedType type )
{
    pthread_mutex_lock( &stats->lock );
    switch( type )
    {
        case RECV_AUTH:
            stats->auth++;
            break;
        case RECV_NAME:
            stats->name++;
            break;
        case RECV_SAY:
            stats->say++;
            break;
        case RECV_KICK:
            stats->kick++;
            break;
        case RECV_LIST:
            stats->list++;
            break;
        case RECV_LEAVE:
            stats->leave++;
    }
    pthread_mutex_unlock( &stats->lock );
}

static inline void clean_up_client( ClientStreams *streams, DynString *name, DynString *line,
    void *arg )
{
    fclose( streams->tx );
    fclose( streams->rx );
    dynstring_destroy( name );
    dynstring_destroy( line );
    free( arg );
}

static inline void write_flush_safe( const char *str, FILE *tx, pthread_mutex_t *tx_lock )
{
    pthread_mutex_lock( tx_lock );
    fputs( str, tx );
    fflush( tx );
    pthread_mutex_unlock( tx_lock );
}

bool negotiate_auth( const DynString *authdstr, ClientStreams *streams, ReceivedStats *stats,
    DynString *line )
{
    enum ReadlineResult readline_res;
    write_flush_safe( "AUTH:\n", streams->tx, &streams->tx_lock );

    do
    {
        readline_res = dynstring_readline( line, streams->rx );
        if( readline_res == READLINE_ERROR || readline_res == READLINE_EOF ) return false;
        if( !strncmp( line->str, "AUTH:", AUTH_LEN ))
        {
            update_stats( stats, RECV_AUTH );
            if( !strcmp( line->str + AUTH_LEN, authdstr->str ))
            {
                write_flush_safe( "OK:\n", streams->tx, &streams->tx_lock );
                return true;
            }
            else
            {
                return false;
            }
        }
        write_flush_safe( "AUTH:\n", streams->tx, &streams->tx_lock );
    } while(1);
}

bool negotiate_name( DynString *name, ClientStreams *streams, ReceivedStats *stats, ClientList *clients,
    DynString *line )
{
    enum ReadlineResult readline_res;
    write_flush_safe( "WHO:\n", streams->tx, &streams->tx_lock );

    do
    {
        readline_res = dynstring_readline( line, streams->rx );
        if( readline_res == READLINE_ERROR || readline_res == READLINE_EOF ) return false;
        if( !strncmp( line->str, "NAME:", NAME_LEN ))
        {
            update_stats( stats, RECV_NAME );
            if( line->length == NAME_LEN || check_name_in_use( clients, line->str + NAME_LEN ))
            {
                write_flush_safe( "NAME_TAKEN:\n", streams->tx, &streams->tx_lock );
            }
            else
            {
                dynstring_npush( name, line->str + NAME_LEN, line->length - NAME_LEN );
                write_flush_safe( "OK:\n", streams->tx, &streams->tx_lock );
                return true;
            }
        }
        write_flush_safe( "WHO:\n", streams->tx, &streams->tx_lock );
    } while(1);
}

void *client_handler( void *temp )
{
    ClientHandlerArg *arg = temp;
    DynString line, name;
    dynstring_init( &line, 20 );
    dynstring_init( &name, 20 );

    // Initialize ClientStreams
    ClientStreams streams;
    streams.rx = fdopen( dup( arg->client_sock ), "r" );
    streams.tx = fdopen( arg->client_sock, "w" );
    pthread_mutex_init( &streams.tx_lock, 0 );

    // Negotiate authentication
    if( !negotiate_auth( arg->authdstr, &streams, arg->stats, &line ))
    {
        clean_up_client( &streams, &name, &line, arg );
        return 0;
    }

    // Negotiate name
    if( !negotiate_name( &name, &streams, arg->stats, arg->clients, &line ))
    {
        clean_up_client( &streams, &name, &line, arg );
        return 0;
    }

    // ClientNode *node = get_client_node(arg->clients, name, arg->clientsLock);
    // send_enter(arg->clients, name, arg->clientsLock, arg->stdoutLock); 
    // int readsBeforeEof;
    // String line;
    // bool left = false;
    // while (!left) {
    //     line = get_line(from, &readsBeforeEof);
    //     if (readsBeforeEof > -1) {
    //         free(line.chars);
    //         break;
    //     }
    //     if (!strncmp(line.chars, "SAY:", 4)) {
    //         handle_say(node, arg->clientsLock, arg->received, arg->receivedLock, arg->clients, name, line.chars + 4,
    //                 arg->stdoutLock);
    //     } else if (!strncmp(line.chars, "KICK:", 5)) {
    //         kick(node, arg->clientsLock, arg->stdoutLock, arg->received, arg->receivedLock, arg->clients, line.chars + 5);
    //     } else if (!strncmp(line.chars, "LIST:", 5)) {
    //         handle_list(node, arg->clientsLock, arg->received, arg->receivedLock, arg->clients);
    //     } else if (!strncmp(line.chars, "LEAVE:", 6)) {
    //         log_received(arg->received, arg->receivedLock, LEAVE);
    //         left = true;
    //     }
    //     free(line.chars);
    //     usleep(100000);
    // }
    // delete_client(arg->clients, name, arg->clientsLock);
    // send_leave(arg->clients, name, arg->clientsLock, arg->stdoutLock);

    clean_up_client( &streams, &name, &line, arg );
    return 0;
}
