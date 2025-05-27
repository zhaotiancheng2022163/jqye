// 此程序可以实现逆波兰计算器的四则运算, 取模, 和三角函数运算
#include <stdio.h>
#include <stdlib.h>
#include "calc.h"

#define MAXOP   100

int main()
{
    int i, type, var = 0;
    double op1, op2, v, t;
    char s[ MAXOP ];
    double variable[ 26 ];

    for( i = 0; i < 26; i++ )
        variable[ i ] = 0.0;

    while( ( type = getop( s ) ) != EOF ){
        switch( type ) {
            case NUMBER:
                push( atof( s ) );
                break;
            case NAME:
                //把所有的数学函数计算合并在一个里面了，不用分开来判断
                mathfnc( s );
                break;
            case '+':
                push( pop() + pop() );
                break;
            case '*':
                push( pop() * pop() );
                break;
            case '-':
                op2 = pop();
                push( pop() - op2 );
                break;
            case '/':
                op2 = pop();
                if( op2 != 0.0 )
                    push( pop() / op2 );
                else
                    printf("error: zero divisor\n");
                break;
            case '%':
                op2 = pop();
                if( op2 != 0.0 )
                    //一定需要知道%只针对整型，fmod用于double型
                    push( fmod( pop(), op2 ));
                else
                    printf("error: zero divisor\n");
                break;
                //这样做的原因是为了避免主程序直接对堆栈和堆栈指针(sp)进行操作
            case '?':
                op2 = pop();
                printf("\t%.8g\n", op2);
                push( op2 );
                break;
            case 'c':
                clear();
                break;
            case 'd':
                op2 = pop();
                push( op2 );
                push( op2 );
                break;
            case 's':
                op1 = pop();
                op2 = pop();
                push( op1 );
                push( op2 );
                break;
            case '=':
                pop();
                if( var >= 'A' && var <= 'Z') {
                    //t为了防止值栈为空
                    t = pop();
                    variable[ var - 'A' ] = t;
                    push( t );
                }
                else
                    printf("error: no variable name\n");
                break;
            case '\n':
                v = pop();
                printf("\t%.8g\n", v );
                break;
            default:
                if( type >= 'A' && type <= 'Z' )
                    push( variable[ type - 'A']);
                else if( type == 'v')
                    push( v );
                else
                    printf("error: unknown command %s\n", s);
                break;
        }
        var = type;
    }
    return 0;
}



