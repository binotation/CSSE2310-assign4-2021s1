#include "dynstring.h"
#include <stdlib.h>
#include <string.h>

#define TERMINATE_LAST_CHAR( dstr ) dstr->str[ --dstr->length ] = '\0'

void dynstring_init( DynString *dstr, unsigned int size )
{
    dstr->str = malloc( sizeof(char) * size );
    dstr->str[0] = '\0';
    dstr->size = size;
    dstr->length = 0;
}

void dynstring_nfrom( DynString *dstr, const char *str, unsigned int length )
{
    dynstring_init( dstr, length + 1 );
    memcpy( dstr->str, str, sizeof(char) * length );
    dstr->str[ dstr->length = length ] = '\0';
}

void dynstring_destroy( DynString *dstr )
{
    dstr->size = 0;
    free( dstr->str );
}

void dynstring_npush( DynString *dstr, const char *str, unsigned int length )
{
    if( dstr->length + length + 1 > dstr->size )
    {
        dstr->size <<= 1;
        while( dstr->length + length + 1 > dstr->size ) dstr->size <<= 1;
        dstr->str = realloc( dstr->str, sizeof(char) * dstr->size );
    }

    memcpy( dstr->str + dstr->length, str, sizeof(char) * length);
    dstr->str[ dstr->length += length ] = '\0';
}

enum ReadlineResult dynstring_readline( DynString *dstr, FILE *stream )
{
    dstr->length = 0;
    dstr->str[0] = '\0';
    char *s;
    unsigned int count = 0;				// Number of iterations
    unsigned int num;					// Number of chars read in the current iteration
    unsigned int shift = dstr->size;	// Number of chars to read next

    do
    {
        s = fgets( dstr->str + dstr->length, shift, stream );
        if( s == NULL ) // If error or eof reached while no chars have been read
        {
            if( feof( stream ) != 0 && count == 0 ) return READLINE_EOF;
            else if( feof( stream ) != 0 && count > 0 ) return READLINE_SUCCESS;
            else return READLINE_ERROR;
        }
        else
        {
            count++;
            dstr->length += num = strlen( dstr->str + dstr->length );

            // If buffer maxed out, i.e. more to read
            if( num == shift - 1 && dstr->str[ dstr->length - 1 ] != '\n' )
            {
                dstr->str = realloc( dstr->str, sizeof(char) * ( dstr->size <<= 1 ));
                shift = dstr->size / 2 + 1; // +1 because we overwrite '\0' in the next iteration
            }
            else if( dstr->str[ dstr->length - 1 ] != '\n' )
            {
                return READLINE_SUCCESS;
            }
            else
            {
                TERMINATE_LAST_CHAR( dstr );
                return READLINE_SUCCESS;
            }
        }
    } while(1);
}
