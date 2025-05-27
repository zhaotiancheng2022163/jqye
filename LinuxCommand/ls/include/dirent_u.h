#ifndef DIRENT
#define DIRENT
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#ifndef DIRSIZ
#define DIRSIZ 14
#endif
#ifndef NAME_MAX
#define NAME_MAX 255
#endif

typedef struct{
	long ino;
	char name[ NAME_MAX + 1 ];
} Dirent;

//为了避免和系统冲突, 如此命名
typedef struct{
    int fd;
    Dirent d;
}DIR_U;

DIR_U *opendir_u( char *dirname );
Dirent *readdir_u( DIR_U *dfd );
void closedir_u( DIR_U *dfd );

/* int stat(char *, struct stat *); */
void dirwalk( char *, void(*fcn)(char *));

void fsize( char * );
void dirwalk( char *, void (*fcn) (char *));
#endif
