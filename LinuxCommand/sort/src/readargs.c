#include "sort.h"


void readargs( int argc, char *argv[] )
{
    int c;

    while( --argc > 0 && ( c = (*++argv)[ 0 ] == '-' || c == '+')) {
        if ( c == '-' && !isdigit( *( argv[ 0 ] + 1)))
            while( c = *++argv[ 0 ] )
                switch( c ) {
                    case 'd':
                        option |= DIR;
                        break;
                    case 'f':
                        option |= FOLD;
                        break;
                    case 'n':
                        option |= NUMERIC;
                        break;
                    case 'r':
                        option |= DECR;
                        break;
                    default:
                        printf("sort: illegal option %c\n", c );
                        //还要在外层循环走一遍啊，这样argc不成了0？
                        error("Usage: sort -dfnr [+post1] [-pos2]");
                        break;
                }
        else if ( c == '-' )
            pos2 = atoi( argv[ 0 ] + 1 );
        else if ( (pos1 = atoi(argv[ 0] + 1 )) < 0 )
            error( "usage :sort -dfnr [+pos1] [-pos2 ]");
    }
    if ( argc || pos1 > pos2 )
        error( "usage :sort -dfnr [+pos1] [-pos2 ]");

}