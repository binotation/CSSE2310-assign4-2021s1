#include "serverlib.h"
#include "util.h"
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
#define ENTER_LEN 6
#define MSG_LEN 4

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
    if( listen( *sock_fd, 100 ) < 0 ) return GET_SOCK_COMM_ERR;

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

/**
 * Update received stats.
 * @param stats	received stats struct
 * @param type	which stat to update
 */
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

/**
 * Free allocated memory for client: streams, name dynstring, line dynstring, and arg from main
 * thread.
 */
static void clean_up_client( ClientStreams *streams, DynString *name, DynString *line,
    void *arg )
{
    pthread_mutex_lock( &streams->tx_lock );
    fclose( streams->tx );
    pthread_mutex_unlock( &streams->tx_lock );
    fclose( streams->rx );
    dynstring_destroy( name );
    dynstring_destroy( line );
    free( arg );
}

/**
 * Lock stream mutex, write, flush, unlock.
 */
static void write_flush_safe( const char *str, FILE *tx, pthread_mutex_t *tx_lock )
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

ListNode *negotiate_name( DynString *name, ClientStreams *streams, ReceivedStats *stats, ClientList *clients,
    DynString *line )
{
    enum ReadlineResult readline_res;
    ListNode *client_node = list_node_init( name, streams->tx, &streams->tx_lock );
    write_flush_safe( "WHO:\n", streams->tx, &streams->tx_lock );

    do
    {
        readline_res = dynstring_readline( line, streams->rx );
        if( readline_res == READLINE_ERROR || readline_res == READLINE_EOF )
        {
            free( client_node );
            return NULL;
        }
        if( !strncmp( line->str, "NAME:", NAME_LEN ))
        {
            update_stats( stats, RECV_NAME );
            dynstring_clear( name );
            dynstring_npush( name, line->str + NAME_LEN, line->length - NAME_LEN );
            if( line->length == NAME_LEN || !list_insert( clients, client_node ))
            {
                write_flush_safe( "NAME_TAKEN:\n", streams->tx, &streams->tx_lock );
            }
            else
            {
                write_flush_safe( "OK:\n", streams->tx, &streams->tx_lock );
                return client_node;
            }
        }
        write_flush_safe( "WHO:\n", streams->tx, &streams->tx_lock );
    } while(1);
}

/**
 * Send ENTER:name to all clients. Print who has entered to stdout.
 */
static void send_enter( ClientList *clients, const DynString *name, pthread_mutex_t *stdout_lock )
{
    // strlen( "ENTER:" ) + strlen( name ) + strlen( "\n\0" )
    char *cmd = malloc( sizeof(char) * ( ENTER_LEN + name->length + 2 ));
    sprintf( cmd, "ENTER:%s\n", name->str );
    list_send_to_all( clients, cmd );
    free( cmd );

    pthread_mutex_lock( stdout_lock );
    printf( "(%s has entered the chat)\n", name->str );
    fflush( stdout );
    pthread_mutex_unlock( stdout_lock );
}

/**
 * Send LEAVE:name to all clients. Print who has left to stdout.
 */
static void send_leave( ClientList *clients, const DynString *name, pthread_mutex_t *stdout_lock )
{
    // strlen( "LEAVE:" ) + strlen( name ) + strlen( "\n\0" )
    char *cmd = malloc( sizeof(char) * ( LEAVE_LEN + name->length + 2 ));
    sprintf( cmd, "LEAVE:%s\n", name->str );
    list_send_to_all( clients, cmd );
    free( cmd );

    pthread_mutex_lock( stdout_lock );
    printf("(%s has left the chat)\n", name->str);
    fflush( stdout );
    pthread_mutex_unlock( stdout_lock );
}

/**
 * Update stats and send MSG:name:message to all clients. Print message to stdout.
 */
static void handle_say( ListNode *client, ClientList *clients, ReceivedStats *stats,
    const DynString *name, DynString *line, pthread_mutex_t *stdout_lock )
{
    list_inc_stat( client, 's' );
    update_stats( stats, RECV_SAY );

    char *msg = line->str + SAY_LEN;
    unsigned int msg_length = line->length - SAY_LEN;
    replace_unprintable( msg );

    // Construct cmd to send
    // strlen( "MSG:" ) + strlen( name + ':' ) + strlen( message ) + strlen( "\n\0" )
    char *cmd = malloc( sizeof(char) * ( MSG_LEN + name->length + 1 + msg_length + 2 ));
    sprintf( cmd, "MSG:%s:%s\n", name->str, msg );
    list_send_to_all( clients, cmd );
    free( cmd );

    pthread_mutex_lock( stdout_lock );
    printf( "%s: %s\n", name->str, msg );
    fflush( stdout );
    pthread_mutex_unlock( stdout_lock );
}

/**
 * Update stats and kick target client by sending KICK: to them.
 */
static void kick( ListNode *client, ClientList *clients, ReceivedStats *stats, DynString *line )
{
    list_inc_stat( client, 'k' );
    update_stats( stats, RECV_KICK );

    char *name = line->str + KICK_LEN;
    list_send_to_node( clients, name, "KICK:\n" );
}

/**
 * Update stats and send LIST:list_of_names to asking client.
 */
static void handle_list( ListNode *client, ClientList *clients, ReceivedStats *stats )
{
    list_inc_stat( client, 'l' );
    update_stats( stats, RECV_LIST );

    DynString names;
    dynstring_init( &names, 20 );
    list_get_names_list( clients, &names );

    pthread_mutex_lock( client->data.tx_lock );
    fprintf( client->data.tx, "LIST:%s\n", names.str );
    fflush( client->data.tx );
    pthread_mutex_unlock( client->data.tx_lock );

    dynstring_destroy( &names );
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
    ListNode *client_node;
    if(( client_node = negotiate_name( &name, &streams, arg->stats, arg->clients, &line )) == NULL )
    {
        clean_up_client( &streams, &name, &line, arg );
        return 0;
    }
    replace_unprintable( name.str );
    send_enter( arg->clients, &name, arg->stdout_lock );

    enum ReadlineResult readline_res;
    bool left = false;
    while( !left )
    {
        readline_res = dynstring_readline( &line, streams.rx );
        if( readline_res == READLINE_ERROR || readline_res == READLINE_EOF ) break;

        if( !strncmp( line.str, "SAY:", SAY_LEN ))
        {
            handle_say( client_node, arg->clients, arg->stats, &name, &line, arg->stdout_lock );
        }
        else if( !strncmp( line.str, "KICK:", KICK_LEN ))
        {
            kick( client_node, arg->clients, arg->stats, &line );
        }
        else if( !strncmp( line.str, "LIST:", LIST_LEN ))
        {
            handle_list( client_node, arg->clients, arg->stats );
        }
        else if( !strncmp( line.str, "LEAVE:", LEAVE_LEN ))
        {
            update_stats( arg->stats, RECV_LEAVE );
            left = true;
        }
    }

    send_leave( arg->clients, &name, arg->stdout_lock );
    list_delete( arg->clients, name.str );
    clean_up_client( &streams, &name, &line, arg );
    return 0;
}
