#include "sort.h"
void writelines( char *lineptr[], int nlines, int decr ) //对啊，不一定需要再次排序啊，那样多麻烦啊，还浪费时间，直接升序排号，逆序输出不久是降序了吗？
{
    int i;

    if( decr )
        for( i = nlines -1; i >= 0; i-- )
            printf("%s\n", lineptr[ i ] );
    else
        for( i = 0; i < nlines; i ++ )
            printf( "%s\n", lineptr[ i ]);
}