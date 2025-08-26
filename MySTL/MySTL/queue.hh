#ifndef QUEUE_HH
#define QUEUE_HH

#include "deque.hh"
#include "vector.hh"
#include "vector.hh"
#include "heap.hh"

namespace mystd{

template<class Obejct, class Container = mystd::deque<Obejct>>
class queue{
  private:
    Container theCon;

  public:

    explicit queue(std::size_t n )
            :theCon(n) {}

    explicit queue( std::size_t n, const Obejct&  value )
            :theCon(n, value) {}

    queue( const queue& rhs )
            :theCon(rhs.theCon) {}

    queue( queue&& rhs )
            :theCon(std::move(rhs.theCon)) {}

    explicit queue( const std::initializer_list<Obejct>& list)
            :theCon(list) {}

    explicit queue( const Container& con = Container() ) // 默认构造函数
            :theCon(con) {}

    queue(Container&& con)
            :theCon(std::move(con)){}

    template<class Iter>
    queue(Iter first, Iter last )
            :theCon(first, last) {}

    queue& operator=(const queue& rhs )
    {
        if( this == &rhs ) {
            theCon = rhs.theCon;
        }
        return *this;
    }

    queue& operator=(queue&& rhs )
    {
        theCon = std::move(rhs.theCon);
        return *this;
    }

    queue& operator=(std::initializer_list<Obejct> rhs)
    {
        theCon = rhs;
        return *this;
    }


    ~queue() = default;

    // 常用操作
    void push(const Obejct& value )
    {
        theCon.push_back(value);
    }

    void push(Obejct&& value)
    {
        theCon.emplace_back(std::move(value));
        // emplace(std::move(value));
    }

    template<typename ... Args>
    void emplace(Args&& ...args)
    {
        theCon.emplace_back(std::forward<Args>(args)...);
    }

    void pop()
    {
        theCon.pop_front();
    }

    Obejct& front()
    {
        return theCon.front();
    }

    Obejct& front() const
    {
        return theCon.front();
    }

    Obejct& back()
    {
        return theCon.back();
    }

    Obejct& back() const noexcept
    {
        return theCon.back();
    }

    size_t size()
    {
        return theCon.size();
    }

    bool empty()
    {
        return theCon.empty();
    }

    void clear()
    {
        if( !empty() )
            theCon.clear();
    }
    void swap(queue& rhs)
    {
        theCon.swap(rhs.theCon);
    }

    public:
    friend bool operator==(const queue& lhs, const queue& rhs )
    {
        return lhs.theCon == rhs.theCon;
    }
    friend bool operator<(const queue& lhs, const queue& rhs )
    {
        return lhs.theCon < rhs.theCon;
    }
};

template<typename Obejct, class Container>
bool operator==(const queue<Obejct,Container>& lhs, const queue<Obejct, Container>& rhs )
{
    return lhs == rhs;
}

template<typename Obejct, class Container>
bool operator!=(const queue<Obejct,Container>& lhs, const queue<Obejct, Container>& rhs )
{
    return !(lhs == rhs);
}

template<typename Obejct, class Container>
bool operator<(const queue<Obejct,Container>& lhs, const queue<Obejct, Container>& rhs )
{
    return lhs < rhs;
}

template<typename Obejct, class Container>
bool operator>(const queue<Obejct,Container>& lhs, const queue<Obejct, Container>& rhs )
{
    return rhs < lhs;
}

template<typename Obejct, class Container>
bool operator<=(const queue<Obejct,Container>& lhs, const queue<Obejct, Container>& rhs )
{
    return !(lhs > rhs);
}
template<typename Obejct, class Container>
bool operator>=(const queue<Obejct,Container>& lhs, const queue<Obejct, Container>& rhs )
{
    return !(lhs < rhs);
}

template<typename Obejct, class Container>
void swap(queue<Obejct, Container>&lhs, queue<Obejct, Container>&rhs)
{
    lhs.swap(rhs);
}

/* ==================================================================================================== */

template<class Obejct, class Container = mystd::vector<Obejct>, class Compare = std::less<Obejct>>
class priority_queue{
  private:
    Container theCon;
    Compare theComp;

  public:
    priority_queue(const Container& theContainer = Container() )
            :theCon(theContainer)
    {
        mystd::make_heap(theCon.begin(), theCon.end(), theComp);
    }

    priority_queue( Container&& theContainer )
            :theCon(std::move(theContainer))
    {
        mystd::make_heap(theCon.begin(), theCon.end(), theComp);
    }

    priority_queue( const priority_queue& rhs ) // 一个priority_queue必然是有序的
            :theCon(rhs.theCon), theComp(rhs.theComp) {}

    priority_queue( priority_queue&& rhs )
            :theCon(std::move(rhs.theCon)), theComp(std::move(rhs.theComp)) {}

    priority_queue& operator=(const priority_queue& rhs)
    {
        if( this != &rhs ){
            theCon = rhs.theCon;
            theComp= rhs.theComp;
        }

        return *this;
    }

    priority_queue& operator=(priority_queue&& rhs)
    {
        theCon = std::move(rhs.theCon);
        theComp= std::move(rhs.theComp);

        return *this;
    }

    explicit priority_queue( size_t n ) // 不需要排序, 应该所有元素必然等值
            :theCon(n) {}

    explicit priority_queue( size_t n, const Obejct& value )
            :theCon(n, value) {}

    priority_queue(const Compare& theCompare)
            :theCon(), theComp(theComp) {}

    explicit priority_queue( std::initializer_list<Obejct> list)
            :theCon(list)
    {
        mystd::make_heap(theCon.begin(), theCon.end(),theComp);
    }

    template<typename Iter>
    priority_queue(Iter first, Iter last )
            :theCon(first, last)
    {
        mystd::make_heap(theCon.begin(), theCon.end(),theComp);
    }

    priority_queue& operator=( std::initializer_list<Obejct> list)
    {
        theCon = list;
        mystd::make_heap(theCon.begin(), theCon.end(),theComp);

        return *this;
    }

    ~priority_queue() = default;


  public:
    const Obejct& top() const
    {
        return theCon.front();
    }

    bool empty() const noexcept
    {
        return theCon.empty();
    }

    size_t size() const noexcept
    {
        return theCon.size();
    }

    void push(const Obejct& value )
    {
        theCon.push_back(value);
        mystd::push_heap(theCon.begin(), theCon.end(), theComp);
    }

    void push(Obejct&& value )
    {
        theCon.push_back(std::move(value));
        mystd::push_heap(theCon.begin(), theCon.end(), theComp);
    }

    template<typename ... Args >
    void emplace( Args ... args )
    {
        theCon.emplace_back(std::forward<Args>(args)...);
        mystd::push_heap(theCon.begin(), theCon.end(), theComp);
    }

    void pop()
    {
        if( !empty() ){
            mystd::pop_heap(theCon.begin(), theCon.end(), theComp);
            theCon.pop_back();
        }
    }

    void clear()
    {
        if( !empty() )
            theCon.clear();
    }

    void swap( priority_queue& rhs )
    {
        theCon.swap(rhs.theCon); //如果Compare不同, 那么就是两个不同的类
    }

  public:
    friend bool operator==(const priority_queue& lhs, const priority_queue& rhs )
    {
        return lhs.theCon == rhs.theCon;
    }

    friend bool operator!=(const priority_queue& lhs, const priority_queue& rhs )
    {
        return lhs.theCon != rhs.theCon;
    }
};

template<class Obejct, class Container, class Compare>
bool operator==( const priority_queue<Obejct, Container, Compare>& lhs,
                 const priority_queue<Obejct, Container, Compare>& rhs )
{
    return lhs == rhs;
}

template <class Obejct, class Container, class Compare>
bool operator!=( const priority_queue<Obejct, Container, Compare>& lhs,
                 const priority_queue<Obejct, Container, Compare>& rhs )
{
    return lhs != rhs;
}

template<class Obejct, class Container, class Compare>
void swap( priority_queue<Obejct, Container, Compare>& lhs,
           priority_queue<Obejct, Container, Compare>& rhs )
{
    lhs.swap(rhs);
}

}

#endif
