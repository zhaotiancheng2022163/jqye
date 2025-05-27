// list 是环状双向链表
// md, 不用环状简直没法做
#ifndef LIST_HH
#define LIST_HH

#include <cassert>
#include <initializer_list>
#include <memory>

namespace mystd{

template <class Object>
class list{
  public:
    struct listNode;
    struct iterator;
    struct const_iterator;

  private:
    listNode* theNode; // 指向尾部元素较好, 符合迭代器开闭原则
    size_t theSize;
    std::allocator<listNode> node_alloc;
    const size_t nodeSize = sizeof(listNode);

  public:

    // 五大函数
    list()
    {
        init(0 , Object());
    }

    explicit list(size_t n )
    {
        init(n , Object());
    }

    list(size_t n, const Object& value )
    {
        init(n , value);
    }

    template <class Iter, typename std::enable_if<!std::is_integral<Iter>::value>::type* = nullptr>
    list(Iter first, Iter last )
    {
        copy_init(first, last);
    }

    list(std::initializer_list<Object> form)
    {
        copy_init(form.begin(), form.end());
    }

    list(const list& rhs)
    {
        copy_init(rhs.begin(), rhs.end());
    }

    list( list&& rhs )
            :theNode(rhs.theNode),theSize(rhs.theSize)
    {
        rhs.theNode = nullptr;
        rhs.theSize = 0;
    }

    list& operator=( const list& rhs )
    {
        if( this != &rhs ) {
            assign(rhs.begin(), rhs.end());
        }
        return *this;
    }

    list& operator=( list&& rhs )
    {
        clear();
        theNode = rhs.theNode;
        theSize = rhs.theSize;
        rhs.theNode = nullptr;
        rhs.theSize = 0;

        return *this;
    }

    list& operator=(std::initializer_list<Object> form)
    {
        list tmp(form.begin(), form.end());
        swap(tmp);
        return *this;
    }

    ~list()
    {
        if( theNode ) {
            clear();
            node_alloc.deallocate(theNode, nodeSize);
            theNode = nullptr;
        }
    }

  public:
    // 特性操作
    size_t max_size()
    {
        return static_cast<size_t> (-1);
    }

    size_t size() const
    {
        return theSize;
    }

    bool empty() const
    {
        return theSize == 0;
    }

    void clear()
    {
        if( theSize ) {
            listNode* tmp = theNode -> next;
            listNode* tmp1 = tmp;

            while( tmp != theNode ) {
                tmp1 = tmp -> next;
                destroy_node(tmp);
                tmp = tmp1;
            }
            theNode -> unlink();
            theSize = 0;

        }
    }

    void resize( size_t size )
    {
        resize(size, Object());
    }

    void resize( size_t size, const Object& value ) //唉, 看到人家的思路才知道人家有多厉害,循环也起到了判断的作用,不要再忘了
    {                                          // 不过我的也不错
        assert( size >= 0 );

        if( 0 == size ) {
            clear();
        }else if( size < theSize) {
            iterator first = begin();
            for(; size > 0; size-- ){
                first++;
            }
            erase(first, end());
        }else{
            size_t n = size - theSize;
            insert(end(), n, value);
        }
    }

    // 迭代器
    iterator begin() noexcept
    { return theNode -> next ; }

    iterator end() noexcept
    { return theNode; }

    const_iterator begin() const noexcept
    { return theNode -> next; }

    const_iterator end() const noexcept
    { return theNode; }

    const_iterator cbegin() const noexcept
    { return theNode -> next; }

    const_iterator cend() const noexcept
    { return theNode; }

    // 元素操作
    Object& front()
    {
        assert(!empty());
        return theNode -> next -> value;
    }

    const Object& front() const
    {
        assert(!empty());
        return theNode -> next -> value;
    }

    Object& back()
    {
        assert(!empty());
        return theNode -> prev -> value;
    }

    const Object& back() const
    {
        assert(!empty());
        return theNode->prev -> value;
    }


    // 赋值操作
    void assign(size_t n, const Object& value)
    {
        iterator first = begin();
        iterator last = end();

        for( ; n > 0 && first != last; n--, first++, theSize++ )
            *first = value;

        if( n > 0 ) {
            insert( last, n, value);
        }else {
            erase( first, last);
        }
    }

    template <class Iter, typename std::enable_if<!std::is_integral<Iter>::value>::type* = nullptr>
    void assign( Iter first, Iter last )
    {
        copy_assign(first, last);
    }

    void assign(std::initializer_list<Object> form)
    {
        copy_assign(form.begin(), form.end());
    }

    // 插入和删除
    template<typename ... Args>
    void emplace_front(Args&& ... args )
    {
        listNode* ptr = create_node(std::forward<Args>(args)...);
        link_to_front(ptr, ptr);
        ++theSize;
    }

    template<typename ... Args>
    void emplace_back(Args&& ... args )
    {
        listNode* ptr = create_node(std::forward<Args>(args)...);
        link_to_back(ptr, ptr);
        ++theSize;
    }

    template<typename ... Args>
    iterator emplace(const_iterator pos, Args&& ... args )
    {
        listNode* ptr = create_node(std::forward<Args>(args)...);
        insert_nodes(pos.current, ptr, ptr);
        ++theSize;
        return ptr;
    }

    iterator insert(iterator pos, const Object& value )
    {
        listNode* ptr = create_node(value);
        if( pos.current == theNode ){
            link_to_back(ptr, ptr);
        }else if( pos.current == theNode -> prev ) {
            link_to_front(ptr, ptr);
        }else{
            insert_nodes(pos.current, ptr, ptr);
        }
        ++theSize;
        return ptr;
    }

    iterator insert(iterator pos, const Object&& value )
    {
        listNode* ptr = create_node(std::move(value));
        if( pos.current == theNode ){
            link_to_back(ptr, ptr);
        }else if( pos.current == theNode -> prev ) {
            link_to_front(ptr, ptr);
        }else{
            insert_nodes(pos.current, ptr, ptr);
        }
        ++theSize;
        return ptr;
    }

    // template<typename Object1>
    // iterator insert(iterator pos, Object1&& value ) // 这样写虽然看着简洁, 但是速度比较慢
    // {
    //     listNode* ptr = create_node(std::forward<Object1>(value));
    //     if( pos == end() ){
    //         link_to_back(ptr, ptr);
    //     }else if( pos == begin() ) {
    //         link_to_front(ptr, ptr);
    //     }else{
    //         insert_nodes(pos.current, ptr, ptr);
    //     }
    //     ++theSize;
    //     return ptr;
    // }

    iterator insert( const_iterator pos, size_t n, const Object& value )
    {
        if( 0 == n )
            return pos;
        list<Object> tmp(n, value);
        insert_nodes(pos.current, tmp.theNode->next, tmp.theNode->prev);
        tmp.theNode -> prev = tmp.theNode -> next = nullptr;
        tmp.theNode = nullptr;
        tmp.theSize = 0;

        theSize += n;

        while( n ) {
            --pos;
            --n;
        }
        return pos;
    }

    template <class Iter, typename std::enable_if<!std::is_integral<Iter>::value>::type* = nullptr>
    iterator insert( const_iterator pos, Iter first, Iter last )
    {
        size_t n = 0;
        listNode* pos_ptr = pos.current;
        listNode* tmp = pos.current;

        try{
            for( ; first != last; ++first ) {
                listNode* ptr = create_node(*first);
                pos_ptr = ptr;
                insert_nodes(tmp, ptr, ptr);
                ++n;
            }
            theSize += n;
        }catch(...){ //如果有了异常, 那么我们恢复原来的链表
            // while (true) {
            //     if( tmp -> prve != tmp1 ) {
            //         listNode* tmp2 = tmp -> prev; //销毁的节点.
            //         tmp -> prev = tmp2 -> prev;
            //         tmp2 -> prev -> next = tmp;
            //         std::allocator_traits<std::allocator<listNode>>::destroy(node_alloc, tmp2);
            //         node_alloc.deallocate( tmp2, nodeSize);
            //     }
            // }
            // theSize = size;
            // return tmp;

            //一旦出现异常, 销毁所有的节点.
            if( theNode ) {
                clear();
                node_alloc.deallocate(theNode, nodeSize);
                theNode = nullptr;
            }
        }
        return pos_ptr;
    }


    void push_front( const Object& value )
    {
        listNode* ptr = create_node(value);
        link_to_front(ptr, ptr);
        ++theSize;
    }

    void push_front( const Object&& value )
    {
        emplace_front(std::move(value));
    }

    // template<typename Object1>
    // void push_front( Object1&& value )
    // {
    //     listNode* ptr = create_node(std::forward<Object1>(value));
    //     link_to_front(ptr, ptr);
    //     ++theSize;
    // }

    void push_back( const Object& value )
    {
        listNode* ptr = create_node(value);
        link_to_back(ptr, ptr);
        ++theSize;
    }

    void push_back( const Object&& value )
    {
        emplace_back(std::move(value));
    }

    // template<typename Object1>
    // void push_back( Object1&& value )
    // {
    //     listNode* ptr = create_node(std::forward<Object1>(value));
    //     link_to_back(ptr, ptr);
    //     ++theSize;
    // }

    void pop_front()
    {
        assert(!empty());
        listNode* ptr = theNode -> next;
        uninsert_nodes(ptr, ptr);
        destroy_node(ptr);
        --theSize;
    }

    void pop_back()
    {
        assert(!empty());
        listNode* ptr = theNode -> prev;
        uninsert_nodes(ptr, ptr);
        destroy_node(ptr);
        --theSize;
    }

    iterator erase(iterator pos)
    {
        assert(!empty() && pos != end());
        listNode* ptr = pos.current;
        ++pos;

        uninsert_nodes(ptr, ptr);
        destroy_node(ptr);
        --theSize;

        return pos;
    }

    iterator erase(iterator first, iterator last)
    {
        assert(!empty() && first != last);

        iterator ite = last;
        listNode* ptr1 = first.current;
        listNode* ptr2 = last.current -> prev;

        listNode* tmp = nullptr;

        uninsert_nodes(ptr1, ptr2);
        while( ptr1 != last.current ){
            tmp = ptr1 -> next;
            destroy_node(ptr1);
            --theSize;
            ptr1 = tmp;
        }

        return ite;
    }



    void splice( const iterator pos, list<Object>& rhs)
    {
        splice(pos, rhs, rhs.begin(), rhs.end());
    }

    void splice( /*const*/ iterator pos, list<Object>&rhs, iterator first )
    {
        if( first != rhs.end() && pos != first ){
            list<Object>::listNode* ptr = first.current;

            rhs.uninsert_nodes( ptr, ptr);
            rhs.theSize -= 1;
            insert_nodes(pos.current, ptr, ptr);
            theSize += 1;
        }
    }

    void splice(const iterator pos, list<Object> & rhs, iterator first, iterator last)
    {
        if( first != last && this != &rhs ) {
            size_t n = 0;
            auto i = first;
            for( ;i != last; i++, n++ );

            list<Object>::listNode* first_ptr = first.current;
            list<Object>::listNode* last_ptr  = last.current -> prev;

            rhs.uninsert_nodes( first_ptr, last_ptr);
            rhs.theSize -= n;
            insert_nodes(pos.current, first_ptr, last_ptr);
            theSize += n;
        }
    }

    void remove( const Object& value )
    {
        iterator tmp = begin();
        iterator tmp1 = tmp;
        while (tmp != end()) {
            tmp1++;
            if( *tmp == value )
                erase( tmp );
            tmp = tmp1;
        }
    }

    template<class UnaryPredicate>
    void remove_if(UnaryPredicate p)
    {
        iterator tmp = begin();
        iterator tmp1 = tmp;

        while( tmp != end() ) {
            tmp1++;
            if( p(*tmp) )
                erase(tmp);
            tmp = tmp1;
        }
    }


    void unique()
    {
        iterator tmp = begin();
        iterator tmp1 = tmp;
        tmp1++;

        while( tmp1 != end() ){
            if( *tmp == *tmp1 ) {
                tmp1 = erase(tmp1);
            }else{
                tmp++;
                tmp1++;
            }
        }
    }

    template<class UnaryPredicate>
    void unique(UnaryPredicate p)
    {
        iterator tmp = begin();
        iterator tmp1 = tmp;
        tmp1++;

        while( tmp1 != end() ){
            if( p(*tmp, *tmp1) ) {
                tmp1 = erase(tmp1);
            }else{
                tmp++;
                tmp1++;
            }
        }
    }


    // 交换、反转、排序、归并
    void swap(list& rhs ) noexcept
    {
        std::swap(theNode, rhs.theNode);
        std::swap(theSize, rhs.theSize);
    }

    void reverse()
    {
        if( 1 >= theSize )
            return;

        listNode* first = theNode -> prev;
        listNode* tmp1 = first;
        listNode* tmp = nullptr;

        while( tmp != tmp1 ) {
            tmp = first -> next;
            first -> next = first -> prev;
            first -> prev = tmp;
            first = tmp;
        }
    }

    template<class Compare>
    void merge(list<Object>& rhs, Compare comp)
    {
        if( this == &rhs )
            return;

        iterator tmp = begin();
        iterator tmp1 = rhs.begin();

        while( tmp != end() && tmp1 != rhs.end() ) {
            if(! comp(*tmp, *tmp1) ){ //这里别搞错了, 因为只能朝前面插
                listNode* tmp1_ptr = tmp1.current;
                iterator next = tmp1;
                next++;

                for( ; next != rhs.end() && !comp(*tmp, *next); next++ );

                tmp1 = next;
                --next;

                listNode* next_ptr = next.current;

                rhs.uninsert_nodes(tmp1_ptr, next_ptr);
                insert_nodes(tmp.current, tmp1_ptr, next_ptr);
            }
            tmp++;
        }

        if( tmp1 != rhs.end() ){
            listNode* start = tmp1.current;
            listNode* end = rhs.end().current -> prev;

            rhs.uninsert_nodes(start, end);
            insert_nodes(tmp.current, start, end);
        }

        theSize += rhs.theSize;
        rhs.theSize = 0;
    }


    void merge(list<Object>& rhs)
    {
        merge(rhs, std::less<Object>());
    }

    void sort()
    {
        merge_sort(begin(), end(), theSize, std::less<Object>());
    }

    template<class Compare>
    void sort(Compare comp)
    {
        merge_sort(begin(), end(), theSize, comp);
    }

  private:
    template<class ... Args >
    listNode* create_node(Args&& ... args )
    {
        listNode* tmp = nullptr;
        try{
            tmp = node_alloc.allocate(nodeSize);
            std::allocator_traits<std::allocator<listNode>>::construct(node_alloc, tmp, std::forward<Args>(args)...);
            tmp -> prev = nullptr;
            tmp -> next = nullptr;
        }catch(...){
            node_alloc.deallocate(tmp, nodeSize);
            tmp = nullptr;
            throw;
        }

        return tmp;
    }

    void destroy_node(listNode* ptr )
    {
        std::allocator_traits<std::allocator<listNode>>::destroy(node_alloc, ptr);
        node_alloc.deallocate(ptr, nodeSize);
    }

    // 用 n 个元素初始化容器
    void init(size_t n, const Object& value )
    {
        theNode = node_alloc.allocate(nodeSize);
        theNode -> unlink();
        theSize = n;

        try{
            for( ; n > 0; n-- ) {
                listNode* node = create_node(value); //这里会抛出异常
                link_to_back(node, node); //link_to_front(node, node);也可以
            }
        }catch(...){
            clear();
            std::allocator_traits<std::allocator<listNode>>::destroy(node_alloc, theNode);
            theNode = nullptr;
            throw;
        }
    }

    // 以 [first, last) 初始化容器
    template<typename Iter>
    void copy_init(Iter first, Iter last )
    {
        theNode = node_alloc.allocate(nodeSize);
        theNode -> unlink();
        size_t n = 0;

        try{
            for( ; first != last; first++, ++n ){
                listNode* node = create_node(*first);
                link_to_back(node, node);
            }
            theSize = n;
        }catch(...){
            clear();
            std::allocator_traits<std::allocator<listNode>>::destroy(node_alloc, theNode);
            theNode = nullptr;
            throw;
        }

    }

    // 在 pos 处连接 [first, last] 的节点
    void insert_nodes(listNode* pos, listNode* first, listNode* last )
    {
        last -> next = pos;
        first -> prev = pos -> prev;
        pos -> prev -> next = first;
        pos -> prev = last;
    }

    // 容器与 [first, last] 节点断开连接
    void uninsert_nodes(listNode* first, listNode* last)
    {
        first -> prev -> next = last -> next;
        last -> next -> prev = first -> prev;
    }

    // 在头部连接 [first, last] 节点
    void link_to_front(listNode* first, listNode* last)
    {
        first -> prev = theNode;
        last -> next = theNode -> next;
        theNode -> next -> prev = last;
        theNode -> next = first;
    }

    // 在尾部连接 [first, last] 节点
    void link_to_back(listNode* first, listNode* last)
    {
        last -> next = theNode;
        first -> prev = theNode -> prev;
        theNode -> prev -> next = first;
        theNode -> prev = last;
    }


    // 复制[first, last)为容器赋值
    template<typename Iter>
    void copy_assign(Iter first, Iter last)
    {
        iterator f1 = theNode -> next;
        iterator l1 = theNode;

        for( ; f1 != l1 && first != last; f1++, first++ )
            *f1 = *first;

        if( f1 == l1 ) {
            insert(f1, first, last);
        }else{
            erase(f1,l1);
        }

    }

    template <class Compare>
    iterator merge_sort(iterator left, iterator right, size_t n, Compare comp)
    {
      if (1 >= n)
        return left;

      if (2 == n) {
        if (comp(*--right, *left)) {
          listNode *ptr = right.current;

          uninsert_nodes(ptr, ptr);
          insert_nodes(left.current, ptr, ptr);

          return right;
        }
        return left;
      }

      size_t average = n / 2;
      size_t data = average;

      iterator center = left;

      while (data) {
        center++;
        data--;
      }

      left = merge_sort(left, center, average, comp);
      center = merge_sort(center, right, n - average, comp);

      size_t count = 0;  // 头次循环判断最左侧节点
      iterator next = nullptr;
      iterator result = left;

      while (left != center && center != right) {
          count++;
          if (comp(*center, *left)) {
              next = center;
              next++;

              for (; next != right && comp(*next, *left); next++) ;

              listNode *center_ptr = center.current;
              listNode *next_ptr = next.current->prev;

              uninsert_nodes(center_ptr, next_ptr);
              insert_nodes(left.current, center_ptr, next_ptr);

              if ( 1 == count) {  // 头次循环判断最左侧节点
                  result = center;
              }
              center = next;

          }
          ++left;
      }
      return result;
    }

  public:
    struct listNode{
        listNode* prev;
        listNode* next;
        Object value;

        listNode() = default;

        listNode(const Object& data )
                :value(data){}

        listNode(const Object&& data)
                :value(std::move(data)) {}

        void unlink()
        {
            prev = next = this;
        }
    };

    struct iterator{
        listNode* current;

        iterator() = default;

        iterator(listNode* curr)
                :current( curr ) {}

        iterator( const iterator& curr)
                :current( curr.current ){}

        iterator( const const_iterator& curr)
                :current( curr.current ) {}

        Object& operator*() const
        {
            return current -> value;
        }

        Object* operator->() const // 编译器行为
        {
            return &(operator*());
        }

        iterator& operator++()
        {
            assert(current != nullptr );

            current = current -> next;

            return *this;
        }

        iterator operator++(int)
        {
            iterator tmp = *this;
            ++*this;
            return tmp;
        }

        iterator& operator--()
        {
            assert(current != nullptr );
            current = current -> prev;

            return *this;
        }

        iterator operator--(int)
        {
            iterator tmp = *this;
            --*this;

            return tmp;
        }

        bool operator==(const iterator& rhs ) const  { return current == rhs.current; }
        bool operator!=(const iterator& rhs ) const  { return current != rhs.current; }

        friend class list<Object>;
    };


    struct const_iterator{
        listNode* current;

        const_iterator() = default;

        const_iterator(listNode* curr)
                :current( curr ) {}

        const_iterator( const const_iterator& curr)
                :current( curr.current ){}
        const_iterator( const iterator& curr )
                : current(curr.current){}

        Object& operator*() const
        {
            return current -> value;
        }

        Object* operator->() const
        {
            return &(operator*());
        }

        const_iterator& operator++()
        {
            assert(current != nullptr );

            current = current -> next;

            return *this;
        }

        const_iterator operator++(int)
        {
            const_iterator tmp = *this;
            ++*this;
            return tmp;
        }

        const_iterator& operator--()
        {
            assert(current != nullptr );
            current = current -> prev;

            return *this;
        }

        const_iterator operator--(int)
        {
            const_iterator tmp = *this;
            --*this;

            return tmp;
        }

        bool operator==(const const_iterator& rhs ) { return current == rhs.current; }
        bool operator!=(const iterator& rhs )
        {
            return !(*this == rhs);
        }

        friend class list<Object>;
    };

};

// 重载比较操作符
template <class Object>
bool operator==(const list<Object>& lhs, const list<Object>& rhs)
{
    auto f1 = lhs.cbegin();
    auto f2 = rhs.cbegin();
    auto l1 = lhs.cend();
    auto l2 = rhs.cend();
    for (; f1 != l1 && f2 != l2 && *f1 == *f2; ++f1, ++f2)
        ;
    return f1 == l1 && f2 == l2;
}

template <class Object>
bool operator<(const list<Object>& lhs, const list<Object>& rhs)
{
    return std::lexicographical_compare(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <class Object>
bool operator!=(const list<Object>& lhs, const list<Object>& rhs)
{
    return !(lhs == rhs);
}

template <class Object>
bool operator>(const list<Object>& lhs, const list<Object>& rhs)
{
    return rhs < lhs;
}

template <class Object>
bool operator<=(const list<Object>& lhs, const list<Object>& rhs)
{
    return !(rhs < lhs);
}

template <class Object>
bool operator>=(const list<Object>& lhs, const list<Object>& rhs)
{
    return !(lhs < rhs);
}

// 重载 mystl 的 swap
template <class Object>
void swap(list<Object>& lhs, list<Object>& rhs) noexcept
{
    lhs.swap(rhs);
}


}

#endif
