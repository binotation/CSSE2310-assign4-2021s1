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
    READLINE_SUCCESS,	// Chars were read
    READLINE_EOF,		// No chars were read and EOF encountered
    READLINE_ERROR,		// Error
};

/**
 * Initialize DynString with size.
 * @param size	initial size in chars; size > 0.
 */
void dynstring_init( DynString *dstr, unsigned int size );

/**
 * Create a dynstring from str by copying memory of length. dstr must be uninitialized.
 * @param length	length of str excluding NULL terminator
 */
void dynstring_nfrom( DynString *dstr, const char *str, unsigned int length );

/**
 * Destroy DynString.
 */
void dynstring_destroy( DynString *dstr );

/**
 * Append str of length to dstr.
 * @param length	length of str excluding NULL terminator
 */
void dynstring_npush( DynString *dstr, const char *str, unsigned int length );

/**
 * Read a line from a file stream excluding the newline.
 * @returns if EOF was reached, error occurred or success
 */
enum ReadlineResult dynstring_readline( DynString *dstr, FILE *stream );

#endif
