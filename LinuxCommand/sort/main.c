#include "sort.h"
#define MAXLINES 5000
char *lineptr[ MAXLINES ];

int main( int argc, char *argv[])
{
    char *lineptr[ LINES ];
    int nlines;
    int rc = 0;

    readargs( argc, argv );

    if( (nlines = readlines( lineptr, LINES )) > 0) { //为什么少了一个呢？strcmp的 答：charcmp以及含有strcmp了, 真了不起
        if( option & NUMERIC )
            quick_sort( (void **) lineptr, 0, nlines - 1,
                        (int (*) (void *, void *)) numcmp);
        else
            quick_sort((void **) lineptr, 0, nlines -1,
                       (int (*)(void *, void *)) charcmp);
        printf("-----------------------\n");
        writelines(lineptr, nlines, option & DECR );
    } else {
        printf("input too big to sort \n");
        rc = -1;
    }

    return rc;
}


