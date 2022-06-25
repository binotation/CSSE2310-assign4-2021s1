#!/bin/bash

if [ -z "$1" ]; then
    >&2 echo 'Missing name'
    exit 1
fi

valgrind -s --leak-check=full\
    --show-leak-kinds=all\
    --track-origins=yes\
    --log-file=build/server_val.log\
    build/client $1 test/testfiles/auth.txt 10754
