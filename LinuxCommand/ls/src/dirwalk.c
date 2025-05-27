#include "dirent_u.h"

#define MAX_PATH 1024

void dirwalk( char *dir, void (*fcn)( char * ))
{
    char name[ MAX_PATH ];

    Dirent *dp;
    DIR_U *dfd;

    if( ( dfd = opendir_u( dir )) == NULL ) {
        fprintf(stderr, "dirwalk: can't open %s\n", dir );
        return;
    }

    while( (dp = readdir_u( dfd ) ) != NULL ) {
        if ( strcmp( dp -> name, "." ) == 0
             || strcmp( dp -> name, ".." ) == 0 )
            continue;

        if( strlen( dir ) + strlen( dp -> name ) + 2 > sizeof( name ))
            fprintf( stderr, "dirwalk: name %s/%s too long\n",
                     dir, dp -> name );
        else {
            sprintf( name, "%s/%s", dir, dp -> name );
            ( *fcn )( name );
        }
    }

    closedir_u( dfd );
}

