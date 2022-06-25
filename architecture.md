# Architecture

## Communication protocol
This chat system has its own communication protocol which consists of two handshakes, one for authentication and one for naming the client.

### Authentication
1. `Server -> AUTH: -> Client`
2. `Server <- AUTH:authstring <- Client`
3. `Server -> OK: -> Client` if authstring correct
4. Close connection if authstring incorrect

### Naming
1. `Server -> WHO: -> Client`
2. `Server <- NAME:name <- Client`
3. `Server -> NAME_TAKEN: -> Client` if name is in use by another connected client
4. `Server -> OK: -> Client` if name is not in use

### Other command types
- `Server -> ENTER:name -> Client` the server is informing the client *name* has joined
- `Server -> LEAVE:name -> Client` the server is informing the client *name* has left
- `Server <- LEAVE: <- Client` the client is about to leave the session
- `Server <- KICK:name <- Client` the client is asking the server to kick *name*
- `Server -> KICK: -> Client` the server is asking or forcing the client to leave
- `Server <- LIST: <- Client` the client is asking for a list of connected clients
- `Server -> List:list_of_names -> Client` the server is giving the client a list of connected clients
- `Server <- SAY:message <- Client` the client has sent a *message*
- `Server -> MSG:name:message -> Client` the server is informing the client a *message* from *name* has been received

## General architecture
The client and server uses POSIX threads (-pthreads) implemented using NPTL, which is one-to-one with kernel threads.
```
getconf GNU_LIBPTHREAD_VERSION
```
### Client threads
1. Handles reading from server and writing to stdout (handle_server_comm)
2. Handles writing to server and reading from stdin (main thread)

### Server threads
1. Handles accepting incoming connection requests on binded socket (main thread)
2. Handles SIGHUP and writes to stderr, ignores SIGPIPE (print_stats_sig_handler)
3. Handles receiving from clients, writing to clients and writing to stdout. Because another client_handler can write to the client, the transmission stream should be mutex protected. Only the dedicated client_handler can receive from the client however. This is not efficient usage of a thread because most of the time the thread is blocked on I/O but we do it for simplicity.

## Data structs
### dynstring.c/h
This is a dynamic string which expands the buffer to accommodate more data. Mainly used for reading unknown-sized strings from streams. Does a lot of bounds-checking though.

### list.c/h
Singly linked list for storing clients' information in ASCII order (by their name). The list must be thread-safe because it is shared between client_handler threads. The lifetime of a client_handler is coupled with the lifetime of its list node, so the reference to its list node is always valid, but you still can't read/write to it without locking the list mutex. The only reason for holding the reference is so the list doesn't need to be searched multiple times.

## Dependency tree
- server.c
    - serverlib.c/h
    - dynstring.c/h
    - list.c/h
    - util.c/h
- client.c
    - clientlib.c/h
    - dynstring.c/h
    - util.c/h

`server.c` and `client.c` only contain a `main()` function, the rest of the implementation is in the corresponding `*lib.c/h` for unit testing purposes.