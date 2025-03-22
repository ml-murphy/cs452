#!/bin/bash

# Compile the tests
gcc -o balloc_test balloc_test.c
gcc -o freelist_test freelist_test.c

# Run the tests
./balloc_test
./freelist_test