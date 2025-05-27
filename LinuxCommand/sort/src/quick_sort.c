#include "sort.h"
void quick_sort( void *v[], int left, int right ,
                 int (*comp)(void *, void *))
{
    int last, i;

    if( left >= right )
        return;

    swap1( v, left, (left + right)/2);
    last = left;

    for( i = left + 1; i <= right; i++ ) {
        if( ( *comp )( v[ i ], v[ left ] ) < 0 )
            swap1( v, i, ++last);
    }

    swap1( v, last, left );
    quick_sort( v, left, last - 1, comp);
    quick_sort( v, last + 1 , right, comp);
}

void swap1( void *v[], int i, int j )
{
    void *temp;

    temp   = v[ i ];
    v[ i ] = v[ j ];
    v[ j ] = temp;
}