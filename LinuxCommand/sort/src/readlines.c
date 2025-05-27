#include "sort.h"
int readlines( char *lineptr[], int maxlines )
{
    int nlen, nlines;
    char *p, line[ MAXLEN ];

    nlines = 0;

    while( ( nlen = getlines( line, MAXLEN ) ) > 0 ) {
        if( nlines >= maxlines || ( p = alloc( nlen ) ) == NULL ) {
            return -1;
        }else {
            line[ nlen - 1 ] = '\0';
            strcpy( p, line );
            lineptr[ nlines++ ] = p;
        }
    }

    return nlines;
}