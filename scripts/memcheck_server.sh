#!/bin/bash

valgrind -s --leak-check=full\
    --show-leak-kinds=all\
    --track-origins=yes\
    --log-file=build/server_val.log\
    build/server test/testfiles/auth.txt 10754 > build/server.out 2> build/server.err
