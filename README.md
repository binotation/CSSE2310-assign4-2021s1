# Chat server and client
This is a simple, networked, multi-threaded server-client chat system.

## Build
```
make target
```
- Binaries are deposited in `build/`.
- The default compiler is `clang` but just change `CC=gcc` in the Makefile if you want to use `gcc` instead.

## Usage
### Server
```
server authfile [port]
```
Start server on *port*; random port selected if not given. The chosen port is immediately printed to stderr.
- The server must be set up with an authentication string contained as the first line of *authfile*.
- Send SIGHUP to the server for chat session statistics, i.e. `kill -SIGHUP server_pid`

### Client
```
client name authfile [host] port
```
Connect client to server at *host* (ipv4) on *port*.
- The server authenticates the client using the first line of *authfile*.

The client has access to the following commands
- `*LIST:` - list session participants
- `*KICK:name` - kick another participant
- `*LEAVE:` - leave the chat

### Examples
Run server
```
build/server test/testfiles/auth.txt 22427
```

Run client
```
build/client Nichola test/testfiles/auth.txt 127.0.0.1 22427
```

## Testing
Using valgrind memcheck, you can view the memory usage of the server and client with the `memcheck_server.sh` and `memcheck_client.sh` scripts.

For a load test:
```
build/server test/testfiles/auth.txt 10754 > build/server.out 2> build/server.err &
python scripts/load_test.py
chmod +x load_test.sh
./load_test.sh
```
This attempts to connect 2500 clients at once. Useful for detecting server memory leaks and invalid read/writes from concurrency related bugs. Also, the client processes tend to not terminate. Kill clients with:

```
for pid in $(ps -ef | grep 'build/client' | awk '{print $2}'); do kill $pid; done
```
