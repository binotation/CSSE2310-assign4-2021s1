#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

#define COMM_ERR_MSG "Communications error\n"
#define AUTH_ERR_MSG "Authentication error\n"

#ifndef UTIL_H
#define UTIL_H

/* A wrapper for heap allocated strings. String.chars is the heap allocated 
 * array, String.size is the size of the allocated memory and String.length is
 * the number of chars excluding the null terminator. */
typedef struct {
    char *chars;
    int size;
    int length;
} String;

String get_line(FILE*, int*);

bool is_valid_2_arg_cmd(const char*);

char *get_first_arg(char*);

char *convert_unprintable(const char*);

#endif
