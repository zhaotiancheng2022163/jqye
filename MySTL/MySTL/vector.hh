#ifndef VECTOR_HH
#define VECTOR_HH


#include <memory>
#include <initializer_list>
#include <cassert>
#include <memory>
#include <algorithm>
#include <type_traits>
#include <stdexcept>

namespace mystd {
template <class Object>
size_t distance_u(Object first, Object last)
{
  size_t n = 0;
  while (first != last){
    ++first;
    ++n;
  }
  return n;
}


template<class Object>
class vector{
    using iterator = Object*;
    using const_iterator =  const Object*;
    struct reverse_iterator;
    struct const_reverse_iterator;
  private:
    iterator theBegin;
    iterator theEnd;
    iterator theCap;
    std::allocator<Object> alloc;

  public:
    vector()
    {
        init(0,0);
    }

    explicit vector(size_t n )
    {
        fill_init(n,Object());
    }

    vector(size_t n, const Object& value)
    {
        fill_init(n, value);
    }

    vector( const vector& rhs )
    {
        if( this != &rhs ){
            range_init(rhs.theBegin, rhs.theEnd);
        }
    }

    vector( vector&& rhs ) noexcept
            :theBegin(rhs.theBegin), theEnd(rhs.theEnd), theCap(rhs.theCap)
    {
        rhs.theBegin = nullptr;
        rhs.theEnd   = nullptr;
        rhs.theCap   = nullptr;
    }

    vector(std::initializer_list<Object> list)
    {
        range_init(list.begin(), list.end());
    }


    // SFINAE技术区分两个函数
    template <class Iter, typename std::enable_if<!std::is_integral<Iter>::value>::type* = nullptr>
    vector(Iter first, Iter last)
    {
        assert(!(last < first));
        range_init(first, last);
    }

    // 2.capacity() < rhs.capacity() ==  size() < rhs.capacity() 不能作为标准1.不明确 2. 一旦条件确定, 那么就可以, 没必要
    // 3.size()  < rhs.size()        == size < rhs.capacity   不能作为标准
    // 4.size() < rhs.capacity()     == size < rhs.size() ==> capacity < rhs.capacity  不能作为标准
    // 通过分析, 以是否放得下将复制的元素为分配空间的依据, 我们大概可以知道有两种情况:
    // 1. 需要释放现有内存重新分配空间的.重新划分内存复制元素, size和capacity和原来一样
    //        a. capacity() < rhs.size() ==> 必定重新划分空间
    // 2. 不需要释放现有内存, 可以直接复制的. 但可能需要清楚原来多余的元素.
    //        a. size()  > rhs.size()    ==> 需要清空后续
    //        d. capacity() > size > rhs.capacity() > rhs.size()
    vector& operator=(const vector& rhs)
    {
      if (this != &rhs) {
          const size_t theCapacity = capacity();
          const size_t theSize     = size();
          const size_t rhsSize     = rhs.size();
          const size_t rhsCapacity = rhs.capacity();

          if (theCapacity < rhsSize) {    //分配空间
              destroy(theBegin, theEnd);
              alloc.deallocate(theBegin, theCapacity);
              init(rhsSize, rhsCapacity);
              uncopy(rhs.theBegin, rhs.theEnd, theBegin);
          } else if (theSize > rhsSize) { //消除尾巴
              uncopy(rhs.theBegin, rhs.theEnd, theBegin);
              for( auto i = theBegin + theSize; i < theBegin + rhsSize; i++)
                  std::allocator_traits<std::allocator<Object>>::destroy(alloc,  i);
              theEnd = theBegin + rhsSize;
          } else {                        //直接复制 theCapacity > rhsSize
              uncopy(rhs.theBegin, rhs.theEnd, theBegin);
              theEnd = theBegin + rhsSize;
          }
      }
      return *this;
    }


    vector& operator=(vector&& rhs)
    {
        destroy(theBegin, theEnd);
        alloc.deallocate(theBegin, static_cast<size_t>(theCap - theBegin));
        theBegin             = rhs.theBegin;
        theEnd               = rhs.theEnd;
        theCap               = rhs.theCap;
        rhs.theBegin         = nullptr;
        rhs.theEnd           = nullptr;
        rhs.theCap           = nullptr;

        return *this;
    }

    vector& operator=(std::initializer_list<Object> list)
    {
        vector tmp(list.begin(), list.end());
        swap(tmp);
        return *this;
    }

    ~vector()
    {
        if( theBegin != nullptr ) {
            destroy(theBegin, theEnd);
            // alloc.deallocate(_begin, (_cap - _begin));
            alloc.deallocate(theBegin, (theCap - theBegin));
            theBegin = nullptr;
            theEnd   = nullptr;
            theCap   = nullptr;
        }
    }

  public:
    iterator begin() noexcept
    {
        return theBegin;
    }
    iterator end() noexcept
    {
        return theEnd;
    }

    const_iterator begin() const noexcept
    {
        return theBegin;
    }
    const_iterator end() const noexcept
    {
        return theEnd;
    }

    const_iterator cbegin() const noexcept
    {
        return theBegin;
    }
    const_iterator cend() const noexcept
    {
        return theEnd;
    }

    // 特性相关操作
    size_t capacity() const
    {
        return theCap - theBegin;
    }

    size_t size() const
    {
        return theEnd - theBegin;
    }
    size_t max_size()
    {
        return static_cast<size_t>(-1) / sizeof(Object);
    }
    bool empty() const
    {
        return theBegin == theEnd;
    }

    void clear()
    {
        erase(begin(), end());
    }

    // _end <= _cap 是由insert, push_back等函数保证的.
    void reserve( size_t n)
    {
        if( n > capacity() ){
            iterator oldBegin = theBegin;
            iterator oldEnd = theEnd;
            iterator oldCap = theCap;
            try{
                init(size(), n);
                unmove(oldBegin, oldEnd, theBegin);
                alloc.deallocate(oldBegin, oldCap - oldBegin);
            } catch (...) {
                theBegin = oldBegin;
                theEnd = oldEnd;
                theCap = oldCap;
                throw;
            }
        }
    }

    void shrink_to_fit()
    {
        iterator oldBegin = theBegin;
        iterator oldEnd = theEnd;
        iterator oldCap = theCap;
        const size_t theSize = size();

        if( size() < capacity() )
            try {
                init(theSize, theSize);
                unmove(oldBegin, oldEnd, theBegin);
                destroy(oldBegin, oldEnd);
                alloc.deallocate(oldBegin, (oldCap - oldBegin));
            } catch (...) {

                theBegin = oldBegin;
                theEnd = oldEnd;
                theCap = oldCap;
                throw;
            }

    }

    void resize(size_t size)
    {
        resize(size, Object());
    }

    void resize(size_t new_size, const Object&value)
    {
        if(new_size < size() ){
            erase( theBegin + new_size, theEnd);
        }else if( new_size <= capacity() ) {
            iterator oldEnd = theEnd;
            theEnd = theBegin + new_size;
            std::uninitialized_fill(oldEnd, theEnd, value);
        }else{
            try{
                iterator tmp = alloc.allocate(new_size);
                unmove(theBegin, theEnd, tmp);
                std::uninitialized_fill(tmp+size(), tmp+new_size, value);
                alloc.deallocate(theBegin,capacity());
                theBegin = tmp;
                theEnd = theBegin + new_size;
                theCap = theBegin + new_size;
            } catch (...) {
                throw;
            }
        }
    }

    void reverse()
    {
        std::reverse(begin(), end());
    }


    // 元素操作
    Object &operator[](size_t n)
    {
        assert(n < size());
        return *(theBegin + n);
    }

    const Object &operator[](size_t n) const
    {
        assert(n < size());
        return *(theBegin + n);
    }

    Object& at(size_t n )
    {
        if ( n >= size() )
            throw std::out_of_range("terminate called after throwing an instance of 'std::out_of_range'\
                       what():  vector::_M_range_check: __n (which is 10) >= this->size() (which is 9)");
        return (*this)[n];
    }

    const Object& at(size_t n ) const
    {
        if ( n >= size() )
            throw std::out_of_range("terminate called after throwing an instance of 'std::out_of_range'\
                       what():  vector::_M_range_check: __n (which is 10) >= this->size() (which is 9)");
        return (*this)[n];
    }

    Object* data()
    { return theBegin; }
    const Object *data() const
    { return theBegin; }

    Object &front()
    {
        assert(!empty());
        return *theBegin;
    }
    const Object &front() const
    {
        assert(!empty());
        return *theBegin;
    }
    Object &back()
    {
        assert(!empty());
        return *(theEnd - 1);
    }
    const Object &back() const
    {
        assert(!empty());
        return *(theEnd - 1);
    }

    // 赋值操作, 将覆盖容器中原有的内容
    void assign(std::initializer_list<Object> list)
    {
        vector tmp(list.begin(), list.end());
        swap(tmp);
    }

    // template<typename Iter>
    void assign(iterator first, iterator last)
    {
        assert( first < last );
        const size_t cap = capacity();
        const size_t len = mystd::distance_u(first, last);

        if( cap < len ){
            iterator tmp = nullptr;

            try{
                tmp = alloc.allocate(len);
                uncopy(first, last, tmp);
            }catch(...){
                if( tmp )
                    alloc.deallocate(tmp, len);
                throw;
            }

            destroy(theBegin, theEnd);
            alloc.deallocate(theBegin,cap);

            theBegin = tmp;
            theEnd = tmp + len;
            theCap = tmp + len;

        }else if( size() > len ) {
            iterator current = theBegin;

            for(; first != last; first++, current++ ){
                *current = *first;
            }

            erase(current, theEnd);
            // _end = _begin + len;
        }else{
            iterator current = theBegin;

            for(; first != last; first++, current++ ){
                *current = *first;
            }
            theEnd = current;
        }
    }

    void assign(size_t n, const Object &value)
    {
        if (n > capacity()) {
            vector tmp(n, value);
            swap(tmp);
        } else if (n > size()) {
            std::fill(begin(), end(), value);
            theEnd = std::uninitialized_fill_n(theEnd, n - size(), value);
        } else {
            erase(std::fill_n(theBegin, n, value), theEnd);
        }
    }

    // 交换操作
    void swap(vector<Object>& v)
    {
        if( this != &v ) {
            std::swap(theBegin, v.theBegin);
            std::swap(theEnd, v.theEnd);
            std::swap(theCap, v.theCap);
        }
    }

    // 比较操作
    bool operator ==(const vector<Object> & rhs) const
    {
        return theBegin == rhs.theBegin && theEnd == rhs.theEnd && theCap == rhs.theCap;
    }
    bool operator != (const vector<Object> & rhs) const
    {
        return theBegin != rhs.theBegin || theEnd != rhs.theEnd || theCap != rhs.theCap;
    }

    // 插入和删除
    void push_back(const Object& value)
    {
        if( theEnd < theCap ) {
            *theEnd = value;
            theEnd++;
        }else {
            reserve(get_new_cap(capacity()));
            *theEnd = value;
            theEnd++;
        }
    }

    void push_back(Object&& value)
    {
        emplace_back(std::move(value));
    }


    template <class ...Args>
    void emplace_back(Args&& ...args)
    {
        if (theEnd < theCap)
        {
            std::allocator_traits<std::allocator<Object>>::construct(alloc,theEnd,std::forward<Args>(args)...);
            theEnd++;
        }
        else
        {
            reallocate_emplace(theEnd, std::forward<Args>(args)...);
        }
    }

    iterator insert(const_iterator pos, const Object& value)
    {
        assert( pos <= theEnd && pos >= theBegin );
        iterator xpos = const_cast<iterator>(pos);
        const size_t len  = pos - theBegin;
        // const size_t len2 = _end - xpos;

        if( theEnd < theCap ){
            iterator tmp = theEnd;
            for( iterator i = tmp; i > xpos; i-- ){
                *i = std::move(*(--tmp));
            }
            *xpos = value;
            theEnd++;
            return xpos;
        }else{
            iterator newBegin = alloc.allocate(2*capacity());
            iterator tmp1 = newBegin;
            iterator tmp2 = newBegin + len;
            const size_t siz = size();
            const size_t cap = capacity();

            for(size_t i = 0; i < len; i++ ) {
                *(tmp1 + i) = std::move(*(theBegin + i));
            }

            *tmp2 = value;

            for(int i = 0; i < theEnd - xpos; i++ ) {
                *( tmp2 + i + 1) = std::move(*(xpos+i));
            }

            alloc.deallocate(theBegin,capacity());
            theBegin = newBegin;
            theEnd = newBegin + siz + 1;
            theEnd = newBegin + 2*cap;

            return theBegin + len;
        }

    }

    template<class ...Args>
    iterator emplace (iterator pos, Args&&...args)
    {
        assert(pos >= theBegin && pos < theEnd );
        iterator xpos = const_cast<iterator>(pos);
        const size_t n = xpos - theBegin;
        if( theEnd != theCap && xpos == theEnd)
        {
            std::allocator_traits<std::allocator<Object>>::construct(alloc,theEnd,std::forward<Args>(args)...);
            ++theEnd;
        }
        else if (theEnd != theCap)
        {
            theEnd++;
            std::copy_backward(xpos, theEnd - 1, theEnd);
            std::allocator_traits<std::allocator<Object>>::construct(alloc,xpos,std::forward<Args>(args)...);
        }
        else
        {
            reallocate_emplace(xpos, std::forward<Args>(args)...);
        }
        return begin()  + n;

    }


    // SFINAE技术, 区分特定模板
    // C++11写法
    // template <class Iter, typename std::enable_if<!std::is_integral<Iter>::value>::type* = nullptr>
    // C++14写法
    template <class Iter,
          typename = std::enable_if_t<!std::is_integral<Iter>::value>>
    iterator insert(iterator pos, Iter first, Iter last)
    {
        assert(pos >= theBegin && pos <= theEnd && first <= last );

        if( first == last )
            return pos;

        const size_t len = last - first;

        if( static_cast<size_t>(theCap - theEnd) >= len ){

            iterator newEnd = theEnd + len;
            iterator tmp    = theEnd - 1;
            iterator i      = newEnd - 1;

            for( ; i >= pos && tmp >= theBegin;) {
                *i = std::move(*tmp);
                --tmp;
                --i;
            }

            for( Iter j = last - 1;  j >= first; j-- ) {
                *i = *j;
                i--;
            }
            theEnd = newEnd;
            return i + 1;
        }else{

            iterator oldBegin = theBegin;
            iterator oldEnd   = theEnd;
            iterator oldCap   = theCap;

            try{
                init(size(), get_new_cap(size() + len));
            } catch (...) {
                theBegin = oldBegin;
                theEnd = oldEnd;
                theCap = oldCap;
                throw;
            }

            iterator newBegin = theBegin;

            for( iterator i = oldBegin; i < pos; i++ ) {
                *newBegin = std::move(*i);
                ++newBegin;
            }

            iterator tmp = newBegin;

            for( auto j = first; j < last; j++ ) {
                *newBegin = *j;
                ++newBegin;
            }

            for( auto k = pos; k < oldEnd; k++ ) {
                *newBegin = std::move(*k);
                ++newBegin;
            }

            theEnd = newBegin;
            alloc.deallocate(oldBegin, (oldCap - oldBegin));

            return tmp;
        }
    }

    iterator insert(iterator pos, size_t n, const Object& value)
    {
        assert(pos >= theBegin && pos <= theEnd && 0 <= n );

        if( 0 == n )
            return pos;

        if( n <= capacity() - size() ) {

            // iterator tmp    = _end - 1;
            // for( int i = n; i > 0; i-- ) {
            //     *(tmp + i) = std::move(*(pos + i ));
            // }

            iterator newEnd = theEnd + n;
            iterator newEle = newEnd - 1;  // _end - pos 为移动的数量, n为移动的距离
            for( int i = theEnd - pos; i > 0; i-- ) {
                *newEle  = *(newEle - n);
                --newEle;
            }

            for( size_t j = 0; j < n; j++  ) {
                *(pos + j) = value;
            }

            theEnd = theEnd + n;

            return pos;

        } else {
            iterator oldBegin = theEnd;
            iterator oldEnd   = theEnd;
            iterator oldCap   = theCap;
            try{
                init( size(), get_new_cap(size() + n ));
            }catch(...){
                theBegin = oldBegin;
                theEnd   = oldEnd;
                theCap   = oldCap;
                throw;
            }

            iterator newBegin = theBegin;
            iterator tmp = oldBegin;
            for( ; tmp < pos; tmp++ ) {
                *newBegin = std::move(*tmp);
                newBegin++;
            }

            iterator xpos = newBegin;

            for( int i = n; i > 0; i-- ) {  // n代表放的数量
                *newBegin = value;
                ++newBegin;
            }

            for( ; tmp < theEnd; tmp ++ ) {
                *newBegin = std::move(*tmp);
                ++newBegin;
            }

            theEnd = newBegin;

            return xpos;
        }

    }

    void pop_back()
    {
        if( theEnd > theBegin ) {
            erase(theEnd - 1);
        }
    }

    iterator erase(iterator pos)
    {
        assert( pos < theEnd && pos >= theBegin );
        auto tmp = pos;
        for(auto i = pos + 1 ; i != theEnd; i++){
            *pos = std::move(*i);
            ++pos;
        }
        destroy(theEnd - 1, theEnd);
        theEnd--;
        return tmp;
    }

    iterator erase(iterator first, iterator last)
    {
        assert( first >= theBegin && last <= theEnd && last >= first);
        const auto n = first - begin();
        destroy(unmove(last, theEnd, first), theEnd);
        theEnd = theEnd - (last - first);
        return theBegin + n;
    }

  private:
    void init( size_t size = 0, size_t cap = 16 ) //1
    {
        assert(size <= cap );
        try {
            theBegin   = alloc.allocate(cap);
            theEnd     = theBegin + size;
            theCap     = theBegin + cap;
        } catch (...) {
            alloc.deallocate(theBegin,cap);
            theBegin = nullptr;
            theEnd   = nullptr;
            theCap   = nullptr;
            throw;
        }
    }

    void fill_init(size_t n, const Object& value )
    {
        const size_t capacitySize = std::max(static_cast<size_t>(16), n );
        init(n,capacitySize);
        std::uninitialized_fill_n(theBegin, n, value); //复制给定值value到始于first的未初始化内存区域的首count个元素
    }

    template<typename Object1>
    void range_init(Object1 first, Object1 last)
    {
        const size_t len = mystd::distance_u(first, last);
        const size_t capacitySize = std::max( len, static_cast<size_t>(16));
        init(len, capacitySize);
        uncopy( first, last, theBegin );
    }

    void destroy(iterator first, iterator last)
    {
        assert( first <= last );
        for( auto i = first; i < last; i++ )
            std::allocator_traits<std::allocator<Object>>::destroy(alloc,  i);
    }


    template <class ...Args>
    void reallocate_emplace(iterator pos, Args&& ...args)
    {
        const auto newSize = get_new_cap(1);
        iterator newBegin = alloc.allocate(newSize);
        iterator newEnd = newBegin;
        try{
            newEnd = unmove(theBegin, pos, newBegin);
            std::allocator_traits<std::allocator<Object>>::construct(alloc,newEnd,std::forward<Args>(args)...);
            ++newEnd;
            newEnd = unmove(pos, theEnd, newEnd);
        } catch (...)
        {
            alloc.deallocate(newBegin, newSize);
            throw;
        }
        alloc.deallocate(theBegin, theCap - theBegin);
        theBegin = newBegin;
        theEnd = newEnd;
        theCap = newBegin + newSize;
    }

    size_t get_new_cap(size_t addSize)
    {
        const auto oldSize = capacity();
        assert(oldSize < max_size() - addSize);

        if (oldSize > max_size() - oldSize / 2){
            return oldSize + addSize > max_size() - 16
            ? oldSize + addSize : oldSize + addSize + 16;
        }
        const size_t newSize = oldSize == 0
            ? std::max(addSize, static_cast<size_t>(16))
            : std::max(oldSize + oldSize / 2, oldSize + addSize);
        return newSize;
    }

    template<typename Object1, typename Object2>
    Object2 uncopy(Object1 first, Object1 last, Object2 begin)
    {
        for( Object1 i = first; i < last; i++ ){
            *begin = *i;
            begin++;
        }
        return begin;
    }

    template<typename Object1, typename Object2>
    Object2 unmove(Object1 first, Object1 last, Object2 begin)
    {
        for( auto i = first; i < last; i++ ){
            *begin = std::move(*i);
            begin++;
        }
        return begin;
    }


};

// 重载比较操作符
template <class Object>
bool operator==(const vector<Object>& lhs, const vector<Object>& rhs)
{
    return lhs == rhs;
}

template <class Object>
bool operator<(const vector<Object>& lhs, const vector<Object>& rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <class Object>
bool operator!=(const vector<Object>& lhs, const vector<Object>& rhs)
{
    return !(lhs == rhs);
}

template <class Object>
bool operator>(const vector<Object>& lhs, const vector<Object>& rhs)
{
    return rhs < lhs;
}

template <class Object>
bool operator<=(const vector<Object>& lhs, const vector<Object>& rhs)
{
    return !(rhs < lhs);
}

template <class Object>
bool operator>=(const vector<Object>& lhs, const vector<Object>& rhs)
{
    return !(lhs < rhs);
}

// 重载 mystd 的 swap
template <class Object>
void swap(vector<Object>& lhs, vector<Object>& rhs)
{
    lhs.swap(rhs);
}


}
#endif
