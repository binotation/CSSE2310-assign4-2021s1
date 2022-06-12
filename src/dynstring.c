#include "dynstring.h"
#include <stdlib.h>
#include <string.h>

#define RESIZE( dstr ) dstr->str = realloc( dstr->str, sizeof(char) * ( dstr->size *= 2 ) )
#define TERMINATE_LAST_CHAR( dstr ) dstr->str[ --dstr->length ] = '\0'

void dynstring_init( DynString *dstr, unsigned int size )
{
    dstr->str = malloc( sizeof(char) * size );
    dstr->str[0] = '\0';
    dstr->size = size;
    dstr->length = 0;
}

void dynstring_destroy( DynString *dstr )
{
    dstr->size = 0;
    free( dstr->str );
}

enum ReadlineResult dynstring_readline( DynString *dstr, FILE *stream )
{
    dstr->length = 0;
    dstr->str[0] = '\0';
    char *s;
    unsigned int num;					// Number of chars read in the current iteration
    unsigned int shift = dstr->size;	// Number of chars to read next

    do
    {
        s = fgets( dstr->str + dstr->length, shift, stream );
        if( s == NULL ) // If error or eof reached while no chars have been read
        {
            return feof( stream ) != 0 ? READLINE_EOF_REACHED : READLINE_ERROR;
        }
        else
        {
            dstr->length += num = strlen( dstr->str + dstr->length );

            // If buffer maxed out, i.e. more to read
            if( num == shift - 1 && dstr->str[ dstr->length - 1 ] != '\n' )
            {
                RESIZE( dstr );
                shift = dstr->size / 2 + 1; // +1 because we overwrite '\0' in the next iteration
            }
            else
            {
                TERMINATE_LAST_CHAR( dstr );
                return READLINE_SUCCESS;
            }
        }
    } while(1);
}
