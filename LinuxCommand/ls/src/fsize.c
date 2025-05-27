#include "dirent_u.h"

void fsize( char *name )
{
    struct stat stbuf;

    if( stat( name, &stbuf ) == -1 ) {
        fprintf(stderr, "fsize: can't access %s\n", name );
        return;
    }

    if( (stbuf.st_mode & S_IFMT ) == S_IFDIR )
        dirwalk( name, fsize );
    printf( "linkno:%d user:%4ld group:%4ld size:%4ld name:%s\n", stbuf.st_nlink,  stbuf.st_uid, stbuf.st_gid, stbuf.st_size, name );
}
