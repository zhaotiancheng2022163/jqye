#include "dirent_u.h"

struct direct {
	ino_t d_ino;
	char d_name[ DIRSIZ ];
};

DIR_U *opendir_u( char *dirname )
{
    int fd;
    struct stat stbuf;
    DIR_U *dp;

    if(( fd = open( dirname, O_RDONLY, 0)) == -1
       || fstat( fd, &stbuf) == -1
       || (stbuf.st_mode & S_IFMT ) != S_IFDIR
       || ( dp = ( DIR_U * ) malloc( sizeof(DIR_U))) == NULL )
        return NULL;

    dp -> fd = fd;
    return dp;
}

void closedir_u( DIR_U *dp )
{
    if( dp ) {
        close( dp -> fd );
        free( dp );
    }
}

Dirent *readdir_u( DIR_U *dp )
{
    struct direct dirbuf;
    static Dirent d;

    while( read( dp -> fd, ( char *) &dirbuf, sizeof( dirbuf ))
           == sizeof( dirbuf )) {
        if( dirbuf.d_ino == 0 )
            continue;

        d.ino = dirbuf.d_ino;
        strncpy( d.name,dirbuf.d_name, DIRSIZ );
        d.name[ DIRSIZ ] = '\0';
        return &d;
    }

    return NULL;
}
