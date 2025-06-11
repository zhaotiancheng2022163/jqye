#ifndef DEQUE_HH
#define DEQUE_HH

#include <initializer_list>
#include <memory>
#include <utility>
#include <cstddef>
#include <iterator>
#include <algorithm>
#include <cassert>

namespace mystd {
template<class Iter>
struct iterator_traits {
  using iterator_category = typename Iter::iterator_category;
};

template <class Object>
struct iterator_traits<Object *> {
    using iterator_category = std::random_access_iterator_tag;
};
template <class Iter, class Distance>
void advance_user(Iter& i, Distance n)
{
  if (n >= 0)
    while (n--)  ++i;
  else
    while (n++)  --i;
}


#ifndef DEQUE_MAP_INIT_SIZE
#define DEQUE_MAP_INIT_SIZE 8
#endif

template <class Object>
size_t distance_deque(Object first, Object last)
{
  size_t n = 0;
  while (first != last){
    ++first;
    ++n;
  }
  return n;
}

template<class Object>
struct deque_default_size{
    static const size_t value = sizeof(Object) < 256 ? 4096 / sizeof(Object) : 16;
};

template<class Object, class Ref, class Ptr>
struct deque_iterator {
    using iterator_category = std::random_access_iterator_tag;

    using iterator = deque_iterator<Object, Object&, Object*>;
    using const_iterator = deque_iterator<Object, const Object&, const Object*>;
    using self = deque_iterator;


    using difference_type = ptrdiff_t;
    using value_type      = Object;
    using pointer         = Ptr;
    using reference       = Ref;
    using size_type       = size_t;
    using value_pointer   = Object* ;
    using map_pointer     = Object**;


    static const size_t buffer_size = deque_default_size<Object>::value;

    Object* first;
    Object* last;
    Object* cur;
    Object** node;

    // 五大函数
    deque_iterator() noexcept
            :first(nullptr), last(nullptr), cur(nullptr), node(nullptr) {}

    deque_iterator(Object* v, Object** n)
            :first(*n), last(*n+buffer_size), cur(v), node(n) {}

    deque_iterator(const iterator &rhs)
            :first(rhs.first), last(rhs.last), cur(rhs.cur), node(rhs.node) {}

    deque_iterator(const const_iterator& rhs )
            :first(rhs.first), last(rhs.last), cur(rhs.cur), node(rhs.node) {}


    // 这里或许使用self会好一些. 这样可以避免类型转换问题。比如，当`self`是`const_iterator`时，拷贝构造函数参数应该是`const self&`，而不是`const iterator&`，否则可能导致无法正确构造。
    // deque_iterator(const self& rhs )
    //         :first(rhs.first), last(rhs.last), cur(rhs.cur), node(rhs.node) {}

    // 移动构造也应该改为self
    deque_iterator(iterator&& rhs)
            :first( rhs.first), last(rhs.last), cur(rhs.cur), node(rhs.node)
    {
        rhs.first = nullptr;
        rhs.last  = nullptr;
        rhs.cur   = nullptr;
        rhs.node  = nullptr;
    }

    // 这里使用self可能比较好一些
    self& operator=(const self& rhs)
    {
        if( this != &rhs ) {
            first = rhs.first;
            last  = rhs.last;
            cur   = rhs.cur;
            node  = rhs.node;
        }
        return *this;
    }

    // 这个移动赋值可以使用上面的哪个来代替
    self& operator=(self&& rhs)
    {
        first = rhs.first;
        last = rhs.last;
        cur = rhs.cur;
        node = rhs.node;

        rhs.first = nullptr;
        rhs.last  = nullptr;
        rhs.cur   = nullptr;
        rhs.node  = nullptr;

        return *this;
    }

    ~deque_iterator() {}


    // 运算符重载
    reference operator*() const
    {
        return *cur;
    }

    pointer operator->() const
    {
        return  &(operator*());
        // return cur; // 为统一, 使用上面的写法
    }

    // 跳转下一个节点, 辅助iterator运算, 所以不用为cur赋值
    void set_node(Object** newNode)
    {
        node = newNode;
        first = *newNode;
        last = first + buffer_size;
    }

    self& operator++()
    {
        ++cur;

        if( cur == last ) {
            set_node(node + 1);
            cur = first;
        }

        return *this;
    }

    self operator++(int)
    {
        self tmp = *this;
        ++*this;
        return tmp;
    }

    self& operator--()
    {
        if( cur == first ) {
            set_node( node - 1 );
            cur = last;
        }
        cur--;

        return *this;
    }

    self operator--(int)
    {
        self tmp = *this;
        --*this;
        return tmp;
    }

    ptrdiff_t operator-(const self& x ) const
    {
        return static_cast<ptrdiff_t>(buffer_size)*( node - x.node - 1 )
                + ( cur - first ) + ( x.last - x.cur );
    }

    self& operator+=(ptrdiff_t n)
    {
        // 巧妙安排, 可以接受n是正数或是负数
        // 而且这种补全的思想在这种时候巧妙的规避
        // 总偏移
        const ptrdiff_t offset = n + ( cur - first );

        // 分类讨论
        if ( offset >= 0 && offset < static_cast<ptrdiff_t>(buffer_size) ) {
            cur += n;
        }else {
            // 又一个巧妙的安排, 可以将正负两种情况融入到一种情况中.
            // 边界处理：通过整数除法和取模操作确保正负偏移的正确跳转
            // -offset - 1 是为了避免当偏移量恰好是 buffer_size 的整数倍时，错误地多计算一个块, 确保余数范围在 [0, buffer_size - 1]
            // 向下取整,确保计算结果正确
            // 最终结果需要取负数并减 1,以对齐正向移动的逻辑

            ptrdiff_t node_offest = offset > 0
                                 ? offset / static_cast<ptrdiff_t>(buffer_size)
                                 : -static_cast<ptrdiff_t>((-offset - 1)/buffer_size)-1;

            set_node(node+node_offest);

            cur = first + ( offset - node_offest * static_cast<ptrdiff_t>(buffer_size));
        }

        return *this;
    }

    self operator+(ptrdiff_t n) const
    {
        self tmp = *this;
        return tmp += n;
    }

    self& operator-=(ptrdiff_t n)
    {
        return *this += -n;
    }

    self operator-(ptrdiff_t n) const
    {
        self tmp = *this;
        return tmp -= n;
    }

    Ref operator[](ptrdiff_t n) const
    {
        return *(*this+n);
    }

    // 比较操作符
    bool operator==(const self& rhs) const
    {
        return cur == rhs.cur;
    }
    bool operator!=(const self& rhs ) const
    {
        return !(*this==rhs);
    }
    bool operator<(const self& rhs ) const
    {
        return node == rhs.node ? ( cur < rhs.cur ) : ( node < rhs.node);
    }
    bool operator<=(const self& rhs) const
    {
        return !( rhs < *this );
    }
    bool operator>(const self& rhs) const
    {
        return rhs < *this;
    }
    bool operator>=(const self& rhs) const
    {
        // return rhs <= *this;
        return !(*this < rhs);
    }
};

} // namespace mystd

// 在全局命名空间中特化 std::iterator_traits
namespace std {
    template <typename Object, typename Ref, typename Ptr>
    struct iterator_traits<mystd::deque_iterator<Object, Ref, Ptr>> {
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = Object;
        using difference_type   = ptrdiff_t;
        using pointer           = Ptr;
        using reference         = Ref;
    };
}

namespace mystd {

template<class Object, class Allocator=std::allocator<Object>>
class deque{
  public:
    using iterator = deque_iterator<Object , Object&, Object*>;
    using const_iterator = deque_iterator<Object, const Object&, const Object*>;

    Allocator get_allocator() { return Allocator(); }

    static const size_t buffer_size = deque_default_size<Object>::value;

  private:
    Allocator alloc;
    std::allocator<Object*> mapAlloc;
    iterator theBegin;
    iterator theEnd;
    Object** map;
    size_t mapSize;

  public:

    // 五大函数
    deque()
    {
        init(0, Object{});
    }

    explicit deque( size_t n )
    {
        init( n, Object{});
    }

    deque( size_t n, const Object& value )
    {
        init( n, value );
    }



    // template <class Iter,
    //           typename std::enable_if<std::is_base_of<input_iterator_tag,
    //                                                   typename iterator_traits<Iter>::iterator_category_user>::value, int>::type = 0>
    template <class Iter, typename std::enable_if<!std::is_integral<Iter>::value>::type* = nullptr>
    deque(Iter first, Iter last)
    {
        using category = typename std::iterator_traits<Iter>::iterator_category;
        copy_init(first, last,category{} );
    }

    deque(std::initializer_list<Object> list)
    {
        copy_init(list.begin(), list.end(), std::forward_iterator_tag{} );
    }

    deque(const deque& rhs )
    {
        copy_init(rhs.begin(), rhs.end(), std::forward_iterator_tag{} );
    }

    deque(deque&& rhs) noexcept
            :theBegin(std::move(rhs.theBegin)),
             theEnd(std::move(rhs.theEnd)),
             map(rhs.map),
             mapSize(rhs.mapSize)
    {
        rhs.map = nullptr;
        rhs.mapSize = 0;
    }

    deque& operator=(const deque& rhs)
    {
        if( this != &rhs ) {
            const size_t len = size();
            if( len > rhs.size() ) {
                erase(std::copy(rhs.theBegin, rhs.theEnd, theBegin), theEnd);
            }else{
                iterator middle = rhs.theBegin + static_cast<ptrdiff_t>(len);
                std::copy(rhs.theBegin, middle, theBegin);
                insert(theEnd, middle, rhs.theEnd);
            }
        }
        return *this;
    }

    deque& operator=( deque&& rhs )
    {
        clear();
        theBegin    = std::move(rhs.theBegin);
        theEnd      = std::move(rhs.theEnd);
        map         = rhs.map;
        mapSize     = rhs.mapSize;
        rhs.map     = nullptr;
        rhs.mapSize = 0;
        return *this;
    }

    deque& operator=(std::initializer_list<Object> list)
    {
        deque tmp(list);
        std::swap(tmp,*this);
        return *this;
    }

    ~deque()
    {
        if( map != nullptr ) {
            clear();
            for( auto cur = theBegin.node; cur < theEnd.node; ++cur )
                alloc.deallocate(*cur, buffer_size);
            *theBegin.node = nullptr;
            *theEnd.node = nullptr;
            mapAlloc.deallocate(map, mapSize);
            map = nullptr;
            mapSize = 0;
        }
    }

  public:
    // 迭代器
    iterator begin() noexcept
    {
        return theBegin;
    }
    iterator begin() const noexcept
    {
        return theBegin;
    }

    iterator end() noexcept
    {
        return theEnd;
    }

    iterator end() const noexcept
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

    // 特性操作
    bool empty() const noexcept
    {
        return theBegin == theEnd;
    }
    size_t size() const noexcept
    {
        return theEnd - theBegin;
    }
    size_t max_size() const noexcept
    {
        return static_cast<size_t>(-1);
    }

    void resize(size_t newSize)
    {
        resize(newSize, Object());
    }
    void resize(size_t newSize, const Object& value)
    {
        const size_t len = size();
        if( newSize < len ){
            erase(theBegin+newSize, theEnd);
        }else{
            insert(theEnd,newSize - len, value);
        }
    }
    void shrink_to_fit() noexcept
    {
        // 至少会留下头部缓冲区
        for( auto cur = map; cur < theBegin.node; cur++ ){
            alloc.deallocate(*cur, buffer_size);
            *cur = nullptr;
        }
        for( auto cur = theEnd.node + 1; cur < map + mapSize; ++cur ) {
            alloc.deallocate(*cur, buffer_size);
            *cur = nullptr;
        }
    }

    // 元素操作
    Object& operator[](size_t n)
    {
        assert( n < size() );
        return theBegin[n];
    }


    const Object& operator[](size_t n) const
    {
        assert( n < size() );
        return theBegin[n];
    }


    Object& at(size_t n)
    {
        assert( n < size() );
        return (*this)[n];
    }
    const Object& at(size_t n) const
    {
        assert( n < size() );
        return (*this)[n];
    }

    Object& front()
    {
        assert( !empty() );
        return *begin();
    }

    const Object& front() const
    {
        assert( !empty() );
        return *begin();
    }

    Object& back()
    {
        assert( !empty() );
        return *(end() - 1 );
    }

    const Object& back() const
    {
        assert( !empty() );
        return *(end() - 1 );
    }

    // 修改容器
    void assign(size_t n, const Object& value )
    {
        fill_assign(n , value);
    }

    // template <class Iter,
    //           typename std::enable_if<std::is_base_of<std::input_iterator_tag,
    //                                                   typename iterator_traits<Iter>::iterator_category_user>::value, int>::type = 0>
    template <class Iter, typename std::enable_if<!std::is_integral<Iter>::value>::type* = nullptr>
    void assign(Iter first, Iter last)
    {

        using category = typename std::iterator_traits<Iter>::iterator_category;
        copy_assign(first, last, category{});
    }

    void assign(std::initializer_list<Object> list)
    {
      copy_assign(list.begin(), list.end(), std::forward_iterator_tag{});
    }

    // emplace_front / emplace_back / emplace

    // 在头部就地构建元素
    template<typename ... Args>
    void emplace_front(Args&& ... args )
    {
        if( theBegin.cur != theBegin.first) {
            std::allocator_traits<std::allocator<Object>>::construct(alloc, theBegin.cur - 1,std::forward<Args>(args)...);
            --theBegin;
        }else{
            require_capacity(1, true);
            try{
            --theBegin;
            std::allocator_traits<std::allocator<Object>>::construct(alloc, theBegin.cur,std::forward<Args>(args)...);

            }catch(...){
                ++theBegin;
                throw;
            }
        }
    }

    // 在尾部就地构建元素
    template<class ... Args>
    void emplace_back(Args&& ... args )
    {
        if( theEnd.cur != theEnd.last - 1 ) {
            std::allocator_traits<std::allocator<Object>>::construct(alloc, theEnd.cur, std::forward<Args>(args)... );
            ++theEnd;
        }else {
            require_capacity(1, false);
            std::allocator_traits<std::allocator<Object>>::construct(alloc, theEnd.cur, std::forward<Args>(args)... );
            ++theEnd;
        }

    }

    // 构建元素
    template<typename ... Args>
    iterator emplace(iterator position, Args&& ... args )
    {
        if( position.cur == theBegin.cur ){
            emplace_front(std::forward<Args>(args)...);
            return theBegin;
        }else if( position.cur == theEnd.cur ){
            emplace_back(std::forward<Args>(args) ... );
            return theEnd - 1;
        }
        return insert_aux(position, std::forward<Args>(args) ... );
    }


    // 插入元素
    void push_front(const Object& value )
    {
        if( theBegin.cur != theBegin.first ){
            std::allocator_traits<std::allocator<Object>>::construct(alloc, theBegin.cur - 1,value);
            --theBegin.cur;
        }else{
            require_capacity(1, true);
            try{
                --theBegin;
                std::allocator_traits<std::allocator<Object>>::construct(alloc, theBegin.cur, value);
            }catch(...){
                ++theBegin;
                throw;
            }
        }
    }

    void push_back(const Object& value )
    {
        if( theEnd.cur != theEnd.last - 1 ){
            std::allocator_traits<std::allocator<Object>>::construct(alloc, theEnd.cur, value );
            ++theEnd.cur;
        }else{
            require_capacity(1, false);
            std::allocator_traits<std::allocator<Object>>::construct(alloc, theEnd.cur, value );
            ++theEnd;
        }
    }

    void push_front(Object&& value )
    {
        emplace_front(std::move(value));
    }

    void push_back(Object&& value )
    {
        emplace_back(std::move(value));
    }


    // 弹出元素
    void pop_front()
    {
        assert(!empty());
        if( theBegin.cur != theBegin.last - 1 ){
            std::allocator_traits<decltype(alloc)>::destroy(alloc, theBegin.cur);
            theBegin++;
        }else{
            std::allocator_traits<decltype(alloc)>::destroy(alloc, theBegin.cur);
            theBegin++;
            destroy_buffer(theBegin.node -1 , theBegin.node - 1 );
        }
    }

    void pop_back()
    {
        assert(!empty());

        if( theEnd.cur != theEnd.first ){
            std::allocator_traits<decltype(alloc)>::destroy(alloc, theEnd.cur - 1);
            theEnd--;
        }else{
            std::allocator_traits<decltype(alloc)>::destroy(alloc, theEnd.cur - 1);
            theEnd--;
            destroy_buffer(theEnd.node + 1, theEnd.node + 1);
        }
    }


    // 插入元素
    iterator insert(iterator position, const Object& x)
    {
        if ( position.cur == theBegin.cur ) {
            push_front( x );
            return theBegin;
        }else if ( position.cur == theEnd.cur ) {
            push_back( x );
            iterator tmp = theEnd;
            tmp--;
            return tmp;
        } else{
            return insert_aux(position, x);
        }

    }

    iterator insert(iterator position, Object&& value )
    {
        if( position.cur == theBegin. cur ) {
            emplace_front(std::move(value));
            return theBegin;
        }else if ( position.cur == theEnd.cur ){
            emplace_back(std::move(value));
            auto tmp = theEnd;
            --tmp;
            return tmp;
        }else{
            return insert_aux(position, std::move(value));
        }
    }

    void insert(iterator position,  size_t n, const Object& value )
    {
        if( position.cur == theBegin.cur ) {
            require_capacity( n, true );
            auto newBegin = theBegin - n;
            std::uninitialized_fill_n( newBegin, n, value );
            theBegin = newBegin;
        }else if(position.cur == theEnd.cur ){
            require_capacity(n, false);
            auto newEnd = theEnd + n;
            std::uninitialized_fill_n( theEnd, n, value );
            theEnd = newEnd;
        }else{
            fill_insert(position, n, value);
        }
    }

    // template <class Iter,
    //           typename std::enable_if<std::is_base_of<std::input_iterator_tag,
    //                                                   typename iterator_traits<Iter>::iterator_category_user>::value, int>::type = 0>
    template <class Iter, typename std::enable_if<!std::is_integral<Iter>::value>::type* = nullptr>
    void insert(iterator position, Iter first, Iter last)
    {
        // 使用 std::iterator_traits 获取迭代器分类标签
        using category = typename std::iterator_traits<Iter>::iterator_category;
        insert_dispatch(position, first, last, category{}); // 传递标签对象
    }

    // 删除元素
    iterator erase( iterator position )
    {
        auto next = position;
        next++;
        const size_t elemBefore = position - theBegin;

        if( elemBefore < size() / 2 ){
            std::copy_backward(theBegin,position, next);
            pop_front();
        }else{
            std::copy(next, theEnd, position);
            pop_back();
        }

        return theBegin + elemBefore;
    }

    iterator erase(iterator first, iterator last )
    {
        if( first == theBegin && last == theEnd ) {
            clear();
            return theEnd;
        }else{
            const size_t len = last - first;
            const size_t elemBefore = first - theBegin;

            if( elemBefore < ( size() - len ) / 2 ) {
                std::copy_backward(theBegin, first, last);
                auto newBegin = theBegin + len;
                for( auto cur = theBegin.cur; cur < newBegin.cur; cur++ )
                    std::allocator_traits<decltype(alloc)>::destroy(alloc, cur);
                theBegin = newBegin;
            }else{
                std::copy(last, theEnd, first);
                auto newEnd = theEnd - len;
                for( auto cur = newEnd.cur; cur < theEnd.cur; cur++ )
                    std::allocator_traits<decltype(alloc)>::destroy(alloc, cur);

                theEnd = newEnd;
            }
            return theBegin + elemBefore;
        }
    }

    void clear() noexcept
    {
        // clear 会保留头部的缓冲区
        // 从容器擦除所有元素.此调用后 size() 返回零
        for( Object** cur = theBegin.node + 1; cur < theEnd.node; ++cur )
            for( size_t i = 0; i < buffer_size; i++)
                std::allocator_traits<decltype(alloc)>::destroy(alloc, *cur+i);

        if( theBegin.node != theEnd.node ){
            for( auto i = theBegin.cur; i < theBegin.last; i++ )
                std::allocator_traits<decltype(alloc)>::destroy(alloc, i);
            for( auto j = theEnd.first; j <= theEnd.cur; j++)
                std::allocator_traits<decltype(alloc)>::destroy(alloc, j);
        }else{
            for( auto i = theBegin.cur; i <= theEnd.cur; i++ )
                std::allocator_traits<decltype(alloc)>::destroy(alloc, i);
        }

        shrink_to_fit();
        theEnd = theBegin;
    }

    void swap(deque& rhs) noexcept
    {
        if( this != &rhs ){
          std::swap(theBegin, rhs.theBegin);
          std::swap(theEnd, rhs.theEnd);
          std::swap(map, rhs.map);
          std::swap(mapSize, rhs.mapSize);
        }
    }

  private:

    /*
     * 帮助函数
     */

    Object** create_map(size_t size)
    {
        Object** tmp = nullptr;
        tmp = mapAlloc.allocate(size);
        for(size_t i = 0; i < size; i++ )
            *(tmp+i) = nullptr;
        return tmp;
    }

    void create_buffer(Object** start, Object** finish)
    {
        Object** cur;
        try{
            for( cur = start; cur <= finish; cur++ ){
                *cur = alloc.allocate(buffer_size);
            }
        }catch(...){
            // 如果抛出异常, 那么意味着分配错误, *cur可以不用管
            while( cur != start ) {
                cur--;
                alloc.deallocate(*cur, buffer_size);
                *cur = nullptr;
            }
            throw;
        }
    }

    void destroy_buffer(Object** start, Object** finish)
    {
        for( Object** cur = start; cur <= finish; cur++ ){
            alloc.deallocate(*cur, buffer_size);
            *cur = nullptr;
        }
    }

    void map_init(size_t count)
    {
        const size_t nNode = count / buffer_size + 1;
        mapSize = std::max(static_cast<size_t>(DEQUE_MAP_INIT_SIZE), nNode+2);
        try{
            map = create_map(mapSize);
        } catch(...) {
            mapAlloc.deallocate(map, mapSize);
            map = nullptr;
            mapSize = 0;
            throw;
        }

        // 让start和finish指向deque node的两端
        // 整数除法不影响, 我们finish的设置可以保证start和finish
        // 中间有nNode个节点
        // finish 是最后一个节点, 为什么? 不应该是最后一个节点的后一个节点吗??
        // start指向第一个节点, finish指向最后一个节点
        Object** start = map + ( mapSize - nNode ) / 2;
        Object** finish = start + nNode - 1;

        try{
            create_buffer(start, finish);
        }catch(...){
            mapAlloc.deallocate( map, mapSize );
            map = nullptr;
            mapSize = 0;
            throw;
        }

        theBegin.set_node(start);
        // theEnd不是指finish+1, 而是指theEnd.cur指向未用过的内存
        theEnd.set_node(finish);
        theBegin.cur = theBegin.first;
        theEnd.cur = theEnd.first + ( count % buffer_size ); //指向最后一个的后一个
    }

    //init 函数
    void init(size_t n, const Object& value)
    {
        map_init(n);

        if( n ) {
            for( auto cur = theBegin.node; cur < theEnd.node; cur++ ) {
                std::uninitialized_fill(*cur, *cur + buffer_size, value);
            }
            std::uninitialized_fill(theEnd.first, theEnd.cur, value);
        }
    }

    template<class Iter>
    void copy_init(Iter first, Iter last, std::input_iterator_tag )
    {
        const size_t n = distance_deque(first, last);
        map_init( n );
        for(; first != last; ++first )
            emplace_back(*first);
    }

    template<class Iter>
    void copy_init(Iter first, Iter last, std::forward_iterator_tag)
    {
        const size_t n = distance_deque(first, last);
        map_init( n );

        for( Object** cur = theBegin.node; cur < theEnd.node; cur++ ){
            auto next = first;
            advance_user(next, buffer_size);
            std::uninitialized_copy(first, next, *cur);
            first = next;
        }
        std::uninitialized_copy(first , last, theEnd.first);
    }

    void fill_assign(size_t n, const Object& value)
    {
        if( n > size() ) {
            std::fill(begin(), end(), value);
            insert(end(), n - size(), value );
        }else{
            erase(begin() + n, end() );
            std::fill( begin(), end(), value);
        }

    }

    template<class Iter>
    void copy_assign(Iter first, Iter last, std::input_iterator_tag)
    {
        iterator begin = theBegin;
        iterator end = theEnd;

        for(; begin != end && first != last; begin++, first++ ) {
            *begin = *first;
        }

        if( begin != end ) {
            erase( begin, end );
        }else{
            insert_dispatch(theEnd, first, last, std::input_iterator_tag{});
        }
    }

    template<typename Iter>
    void copy_assign(Iter first, Iter last, std::forward_iterator_tag)
    {
        const size_t len1 = size();
        const size_t len2 = distance_deque(first, last);

        if( len1 < len2 ){
            Iter next = first;
            advance_user(next, len1);
            std::copy(first, next, theBegin);
            insert_dispatch(theEnd, next, last, std::forward_iterator_tag{});
        }else{
            erase(std::copy(first, last, theBegin), theEnd);
        }
    }

    template<typename ... Args>
    iterator insert_aux(iterator position, Args&& ... args )
    {
        size_t elem_front = position - theBegin;
        Object copyValue = Object(std::forward<Args>(args)...);

        if( elem_front < size() / 2 ) {
            emplace_front(front());
            iterator front1 = theBegin;
            ++front1;
            iterator front2 = front1;
            ++front2;
            position = theBegin + elem_front;
            iterator pos = position;
            //?? 为什么这里是++pos?
            ++pos;
            //?? 为什么这里会是这样, 不应该是front1吗?
            std::copy(front2, pos, front1);
        }else{
            emplace_back(back());
            iterator back1 = theEnd;
            --back1;
            iterator back2 = back1;
            --back2;
            position = theBegin + elem_front;
            std::copy_backward(position , back2, back1);
        }

        *position = std::move(copyValue);
        return position;
    }

    void fill_insert( iterator position, size_t n, const Object& value )
    {
        const size_t elemBefore = position - theBegin;
        const size_t len = size();
        Object copyValue = value;

        if( elemBefore < ( len / 2 ) ) {
            require_capacity( n, true );
            // 防止迭代器失效
            iterator oldBegin = theBegin;
            iterator newBegin = theBegin - n;
            position = oldBegin + elemBefore;
            try {
                if( elemBefore >= n ) {
                    iterator afterBegin = theBegin + n;
                    std::uninitialized_copy(theBegin, afterBegin, newBegin);
                    theBegin = newBegin;
                    std::copy(afterBegin, position, oldBegin);
                    std::fill(position - n, position, copyValue);
                } else {
                    std::uninitialized_fill(
                        std::uninitialized_copy( theBegin, position, newBegin), newBegin, copyValue);
                    theBegin = newBegin;
                    std::fill(oldBegin, position, copyValue);
                }
            } catch (...) {
                if( newBegin.node != theBegin.node )
                    destroy_buffer(newBegin.node, theBegin.node - 1);
                throw;
            }
        } else {
            require_capacity(n, false);
            // 防止迭代器失效
            iterator oldEnd = theEnd;
            iterator newEnd = theEnd + n;
            // 这样写不能使用theEnd, theEnd不指向具体元素
            const size_t elemAfter = len - elemBefore;
            position = theEnd - elemAfter;

            try{
                if( elemAfter > n ) {
                    iterator afterEnd = theEnd - n;
                    std::uninitialized_copy(afterEnd, theEnd, theEnd);
                    theEnd = newEnd;
                    std::copy_backward(position, afterEnd, oldEnd);
                    std::fill(position, position + n, copyValue);
                } else {
                    std::uninitialized_fill(theEnd, position + n, copyValue);
                    // 覆盖元素, 这样做反而比较直观没有那么复杂
                    std::uninitialized_copy(position,theEnd,position + n);
                    theEnd = newEnd;
                    std::fill(position, oldEnd, copyValue);
                }
            } catch( ... ) {
                if( newEnd.node != theEnd.node )
                    destroy_buffer(theEnd.node + 1, newEnd.node );
                throw;
            }
        }
    }

    template<typename Iter>
    void copy_insert(iterator position, Iter first, Iter last, size_t n )
    {
        if( last <= first ) return;
        const size_t elemBefore = position - theBegin;
        const size_t len = size();

        if( elemBefore < len / 2 ){
            require_capacity(n , true);
            // 迭代器可能失效
            iterator oldBegin = theBegin;
            iterator newBegin = theBegin - n;
            position = theBegin + elemBefore;
            try{
                if( elemBefore >= n ){
                    iterator afterBegin = theBegin + n;
                    std::uninitialized_copy(theBegin,afterBegin, newBegin);
                    theBegin = newBegin;
                    std::copy(afterBegin, position, oldBegin);
                    std::copy(first, last, position - n );
                }else{
                    auto middle = first;
                    advance_user(middle, n - elemBefore);
                    std::uninitialized_copy(first, middle,
                                            std::uninitialized_copy(theBegin, position, newBegin));

                    theBegin = newBegin;
                    std::copy(middle, last, oldBegin);
                }
            }catch(...){
                if( newBegin.node != theBegin.node )
                    destroy_buffer(newBegin.node,  theBegin.node - 1);
                throw;
            }
        }else{
            require_capacity(n, false);
            // 迭代器可能失效
            iterator oldEnd = theEnd;
            iterator newEnd = theEnd + n;
            const size_t elemAfter = len - elemBefore;
            position = theEnd - elemAfter;
            try{
                if( elemAfter > n ) {
                    iterator endAfter = theEnd - n;
                    std::uninitialized_copy(endAfter, theEnd, theEnd);
                    theEnd = newEnd;
                    std::copy_backward(position, endAfter, oldEnd);
                    std::copy(first, last, position);
                }else{
                    auto middle = first;
                    advance_user(middle, elemAfter);
                    std::uninitialized_copy(position, theEnd,
                                            std::uninitialized_copy(middle, last, theEnd));
                    theEnd = newEnd;
                    std::copy(first, middle, position);
                }
            }catch(...){
                if( newEnd.node != theEnd.node )
                    destroy_buffer(theEnd.node + 1, newEnd.node );
                throw;
            }
        }
    }

    template<typename Iter>
    void insert_dispatch(iterator position, Iter first, Iter last, std::input_iterator_tag )
    {
        if( last <= first ) return;
        const size_t n = distance_deque(first, last);
        const size_t elemBefore = position - theBegin;

        if( elemBefore  < size() / 2  ) {
            require_capacity(n, true);
        }else{
            require_capacity(n, false);
        }

        position = theBegin + elemBefore;

        Iter cur = last;
        --cur;

        for(int i = 0; i <= n; i++ )
            insert(position, *cur);
    }

    template<typename Iter>
    void insert_dispatch(iterator position, Iter first, Iter last, std::forward_iterator_tag)
    {
        if( last <= first ) return;
        const size_t n = distance_deque(first,last);
        if( position.cur == theBegin.cur ){
            require_capacity(n, true);
            iterator newBegin = theBegin -n;
            try{
                std::uninitialized_copy(first,last,newBegin);
                theBegin = newBegin;
            }catch(...){
                if( newBegin.node != theBegin.node )
                    destroy_buffer(newBegin.node, theBegin.node - 1);
                throw;
            }
        }else if( position.cur == theEnd.cur ) {
            require_capacity(n, false);
            auto newEnd = theEnd + n;
            try{
                std::uninitialized_copy(first, last, theEnd);
                theEnd = newEnd;
            }catch(...){
                if( theEnd.node != newEnd.node )
                    destroy_buffer(theEnd.node+1, newEnd.node);
                throw;
            }
        }else{
            copy_insert(position, first, last, n);
        }
    }

    void require_capacity(size_t n, bool front)
    {
        if( front && static_cast<size_t>( theBegin.cur - theBegin.first ) < n ){
            const size_t needNode = ( n - static_cast<size_t>( theBegin.cur - theBegin.first ) ) / buffer_size + 1;
            // if( theBegin.node - needNode < map  )
            if( needNode > static_cast<size_t>( theBegin.node - map ) ) {
                reallocate_map_at_front(needNode);
                return;
            }
            create_buffer( theBegin.node - needNode, theBegin.node - 1 );
        }else if( !front && static_cast<size_t>( theEnd.last - theEnd.cur - 1 ) < n ){
            const size_t needNode = ( n - static_cast<size_t>( theEnd.last - theEnd.cur - 1 )) / buffer_size + 1;
            if( needNode > static_cast<size_t>( map + mapSize - theEnd.node - 1)) {
                reallocate_map_at_back( needNode );
                return;
            }
            create_buffer( theEnd.node + 1, theEnd.node + needNode );
         }
    }

    void reallocate_map_at_front(size_t needNodeSize)
    {
        const size_t newMapSize = std::max(mapSize << 1, mapSize + needNodeSize + DEQUE_MAP_INIT_SIZE );
        Object** newMap = create_map( newMapSize );
        const size_t oldNodeSize = theEnd.node - theBegin.node + 1;
        const size_t newNodeSize = oldNodeSize + needNodeSize;

        Object** begin  = newMap + ( newMapSize - newNodeSize ) / 2;
        Object** middle = begin + needNodeSize;
        Object** end    = middle + oldNodeSize - 1;

        create_buffer(begin, middle - 1);

        // begin1 <= theEnd; begin1++, begin2++ ) 这里不应该是 <= 吗?
        // 因为theEnd
        // 只是cur不指向当前元素
        for (auto begin1 = theBegin.node, begin2 = middle; begin2 <= end;
             begin1++, begin2++)
          *begin2 = *begin1;

        theBegin = iterator( *middle + ( theBegin.cur - theBegin.first ), middle);
        theEnd   = iterator( *end + ( theEnd.cur - theEnd.first), end );
        mapAlloc.deallocate(map,mapSize);
        mapSize  = newMapSize;
        map      = newMap;
    }


    void reallocate_map_at_back(size_t needNodeSize)
    {
        const size_t newMapSize = std::max(mapSize << 1, mapSize + needNodeSize + DEQUE_MAP_INIT_SIZE );
        Object** newMap = create_map( newMapSize );
        const size_t oldNodeSize = theEnd.node - theBegin.node + 1;
        const size_t newNodeSize = oldNodeSize + needNodeSize;

        Object** begin  = newMap + ( newMapSize - newNodeSize ) / 2;
        Object** middle = begin + oldNodeSize - 1;
        Object** end    = middle + needNodeSize;

        create_buffer(middle + 1, end);

        for( auto begin1 = theBegin.node, begin2 = begin;
             begin2 <= middle; begin1++, begin2++ )
            *begin2 = *begin1;


        theBegin = iterator( *begin + ( theBegin.cur - theBegin.first ), begin);
        theEnd   = iterator( *middle + ( theEnd.cur - theEnd.first), middle );
        mapAlloc.deallocate(map,mapSize);
        mapSize  = newMapSize;
        map      = newMap;
    }

};

template<typename Object>
bool operator==(const deque<Object> &lhs, const deque<Object>& rhs)
{
    return lhs.size() == rhs.size() &&
            std::equal(lhs.begin(), lhs.end(), rhs.begin);
}

template<typename Object>
bool operator<(const deque<Object> &lhs, const deque<Object>& rhs)
{
    return std::lexicographical_compare( lhs.begin(), lhs.end(),
                                         rhs.begin(), rhs.end());
}

template <typename Object>
bool operator!=(const deque<Object>& lhs, const deque<Object>& rhs)
{
    return !(lhs==rhs);
}

template<typename Object>
bool operator>(const deque<Object> &lhs, const deque<Object>& rhs)
{
    return rhs < lhs;
}

template<typename Object>
bool operator<=(const deque<Object> &lhs, const deque<Object>& rhs)
{
    return !(lhs > rhs);
}

template<typename Object>
bool operator>=(const deque<Object> &lhs, const deque<Object>& rhs)
{
    return !(lhs < rhs);
}

template <typename Object>
void swap(const deque<Object>& lhs, const deque<Object>& rhs)
{
    lhs.swap(rhs);
}

}

#endif
