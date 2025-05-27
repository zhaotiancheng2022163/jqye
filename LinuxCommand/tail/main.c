#include "./include/tail.h"

int main( int argc, char *argv[] )
{
    char *lineptr[ MAXLINE ];
    char value[ MAXLEN ];

    int number;

    int nm = 0;
    int nlines = 0;
    char **ptr = lineptr;

    int c;
    char s[10000];
    char *sptr = s;

    while( --argc == 1 && (*++argv)[ 0 ] == '-' ) {
        while( c = *++argv[ 0 ] ) {
            if( isdigit( c ) )
                *sptr++ = c;
            else{
                printf("tail: illegal option %c\n", c);
            }
        }
    }

    *sptr = '\0';
    number = atoi( s );

    if( argc != 0 )
        printf("Usage: ./main -n pattern\n");
    else{
        while( ( nm = readlines( lineptr, value, MAXLINE ) ) > 0 ) //由于是用EOF停止输入的, 所以nm最后的值为0,这就有必要用一个变量来保存nm倒数第二次的值了
            nlines = nm;

        if( number >= nlines || number <= 0) //number不可能是负数, 由于上面的处理, 现在想想没必要
            number = nlines;

        printf("\n");
        for( ptr = lineptr + nlines - number; ptr <= lineptr + nlines - 1; ++ptr )
            printf("%s\n", *ptr );
    }
}

