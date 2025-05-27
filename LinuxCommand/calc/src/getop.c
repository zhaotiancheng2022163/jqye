#include "../include/calc.h"
#include <ctype.h>
#include <stdio.h>

#define MAXLINE 100
char line[ MAXLINE ];
int li   = 0;

int getop( char s[] )
{
    int i, c;

    if( line[ li ] == '\0' ) {
        if( getlines( line, MAXLINE ) == 0 )
            return EOF;
        else
            li = 0;
    }

    while( ( s[0] = c = line[ li++ ] ) == ' ' || c == '\t' )
        ;
    s[ 1 ] = '\0';
    i = 0;

    //没有必要在getop里面就做好全部判断，扔给main来进行最后的决断
    if( islower( c )) {
        while( islower( s[ ++i] = c = line[ li++ ] ) )
            ;
        s[ i ] = '\0';
        if( c != EOF )
            --li;
        if( strlen( s ) > 1 )
            return NAME;
        else
            //由于已经最少接受了一个输入，所以必须减1才能代表最初的代码
            return s[ --i ];
    }

    //由于除了数字，"." "-" 之外还有字母，所以这个if语句必须放在后面
    //这个语句主要针对非字母符号
    if( !isdigit( c ) && c != '.' && c != '-')
        return c;

    //减法和负数的区别就是一个是贴着数一个没有贴着数，这是最重要的区别
    //需要注意一点，这个是后缀表达式，所以是不用担心a-b这样的式子应该怎么做的
    if( c== '-') {
        if( isdigit( c = line[ li++ ]) || c == '.')
            s[ ++i ] = c;
        else{
            if( c != EOF )
                --li;
            return '-';
        }
    }

    if( isdigit( c ) )
        while( isdigit( s[++i] = c = line[ li++ ] )) //使用++i的好处是i的指向必然为存储的
            ;                                   //地址，不会多出来指向不应该的位置

    if( c == '.')
        while( isdigit( s[ ++i ] = c = line[ li++ ] ))
            ;

    s[ i ] = '\0'; //覆盖多的一个输入

    if( c != EOF )
        --li;

    return NUMBER;
}

int getlines( char s[], int lim)
{
    int i = 0;
    int c;
    while( --lim > 0  && ( c = getchar() ) != EOF && c != '\n')
        s[ i++ ] = c;
    if( c == '\n')
        s[ i++ ] = c;
    s[ i ] = '\0';
    return i;
}