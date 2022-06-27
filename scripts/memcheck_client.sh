#!/bin/bash

valgrind -s --leak-check=full\
    --show-leak-kinds=all\
    --track-origins=yes\
    --log-file=build/client_val.log\
    build/client $1 $2 $3 $4 > build/client.out
