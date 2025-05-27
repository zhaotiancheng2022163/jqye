#include "sort.h"
#define ALLOCSIZE 100000
char allocbuf[ ALLOCSIZE ];
char *allocp = allocbuf;

char *alloc( int n )
{
    if( allocbuf + ALLOCSIZE - allocp >= n ) {
        allocp += n;
        return allocp - n;
    }else
        return 0;
}

void afree( char *p )
{
    if( p >= allocbuf && p < allocbuf + ALLOCSIZE )
        allocp = p;
}