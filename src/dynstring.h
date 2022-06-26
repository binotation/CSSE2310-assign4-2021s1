/**
 * Dynamically sized string. Resize strategy is to double the allocated memory.
 */
#ifndef DYNSTRING_H
#define DYNSTRING_H

#include <stdio.h>

/**
 * Dynamically sized string. No members should be mutated externally.
 * @param .str		buffer
 * @param .size		size of the allocated memory in number of chars.
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
 * @param size	initial size in number of chars; size > 0.
 */
void dynstring_init( DynString *dstr, unsigned int size );

/**
 * Create a dynstring from str by copying memory of length. dstr must be uninitialized.
 * @param length	length of str excluding NULL terminator
 * @param size		size of memory to be allocated, must be sufficient to store the string
 */
void dynstring_nfrom( DynString *dstr, const char *str, unsigned int length, unsigned int size );

/**
 * Destroy DynString.
 */
void dynstring_destroy( DynString *dstr );

/**
 * Truncate dynstring to 0.
 */
void dynstring_clear( DynString *dstr );

/**
 * Append str of length to dstr.
 * @param length	length of str excluding NULL terminator
 */
void dynstring_npush( DynString *dstr, const char *str, unsigned int length );

/**
 * Append char to dstr.
 */
void dynstring_pushc( DynString *dstr, char c );

/**
 * Remove and return last char of dstr.
 */
char dynstring_popc( DynString *dstr );

/**
 * Read a line from a file stream excluding the newline.
 */
enum ReadlineResult dynstring_readline( DynString *dstr, FILE *stream );

#endif
