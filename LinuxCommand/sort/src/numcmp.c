#include "sort.h"
#define MAXSTR 100

int numcmp( const char *s1, const char *s2 )
{
    double v1, v2;
    char str[ MAXSTR ];

    substr( s1, str);
    v1 = atof( str );
    substr( s2, str);
    v2 = atof( str );

    if( v1 < v2 )
        return -1;
    else if ( v1 > v2 )
        return 1;
    else
        return 0;
}