#ifndef STACK_HH
#define STACK_HH

#include "deque.hh"
#include <initializer_list>


namespace  mystd {
template <class Object, class Container = mystd::deque<Object>>
class stack{
  private:
    Container con;

  public:

    // 构造函数
    stack(){}

    stack(std::size_t n, Object& value )
            :con(n, value) {}

    stack( std::size_t n )
            :con(n) {}

    stack( std::initializer_list<Object> list )
            :con(list.begin(), list.begin()) {}

    template<typename Iter>
    stack(Iter first, Iter last)
            :con(first, last)  {}

    stack( const Container & con1 )
            :con(con1) {}

    stack( Container&& con1)
            :con(std::move(con1)) {}

    stack( const stack& rhs )
            :con(rhs.con) {}

    stack( stack&& rhs )
            :con(std::move(rhs.con)) {}

    stack& operator=(const stack& rhs)
    {
        if( this != &rhs ) {
            con = rhs.con;
        }

        return *this;
    }

    stack& operator=(stack&& rhs)
    {
        con = std::move(rhs.con);

        return *this;
    }

    stack& operator=(std::initializer_list<Object> list)
    {
        con = list;
        return *this;
    }

    ~stack() = default;

  public:

    // 元素操作
    Object& top()
    {
        return con.back();
    }

    const Object& top() const
    {
        return con.back();
    }

    // 特性操作
    bool empty() const
    {
        return con.empty();
    }

    std::size_t size() const
    {
        return con.size();
    }

    // 插入和删除
    template <typename ... Args>
    void emplace(Args&& ... args )
    {
        con.emplace_back(std::forward<Args>(args)...);
    }

    void push(const Object& value )
    {
        con.push_back(value);
    }

    void push(Object&& value )
    {
        con.push_back(std::move(value));
    }

    void pop()
    {
        con.pop_back();
    }

    void clear()
    {
        while( !empty() )
            pop();
    }

    void swap( stack& rhs )
    {
        std::swap(con, rhs.con);
    }

  public:
    friend bool operator==(const stack& lhs, const stack& rhs )
    {
        return lhs.con == rhs.con;
    }

    friend bool operator<(const stack& lhs, const stack& rhs )
    {
        return lhs.con < rhs.con;
    }
};

// 重载比较操作符
template <class Object, class Container>
bool operator==(const stack<Object, Container>& lhs, const stack<Object, Container>& rhs)
{
    return lhs == rhs;
}

template <class Object, class Container>
bool operator<(const stack<Object, Container>& lhs, const stack<Object, Container>& rhs)
{
    return lhs < rhs;
}

template <class Object, class Container>
bool operator!=(const stack<Object, Container>& lhs, const stack<Object, Container>& rhs)
{
    return !(lhs == rhs);
}

template <class Object, class Container>
bool operator>(const stack<Object, Container>& lhs, const stack<Object, Container>& rhs)
{
    return rhs < lhs;
}

template <class Object, class Container>
bool operator<=(const stack<Object, Container>& lhs, const stack<Object, Container>& rhs)
{
    return lhs < rhs || lhs == rhs;
}

template <class Object, class Container>
bool operator>=(const stack<Object, Container>& lhs, const stack<Object, Container>& rhs)
{
    return !(lhs < rhs);
}

// 重载 mystd 的 swap
template <class Object, class Container>
void swap(stack<Object, Container>& lhs, stack<Object, Container>& rhs) noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}

}
#endif
