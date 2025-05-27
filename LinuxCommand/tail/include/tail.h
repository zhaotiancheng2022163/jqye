#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#define MAXLEN  1000
#define MAXLINE 1000
#define MAXSTOR 5000

int readlines( char *lineptr[], char *linestor, int maxline);
int getlines( char *s, int lim );