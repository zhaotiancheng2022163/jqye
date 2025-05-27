#include "sort.h"

int charcmp( const char *s, const char *t) //这个函数写的真好啊，三种情况都放在一个函数里面了,虽说精妙，但这也是因为框架是一样的才能这样做,不带有普适性，但在特定问题的解决上面真的是一个很好的解决方案。
{
    char a, b;
    int i, j, endpos;
    extern char option;
    extern int pos1, pos2;
    int fold = ( option & FOLD ) ? 1 : 0; //像这样写很好，不用三目运算也
    int dir  = ( option & DIR ) ? 1 : 0;  //可以，但这样好一点

    i = j = pos1;
    if( pos2 > 0 )
        endpos = pos2;
    else if ( (endpos - strlen( s )) > strlen( t ) ) //这是为什么啊？
        endpos = strlen( t );

    do {
        if ( dir ) {
            while( i < endpos && !isalnum( i ) &&
                   s[ i ] != ' ' && s[ i ] != '\0' )
                i++;
            while( j < endpos && !isalnum( j ) &&
                   s[ j ] != ' ' && s[ j ] != '\0' )
                j++;
        }


        if( i < endpos && j < endpos ) {
            a = fold ? tolower( s[ i ] ) : s[ i ];
            i++;
            b = fold ? tolower( t[ j ]  ) : t[ j ];
            j++;
            if ( a == b && a == '\0' )
                return 0;
        }
    } while( a == b && i < endpos && j < endpos );

    return a - b;
}