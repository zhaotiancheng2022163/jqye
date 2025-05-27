#include "dirent_u.h"

int main( int argc, char **argv )
{
    if( argc == 1 )
        fsize(".");
    else
        while( --argc > 0 )
            fsize(*++argv);
    return 0;
}
