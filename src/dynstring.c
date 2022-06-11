#include "dynstring.h"
#include <stdlib.h>
#include <string.h>

#define RESIZE( str ) str->chars = realloc( str->chars, sizeof(char) * ( str->size *= 2 ) )
#define TERMINATE_LAST_CHAR( str ) str->chars[ --str->length ] = '\0'

void dynstring_init( DynString *str, unsigned int size )
{
    str->chars = malloc( sizeof(char) * size );
    str->chars[0] = '\0';
    str->size = size;
    str->length = 0;
}

void dynstring_destroy( DynString *str )
{
    str->size = 0;
    free( str->chars );
}

enum ReadlineResult dynstring_readline( DynString *str, FILE *stream )
{
    str->length = 0;
    char *s;
    unsigned int num;				// Number of chars read in the current iteration
    unsigned int shift = str->size;	// Number of chars to read next

    do
    {
        s = fgets( str->chars + str->length, shift, stream );
        if( s == NULL ) // If error or eof reached while no chars have been read
        {
            return feof( stream ) != 0 ? EOF_REACHED : ERROR;
        }
        else
        {
            str->length += num = strlen( str->chars + str->length );

            // If buffer maxed out, i.e. more to read
            if( num == shift - 1 && str->chars[ str->length - 1 ] != '\n' )
            {
                RESIZE( str );
                shift = str->size / 2 + 1; // +1 because we overwrite '\0' in the next iteration
            }
            else
            {
                TERMINATE_LAST_CHAR( str );
                return SUCCESS;
            }
        }
    } while(1);
}
