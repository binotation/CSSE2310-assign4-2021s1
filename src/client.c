#include "clientlib.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// /** 
//  *  Writes the client name and number to the file stream. e.g. clientName1.
//  *  Params:
//  *      stream: the file stream
//  *      name: the client's name
//  *      nameNum: a pointer to the int to append to the client's name, -1 if no
//  *      int to append
//  **/
// void send_name(FILE *stream, const char *name, int *nameNum) {
//     if (*nameNum == -1) {
//         fprintf(stream, "NAME:%s\n", name);
//     } else {
//         fprintf(stream, "NAME:%s%d\n", name, *nameNum);
//     }
//     fflush(stream);
// }               

// /** 
//  *  Authenticates the client by receiving the AUTH: challenge and responding
//  *  and handling the reply. The client will exit with status 4 if 
//  *  authentication is denied.
//  *  Params:
//  *      streams: an array with the server read and write streams.
//  *      auth: the auth string
//  **/
// void authenticate(FILE **streams, char *auth) {
//     int readsBeforeEof;
//     String line;
//     bool authenticated = false;

//     while (!authenticated) {
//         line = get_line(streams[0], &readsBeforeEof);
//         if (readsBeforeEof > -1) {
//             fputs(AUTH_ERR_MSG, stderr);
//             exit(4);
//         } else if (!strncmp(line.chars, "AUTH:", 5)) {
//             fprintf(streams[1], "AUTH:%s\n", auth);
//             fflush(streams[1]);
            
//             while (!authenticated) {
//                 free(line.chars);
//                 line = get_line(streams[0], &readsBeforeEof);
//                 if (readsBeforeEof > -1) {
//                     fputs(AUTH_ERR_MSG, stderr);
//                     exit(4);
//                 } else if (!strncmp(line.chars, "AUTH:", 5)) {
//                     fprintf(streams[1], "AUTH:%s\n", auth);
//                     fflush(streams[1]);
//                 } else if (!strncmp(line.chars, "OK:", 3)) {
//                     authenticated = true;
//                 }
//             }
//         } 
//         free(line.chars); 
//     }
// }

// /**
//  *  Does the name negotiation. Returns when the name is accepted by the server.
//  *  Exits with status 2 if the connection is lost.
//  *  Params:
//  *      streams: an array with the server read and write streams.
//  *      name: the client's name
//  *      nameNum: a pointer to the int to append to the client's name, -1 if no
//  *      int to append.
//  **/
// void negotiate_name(FILE **streams, const char *name, int *nameNum) {
//     int readsBeforeEof;
//     String line;
//     bool named = false;
    
//     while (!named) {
//         line = get_line(streams[0], &readsBeforeEof);
//         if (readsBeforeEof > -1) {
//             fputs(COMM_ERR_MSG, stderr);
//             exit(2);
//         } else if (!strncmp(line.chars, "WHO:", 4)) {
//             send_name(streams[1], name, nameNum);
//         } else if (!strncmp(line.chars, "NAME_TAKEN:", 11)) {
//             (*nameNum)++;
//         } else if (!strncmp(line.chars, "OK:", 3)) {
//             named = true;
//         }
//         free(line.chars);
//     }
// }

// /**
//  *  Prints 'name: message' to stdout based on the command MSG:name:message.
//  *  Params:
//  *      cmd: the command string of the format 'MSG:name:message'
//  **/
// void handle_msg(char *cmd) {
//     char *firstArgIndex = strstr(cmd, ":") + 1;
//     char *secondArgIndex = strstr(firstArgIndex, ":") + 1;
//     char *firstArg = get_first_arg(cmd);
//     printf("%s: %s\n", firstArg, secondArgIndex);
//     fflush(stdout);
//     free(firstArg);
// }

// /** 
//  *  Thread routine for handling server communication. The client exits if EOF 
//  *  is read or the client is kicked.
//  *  Params:
//  *      arg: the server read stream
//  *  Returns: NULL
//  **/
// void *handle_server_comm(void *arg) {
//     FILE *from = (FILE*)arg;
//     int readsBeforeEof;
//     String line;

//     while (1) {
//         line = get_line(from, &readsBeforeEof);
//         if (readsBeforeEof > -1) {
//             fputs(COMM_ERR_MSG, stderr);
//             exit(2);
//         } else if (line.length > 6 && !strncmp(line.chars, "ENTER:", 6)) {
//             printf("(%s has entered the chat)\n", line.chars + 6);
//         } else if (line.length > 6 && !strncmp(line.chars, "LEAVE:", 6)) {
//             printf("(%s has left the chat)\n", line.chars + 6);
//         } else if (!strncmp(line.chars, "MSG:", 4) && is_valid_2_arg_cmd(line.chars)) {
//             handle_msg(line.chars);
//         } else if (!strncmp(line.chars, "KICK:", 5)) {
//             fputs("Kicked\n", stderr);
//             exit(3);
//         } else if (line.length > 5 && !strncmp(line.chars, "LIST:", 5)) {
//             printf("(current chatters: %s)\n", line.chars + 5);
//         }
//         fflush(stdout);
//         free(line.chars);
//     }
//     return 0;
// }

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
            return ARGS_ERR
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
    if( !negotiate_auth( &server, args.authdstr.str, &line ) )
    {
        fputs(AUTH_ERR_MSG, stderr);
        return AUTH_ERR;
    }

    // int nameNum = -1;
    // negotiate_name(streams, argv[1], &nameNum);

    // bool exited = false;
    // pthread_t serverHandler;
    // pthread_create(&serverHandler, 0, handle_server_comm, (void*)streams[0]);

    // String line;
    // do {
    //     line = get_line(stdin, &readsBeforeEof);
    //     if (readsBeforeEof > -1) {
    //         free(line.chars);
    //         break;
    //     }
    //     if (line.chars[0] == '*') {
    //         fprintf(streams[1], "%s\n", line.chars + 1);
    //         if (!strncmp(line.chars + 1, "LEAVE:", 6)) {
    //             exited = true;
    //         }
    //     } else {
    //         fprintf(streams[1], "SAY:%s\n", line.chars);
    //     }
    //     fflush(streams[1]);
    //     free(line.chars);
    // } while (!exited);
    // pthread_cancel(serverHandler);
    // pthread_join(serverHandler, 0);

    dynstring_destroy( &line );
    fclose( server.read );
    fclose( server.write );
    dynstring_destroy( &args.authdstr );
    return NO_ERR;
}
