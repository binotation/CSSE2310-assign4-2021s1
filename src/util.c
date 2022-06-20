#include "util.h"
#include <string.h>

bool get_two_args( const char *cmdstr, TwoArgs *targs )
{
    if(( targs->arg1.str = strstr( cmdstr, ":" ) + 1 ) == (char*)1 ) return false;
    if(( targs->arg2.str = strstr( targs->arg1.str, ":" ) + 1 ) == (char*)1 ) return false;

    targs->arg1.length = targs->arg2.str - targs->arg1.str - 1;
    targs->arg2.length = strlen( targs->arg2.str );
    return ( targs->arg1.length > 0 ) && ( targs->arg2.length > 0 );
}

void replace_unprintable( char *str )
{
    unsigned int i = 0;
    while( str[i] != '\0' )
    {
        if ( str[i] < 32 )
        {
            str[i] = '?';
        }
        i++;
    }
}
