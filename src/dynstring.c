#include "dynstring.h"
#include <stdlib.h>

void dynstring_init( DynString *str, unsigned int size )
{
    str->chars = malloc(sizeof(char) * size);
    str->size = size;
    str->length = 0;
}

void dynstring_destroy( DynString *str )
{
    str->size = 0;
    free(str->chars);
}
