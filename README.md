# Simple server-client chat system
This is a simple networked, multi-threaded server-client chat system. Only works locally.

Build:

```bash
$ make
```

Server usage: `server authfile [port]`

- The server must be set up with an authentication password contained in the first line of <i>authfile</i>.

- Send SIGHUP to the server for chat session statistics.



Client usage: `client name authfile port`

The server authenticates the client using the first line of <i>authfile</i>.

The client has access to the following commands

- "*LIST:" - list session participants
- "*KICK:name" - kick another participant
- "*LEAVE:" - leave the chat



Example run server

```bash
$ ./server testing/authFile.txt 22427
```

Example run client

```bash
$ ./client clientName testing/authFile.txt 22427
```

