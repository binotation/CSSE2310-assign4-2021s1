#!/bin/bash

for i in {1..100}; do
    # (cat test/testfiles/spam_server.txt | valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes --log-file=build/client_val$i.log build/client Barry test/testfiles/auth.txt 10754 > /dev/null) &
    (cat test/testfiles/spam_server.txt | build/client Barry test/testfiles/auth.txt 10754 > /dev/null) &
done
