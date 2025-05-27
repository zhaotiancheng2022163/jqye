#include "tail.h"
int readlines( char *lineptr[], char *linestor, int maxline)
{
    int len, nlines;
    char *p, line[ MAXLEN ];
    p = linestor;
    nlines = 0;
    char *linestop = linestor + MAXSTOR;

    while( ( len = getlines( line, MAXLEN ) ) > 0 )
        if( nlines >= maxline || p + len >= linestop )
            return -1;
        else {
            line[ len - 1 ] = '\0';
            strcpy( p, line );
            lineptr[ nlines++ ] = p;
            p += len;
        }

    return nlines;
}
