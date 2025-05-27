#ifndef HEAP_HH
#define HEAP_HH

#include <algorithm>
#include <cstddef>
#include <utility>

// 需要实现的几个功能:
// 1. make_heap
// 2. sort_heap
// 3. insert_heap
// 4. pop_heap

// 并实现 " < " 和 比较大小的函数对象两类
namespace mystd{

inline std::size_t left_child(std::size_t i)
{
    return 2 * i + 1;
}

inline std::size_t parent( size_t i )
{
    return ( i - 1 ) / 2;
}

// 上滤操作
template <typename Iter, typename Distance=std::ptrdiff_t, typename Object>
void push_heap(Iter first, Distance holeIndex, Distance topIndex, Object value)
{
    Distance parentIndex = parent( holeIndex );

    while( holeIndex > topIndex && *(first + parentIndex) < value ) {
        *(first + holeIndex) = std::move(*(first + parentIndex ));
        holeIndex = parentIndex;
        parentIndex = parent(holeIndex);
    }
    *(first + holeIndex ) = std::move(value);
}

template<typename Iter>
void push_heap(Iter first, Iter last)
{
    if( last - first > 1 ) {
        mystd::push_heap(first, last - first -1, static_cast<std::ptrdiff_t>(0), *(last -1));
    }
}

// 下滤操作
template <typename Iter, typename Distance=std::ptrdiff_t,typename Object>
void adjust_heap(Iter first, Distance holeIndex, Distance len, Object value)
{
    Object tmp = std::move(*(first + holeIndex));
    Distance child;

    for( ;left_child(holeIndex) < len; holeIndex = child ) {
        child = left_child(holeIndex);

        if( child != len - 1 && *(first + child)< *(first + child + 1))
            ++child;

        if( tmp < *(first + child ))
            *(first+holeIndex) = std::move(*(first+child));
        else
            break;
    }
    *(first+holeIndex) = std::move(tmp);
}

template <typename Iter>
void pop_heap(Iter first, Iter last)
{
    if( 1 >= last - first )
        return;

    std::swap(*first, *(last - 1));

    if ( 2 == last - first )
      return;

    mystd::adjust_heap(first, static_cast<std::ptrdiff_t>(0), last - first  - 1,*(first));
}

template <typename Iter>
void sort_pop(Iter first, Iter last)
{
    while( last - first > 1 ) {
        mystd::pop_heap(first, last);
        --last;
    }
}

template <typename Iter, typename Distance=std::ptrdiff_t>
void make_heap(Iter first,Iter last, Distance*)
{
    if( last - first < 2 )
        return;

    Distance len = last - first;
    for( Distance i = (len - 2) / 2; i >= 0; i-- ) // (len - 1 ) - 1 == len - 2
        mystd::adjust_heap(first, static_cast<std::ptrdiff_t>(i), static_cast<std::ptrdiff_t>( len ) , *(first));
}


template <typename Iter>
void make_heap(Iter first,Iter last)
{
    mystd::make_heap(first, last, static_cast<std::ptrdiff_t*>(0));
}

/* ==================================================================================================== */

// 上滤操作
template <typename Iter, typename Distance=std::ptrdiff_t, typename Object, typename Compare>
void push_heap(Iter first, Distance holeIndex, Distance topIndex, Object value, Compare& comp)
{
    Distance parentIndex = parent( holeIndex );

    while( holeIndex > topIndex && comp(*(first + parentIndex) , value) ) {
        *(first + holeIndex) = std::move(*(first + parentIndex ));
        holeIndex = parentIndex;
        parentIndex = parent(holeIndex);
    }
    *(first + holeIndex ) = std::move(value);
}

template<typename Iter, typename Compare>
void push_heap(Iter first, Iter last, Compare& comp)
{
    if( last - first > 1 ) {
        mystd::push_heap(first, last - first -1, static_cast<std::ptrdiff_t>(0), *(last -1), comp );
    }
}

// 下滤操作
template <typename Iter, typename Distance=std::ptrdiff_t,typename Object,typename Compare>
void adjust_heap(Iter first, Distance holeIndex, Distance len, Object value,Compare& comp )
{
    Object tmp = std::move(*(first + holeIndex));
    Distance child;

    for( ;left_child(holeIndex) < len; holeIndex = child ) {
        child = left_child(holeIndex);

        if( child != len - 1 && comp(*(first + child), *(first + child + 1)))
            ++child;

        if( comp(tmp, *(first + child )))
            *(first+holeIndex) = std::move(*(first+child));
        else
            break;
    }
    *(first+holeIndex) = std::move(tmp);
}

template <typename Iter, typename Compare>
void pop_heap(Iter first, Iter last, Compare& comp )
{
    if( 1 >= last - first )
        return;

    std::swap(*first, *(last - 1));

    if ( 2 == last - first )
      return;

    mystd::adjust_heap(first, static_cast<std::ptrdiff_t>(0), last - first  - 1,*(first), comp);
}

template <typename Iter, typename Compare>
void sort_pop(Iter first, Iter last, Compare& comp )
{
    while( last - first > 1 ) {
        mystd::pop_heap(first, last, comp);
        --last;
    }
}

template <typename Iter, typename Distance=std::ptrdiff_t, typename Compare>
void make_heap(Iter first,Iter last, Distance*, Compare& comp)
{
    if( last - first < 2 )
        return;

    Distance len = last - first;
    for( Distance i = (len - 2) / 2; i >= 0; i-- ) // (len - 1 ) - 1 == len - 2
        mystd::adjust_heap(first, static_cast<std::ptrdiff_t>(i), static_cast<std::ptrdiff_t>( len ) , *(first), comp);
}


template <typename Iter, typename Compare>
void make_heap(Iter first,Iter last, Compare& comp)
{
    mystd::make_heap(first, last, static_cast<std::ptrdiff_t*>(0), comp);
}

}

#endif
