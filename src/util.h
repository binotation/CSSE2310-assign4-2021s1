/**
 * Miscellaneous string utility functions.
 */

#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

typedef struct
{
    const char *str;
    unsigned int length;
} StrSlice;

typedef struct
{
    StrSlice arg1;
    StrSlice arg2;
} TwoArgs;

/** 
 * Checks if a command string with 2 arguments of the form CMD:ARG1:ARG2 is valid,
 * i.e. the first and second arguments are not empty; e.g. CMD::ARG2 and CMD:ARG1: are invalid.
 * Populates *cmd with string slices for arg1 and arg2.
 * @returns	true if both arguments are not empty, false otherwise.
 **/
bool get_two_args( const char *cmdstr, TwoArgs *targs );

/**
 * Replace unprintable chars (< 32) with '?'.
 **/
void replace_unprintable( char *str );

#endif
