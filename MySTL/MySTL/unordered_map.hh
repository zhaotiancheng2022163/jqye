// 使用 hashtable 作为底层实现机制，容器内的元素不会自动排序
#ifndef UNORDERED_MAP_HH
#define UNORDERED_MAP_HH


#include "hashtable.hh"
#include <utility>

namespace mystd {

// 模板类 unordered_map，键值不允许重复

template<class Key, class Object, class Hash= mystd::hash<Key>,  class keyEqual = std::equal_to<Key>>
class unordered_map{
  private:
    using base_type = mystd::hashtable<std::pair<const Key, Object>, Hash, keyEqual>;
    base_type Hashtable;

  public:
    using key_type   = typename base_type::key_type;
    using value_type = typename base_type::value_type;

    using iterator       = typename base_type::iterator;
    using const_iterator = typename base_type::const_iterator;

  public:
    unordered_map()
            :Hashtable(101, Hash(), keyEqual()) {}

    explicit unordered_map(size_t bucket_count,
                           const Hash& hash = Hash(),
                           const keyEqual& equal = keyEqual())
            :Hashtable(bucket_count, hash, equal){}


    template<class inputIterator>
    unordered_map(inputIterator first, inputIterator last,
                  const size_t bucket_count = 101,
                  const Hash& hash = Hash(),
                  const keyEqual& equal = keyEqual())
            :Hashtable(std::max(bucket_count, static_cast<size_t>(std::distance(first, last ))), hash, equal)
    {
        for(; first != last; ++first )
            Hashtable.insert_unique_noresize(*first);
    }

    unordered_map( std::initializer_list<value_type> list,
                   const size_t bucket_count = 101,
                   const Hash& hash = Hash(),
                   const keyEqual& equal = keyEqual())
            :Hashtable(std::max(bucket_count, static_cast<size_t>(list.size())), hash, equal)
    {
        for( auto first = list.begin(), last = list.end(); first != last; ++first)
            Hashtable.insert_unique_noresize(*first);
    }

    unordered_map( const unordered_map& rhs)
            :Hashtable(rhs.Hashtable){}

    unordered_map(unordered_map&& rhs) noexcept
            :Hashtable(std::move(rhs.Hashtable)) {}

    ~unordered_map() = default;

    // 赋值操作
    unordered_map& operator=(const unordered_map& rhs)
    {
        if(&rhs == this )
            return *this;
        Hashtable = rhs.Hashtable;
        return *this;
    }

    unordered_map& operator=(unordered_map&& rhs)
    {
        Hashtable = std::move(rhs.Hashtable);
        return *this;
    }


    unordered_map& operator=(std::initializer_list<value_type> list)
    {
        Hashtable.clear();
        Hashtable.reserve(list.size());
        for( auto first = list.begin(), last = list.end(); first != last; ++first )
            Hashtable.insert_unique_noresize(*first);
        return *this;
    }

  public:
    // 特性操作
    size_t size() const noexcept
    {
        return Hashtable.size();
    }

    bool empty() const noexcept
    {
        return Hashtable.empty();
    }

    void clear()
    {
        Hashtable.clear();
    }

    size_t bucket_count() const noexcept
    {
        return Hashtable.bucket_count();
    }

    float load_factor() const noexcept
    {
        return Hashtable.load_factor();
    }

    float max_load_factor() const noexcept
    {
        return Hashtable.max_load_factor();
    }
    void max_load_factor( float mlf )
    {
        Hashtable.max_load_factor(mlf);
    }


    void reserve( size_t count )
    {
        Hashtable.reserve(count);
    }

    void rehash( size_t count )
    {
        Hashtable.rehash(count);
    }

    size_t bucket_size(size_t n ) const noexcept
    {
        return Hashtable.bucket_size(n);
    }

    size_t bucket( const key_type& key) const
    {
        return Hashtable.bucket(key);
    }

    // 迭代器
    iterator begin() noexcept
    {
        return Hashtable.begin();
    }
    iterator end() noexcept
    {
        return Hashtable.end();
    }

    const_iterator begin() const noexcept
    {
        return Hashtable.begin();
    }
    const_iterator end() const noexcept
    {
        return Hashtable.end();
    }

    const_iterator cbegin() const noexcept
    {
        return Hashtable.cbegin();
    }
    const_iterator cend() const noexcept
    {
        return Hashtable.cend();
    }


    // 元素操作
    Object& operator[](const key_type& key)
    {
        iterator it = Hashtable.find( key );
        if( it.node == nullptr )
            it = Hashtable.emplace_unique(key, Object{}).first;
        return it -> second;
    }

    Object& operator[](key_type&& key)
    {
        iterator it = Hashtable.find(key);
        if( it.node_ptr==nullptr)
            it = Hashtable.emplace_unique(std::move(key), Object{}).first;
        return it->second;
    }

    Object& at( const key_type& key)
    {
        iterator it = Hashtable.find(key);
        // assert(it.node_ptr == nullptr );
        return it -> second;
    }

    const Object& at( const key_type& key ) const
    {
        iterator it = Hashtable.find(key);
        // assert(it.node_ptr == nullptr );
        return it->second;
    }


    // 交换操作
    void swap(unordered_map& other) noexcept
    {
        Hashtable.swap(other.Hashtable);
    }

    // 比较操作
    bool operator==( const unordered_map& rhs ) const
    {
        return Hashtable.equal_range_unique(rhs.Hashtable);
    }

    bool operator!=( const unordered_map& rhs ) const
    {
        return !Hashtable.equal_range_unique(rhs.Hashtable);
    }

    // 查找操作
    iterator find( const key_type& key )
    {
        return Hashtable.find(key);
    }

    const_iterator find( const key_type& key ) const
    {
        return Hashtable.find(key);
    }

    size_t count( const key_type& key ) const
    {
        return Hashtable.count(key);
    }

    // 插入和删除
    template<class inputIterator>
    void insert(inputIterator first, inputIterator last)
    {
        Hashtable.insert_unique(first, last);
    }

    std::pair<iterator, bool> insert(const value_type& value)
    {
        return Hashtable.insert_unique(value);
    }

    std::pair<iterator, bool> insert(value_type&& value)
    {
        return Hashtable.emplace_unique(std::move(value));
    }

    iterator insert( const_iterator hint, const value_type& value)
    {
        return Hashtable.insert_unique_use_hint(hint, value);
    }
    iterator insert( const_iterator hint, value_type&& value)
    {
        return Hashtable.emplace_unique_use_hint(hint, std::move(value));
    }

    template<class ...Args>
    std::pair<iterator, bool> emplace(Args&& ...args)
    {
        return Hashtable.emplace_unique(std::forward<Args>(args)...);
    }

    // 删除
    size_t erase( const key_type& key)
    {
        return Hashtable.erase_unique(key);
    }

    void erase(iterator pos)
    {
        Hashtable.erase(pos);
    }

    void erase(iterator first, iterator last)
    {
        Hashtable.erase(first, last);
    }

  public:
    friend bool operator==(const unordered_map& lhs, const unordered_map& rhs )
    {
        return lhs.Hashtable.equal_range_unique(rhs.Hashtable);
    }

    friend bool operator!=( const unordered_map& lhs, const unordered_map& rhs )
    {
        return !lhs.Hashtable.equal_range_unique(rhs.Hashtable);
    }
};

template<class Key, class Object, class Hash, class keyEqual>
bool operator==(const unordered_map<Key, Object, Hash, keyEqual>& lhs,
                const unordered_map<Key, Object, Hash, keyEqual>& rhs)
{
    return lhs == rhs;
}
template<class Key, class Object, class Hash, class keyEqual>
bool operator!=(const unordered_map<Key, Object, Hash, keyEqual>& lhs,
                const unordered_map<Key, Object, Hash, keyEqual>& rhs)
{
    return lhs != rhs;
}

template<class Key, class Object, class Hash, class keyEqual>
void swap(unordered_map<Key, Object, Hash, keyEqual>& lhs,
          unordered_map<Key, Object, Hash, keyEqual>& rhs)
{
    lhs.swap(rhs);
}

/* ==================================================================================================== */

// 模板类 unordered_multimap，键值允许重复

template <class Key, class Object, class Hash = mystd::hash<Key>, class keyEqual = std::equal_to<Key>>
class unordered_multimap{
  private:
    using base_type = mystd::hashtable<std::pair<const Key, Object>, Hash, keyEqual>;
    base_type Hashtable;

  public:
    using key_type   = typename base_type::key_type;
    using value_type = typename base_type::value_type;

    using iterator       = typename base_type::iterator;
    using const_iterator = typename base_type::const_iterator;

  public:
    unordered_multimap()
            :Hashtable(101, Hash(), keyEqual()) {}

    explicit unordered_multimap(size_t bucket_count,
                                const Hash& hash = Hash(),
                                const keyEqual& equal = keyEqual())
            :Hashtable(bucket_count, hash, equal){}

    template<class inputIterator>
    unordered_multimap(inputIterator first, inputIterator last,
                       const size_t bucket_count = 101,
                       const Hash& hash = Hash(),
                       const keyEqual& equal = keyEqual())
            :Hashtable(std::max(bucket_count, static_cast<size_t>(std::distance(first, last ))), hash, equal)
    {
        for(; first != last; ++first )
            Hashtable.insert_multi_noresize(*first);
    }

    unordered_multimap( std::initializer_list<value_type> list,
                        const size_t bucket_count = 101,
                        const Hash& hash = Hash(),
                        const keyEqual& equal = keyEqual())
            :Hashtable(std::max(bucket_count, static_cast<size_t>(list.size())), hash, equal)
    {
        for( auto first = list.begin(), last = list.end(); first != last; ++first)
            Hashtable.insert_multi_noresize(*first);
    }

    unordered_multimap( const unordered_multimap& rhs)
            :Hashtable(rhs.Hashtable){}

    unordered_multimap(unordered_multimap&& rhs) noexcept
            :Hashtable(std::move(rhs.Hashtable)) {}

    ~unordered_multimap() = default;

  public:
    // 赋值操作
    unordered_multimap& operator=(const unordered_multimap& rhs)
    {
        if( &rhs == this )
            return *this;
        Hashtable = rhs.Hashtable;
        return *this;
    }

    unordered_multimap& operator=(unordered_multimap&& rhs)
    {
        Hashtable = std::move(rhs.Hashtable);
        return *this;
    }

    unordered_multimap& operator=(std::initializer_list<value_type> list)
    {
        Hashtable.clear();
        Hashtable.reserve(list.size());
        for( auto first = list.begin(), last = list.end(); first != last; ++first )
            Hashtable.insert_multi_noresize(*first);
        return *this;
    }

    // 特性操作
    size_t size() const noexcept
    {
        return Hashtable.size();
    }

    bool empty() const noexcept
    {
        return Hashtable.empty();
    }

    void clear()
    {
        Hashtable.clear();
    }

    size_t bucket_count() const noexcept
    {
        return Hashtable.bucket_count();
    }

    float load_factor() const noexcept
    {
        return Hashtable.load_factor();
    }

    float max_load_factor() const noexcept
    {
        return Hashtable.max_load_factor();
    }

    void max_load_factor( float mlf )
    {
        Hashtable.max_load_factor(mlf);
    }

    void reserve( size_t count )
    {
        Hashtable.reserve(count);
    }

    void rehash( size_t count )
    {
        Hashtable.rehash(count);
    }

    size_t bucket_size(size_t n ) const noexcept
    {
        return Hashtable.bucket_size(n);
    }

    size_t bucket( const key_type& key) const
    {
        return Hashtable.bucket(key);
    }

    // 迭代器
    iterator begin() noexcept
    {
        return Hashtable.begin();
    }

    iterator end() noexcept
    {
        return Hashtable.end();
    }

    const_iterator begin() const noexcept
    {
        return Hashtable.begin();
    }
    const_iterator end() const noexcept
    {
        return Hashtable.end();
    }

    const_iterator cbegin() const noexcept
    {
        return Hashtable.cbegin();
    }
    const_iterator cend() const noexcept
    {
        return Hashtable.cend();
    }


    // 元素操作
    Object& operator[](const key_type& key)
    {
        iterator it = Hashtable.find( key );
        if( it.node == nullptr )
            it = Hashtable.emplace_multi(key, Object{}).first;
        return it -> second;
    }

    Object& operator[](key_type&& key)
    {
        iterator it = Hashtable.find(key);
        if( it.node_ptr==nullptr)
            it = Hashtable.emplace_multi(std::move(key), Object{}).first;
        return it->second;
    }

    Object& at( const key_type& key)
    {
        iterator it = Hashtable.find(key);
        assert(it.node_ptr == nullptr );
        return it -> second;
    }

    const Object& at( const key_type& key ) const
    {
        iterator it = Hashtable.find(key);
        assert(it.node_ptr == nullptr );
        return it->second;
    }


    // 交换操作
    void swap(unordered_multimap& other) noexcept
    {
        Hashtable.swap(other.Hashtable);
    }

    // 比较操作
    bool operator==( const unordered_multimap& rhs ) const
    {
        return Hashtable.equal_range_multi(rhs.Hashtable);
    }

    bool operator!=( const unordered_multimap& rhs ) const
    {
        return !Hashtable.equal_range_multi(rhs.Hashtable);
    }

    // 查找操作
    iterator find(const key_type& key)
    {
        return Hashtable.find(key);
    }

    const_iterator find( const key_type& key ) const
    {
        return Hashtable.find(key);
    }

    size_t count( const key_type& key ) const
    {
        return Hashtable.count(key);
    }

    // 插入和删除
    template<class inputIterator>
    void insert(inputIterator first, inputIterator last)
    {
        Hashtable.insert_multi(first, last);
    }

    iterator insert(const value_type& value)
    {
        return Hashtable.insert_multi(value);
    }

    iterator insert(value_type&& value)
    {
        return Hashtable.emplace_multi(std::move(value));
    }

    iterator insert( const_iterator hint, const value_type& value)
    {
        return Hashtable.insert_multi_use_hint(hint, value);
    }
    iterator insert( const_iterator hint, value_type&& value)
    {
        return Hashtable.emplace_multi_use_hint(hint, std::move(value));
    }

    template<class ...Args>
    iterator emplace(Args&& ...args)
    {
        return Hashtable.emplace_multi(std::forward<Args>(args)...);
    }

    // 删除
    void erase(iterator pos)
    {
        Hashtable.erase(pos);
    }

    void erase(iterator first, iterator last)
    {
        Hashtable.erase(first, last);
    }

    size_t erase( const key_type& key)
    {
        return Hashtable.erase_multi(key);
    }


  public:
    friend bool operator==(const unordered_multimap& lhs, const unordered_multimap& rhs )
    {
        return lhs.Hashtable.equal_range_multi(rhs.Hashtable);
    }

    friend bool operator!=( const unordered_multimap& lhs, const unordered_multimap& rhs )
    {
        return !lhs.Hashtable.equal_range_multi(rhs.Hashtable);
    }
};

template <class Key, class Object, class Hash, class keyEqual>
bool operator==(const unordered_multimap<Key, Object, Hash, keyEqual>& lhs,
                const unordered_multimap<Key, Object, Hash, keyEqual>& rhs )
{
    return lhs == rhs;
}

template <class Key, class Object, class Hash, class keyEqual>
bool operator!=(const unordered_multimap<Key, Object, Hash, keyEqual>& lhs,
                const unordered_multimap<Key, Object, Hash, keyEqual>& rhs )
{
    return lhs != rhs;
}

template <class Key, class Object, class Hash, class keyEqual>
void swap(unordered_multimap<Key, Object, Hash, keyEqual>& lhs,
          unordered_multimap<Key, Object, Hash, keyEqual>& rhs)
{
    lhs.swap(rhs);
};

}

#endif
