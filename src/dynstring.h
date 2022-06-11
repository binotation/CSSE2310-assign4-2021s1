/**
 * Dynamically sized string. Resize strategy is to double the allocated memory.
 */
#ifndef DYNSTRING_H
#define DYNSTRING_H

#include <stdio.h>

/**
 * Dynamically sized string. No members should be mutated externally.
 * @param .str		the string
 * @param .size		size of the allocated memory in chars.
 * @param .length	number of chars in the string excluding the null terminator.
 */
typedef struct
{
    char *str;
    unsigned int size;
    unsigned int length;
} DynString;

enum ReadlineResult
{
    SUCCESS,
    EOF_REACHED,
    ERROR,
};

/**
 * Initialize DynString with size.
 * @param size	initial size in chars; size > 0.
 */
void dynstring_init( DynString *dstr, unsigned int size );

/**
 * Destroy DynString.
 */
void dynstring_destroy( DynString *dstr );

/**
 * Read a line from a file stream excluding the newline.
 * @returns if EOF was reached, error occurred or success
 */
enum ReadlineResult dynstring_readline( DynString *dstr, FILE *stream );

#endif
