#include "serverlib.h"
#include "list.h"
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>

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

    // Listen on port
    int listen_sock;
    unsigned short port;
    enum GetSockResult get_sock_res = get_listening_socket( args.port, &listen_sock, &port );
    switch( get_sock_res )
    {
        case GET_SOCK_PORT_INVALID:
            fputs( PORT_ERR_MSG, stderr );
            return PORT_ERR;
        case GET_SOCK_COMM_ERR:
            fputs( COMM_ERR_MSG, stderr );
            return COMM_ERR;
        case GET_SOCK_SUCCESS:
            fprintf( stderr, "%u\n", port );
    }

    // Initialize clients list and received stats
    ClientList clients;
    ReceivedStats stats;
    list_init( &clients );
    received_stats_init( &stats );

    // Set up signal handler thread
    pthread_t sig_handler;
    SigHandlerArg sig_handler_arg;

    sig_handler_arg.stats = &stats;
    sig_handler_arg.clients = &clients;

    sigemptyset( &sig_handler_arg.set );
    sigaddset( &sig_handler_arg.set, SIGHUP );
    sigaddset( &sig_handler_arg.set, SIGPIPE );
    // Block SIGHUP, SIGPIPE in this thread (main thread)
    pthread_sigmask( SIG_SETMASK, &sig_handler_arg.set, NULL );

    // Create signal handler thread
    pthread_create( &sig_handler, NULL, print_stats_sig_handler, &sig_handler_arg );

    // Start accepting clients
    pthread_mutex_t stdout_lock;
    pthread_mutex_init( &stdout_lock, 0 );

    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(struct sockaddr_in);

    int client_sock;
    ClientHandlerArg *client_handler_arg;
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init( &attr );
    pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );

    while(1)
    {
        client_sock = accept( listen_sock, (struct sockaddr*)&client_addr, &client_addr_size );
        if( client_sock < 0 ) continue;

        client_handler_arg = malloc( sizeof(ClientHandlerArg) );
        *client_handler_arg = (ClientHandlerArg)
        {
            .client_sock = client_sock,
            .authdstr = &args.authdstr,
            .stats = &stats,
            .clients = &clients,
            .stdout_lock = &stdout_lock,
        };

        // Create client handler thread
        pthread_create( &tid, &attr, client_handler, client_handler_arg );
    }

    pthread_cancel( sig_handler );
    pthread_join( sig_handler, 0 );
    dynstring_destroy( &args.authdstr );
    return NO_ERR;
}
