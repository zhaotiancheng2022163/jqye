#include <string.h>
#include <math.h> //fmod函数
#ifndef CALC
#define CALC
#define NUMBER  '0'
#define NAME    'n'


int getlines( char[], int);
int getop( char[]);
int getch( void );
void ungetch( int );
void ungets( char s[] );

void push( double );
double pop( void );
void clear( void );

void mathfnc( char []);

#endif