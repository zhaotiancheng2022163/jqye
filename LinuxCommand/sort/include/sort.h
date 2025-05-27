#ifndef SORT
#define SORT
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>


#define ALLOCSIZE 100000
#define MAXLEN   1000
#define NUMERIC  1
#define DECR     2                      //为了可以使用位运算，0010
#define FOLD     4
#define DIR      8
#define LINES    100

static char  option = 0;
static int pos1 = 0;
static int pos2 = 0;


int  readlines( char *lineptr[], int maxlines );
int  getlines( char *, int );
void writelines( char *lineptr[], int maxlines, int decr );
void quick_sort( void *v[], int left, int right,
                 int (*comp)(void *, void *));
void swap1( void *lineptr[], int i, int j );

char *alloc( int n );
void  afree( char *p );
void  error( char * );
void  readargs( int argc, char *argv[] );
void  substr(const char *s,char *t );
int   numcmp( const char *, const char *);
int   charcmp( const char *s, const char *t);

#endif