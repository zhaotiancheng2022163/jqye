#ifndef HASHTABLE_HH
#define HASHTABLE_HH

#include <initializer_list>
#include <memory>
#include <vector>
#include <utility> // 包含 std::pair
#include <cassert>
#include <functional>
#include <string>
#include <cstring>

namespace mystd{

template<class Key>
struct hash{};

inline std::size_t unaligned_load(const char* p, int len)
{
    std::size_t result;
    std::memcpy(&result, p, len ); //为免有脏数据只能使用len长度的.
    return result;                 //保证同样的值哈希值也一致
}

// murmurhash 函数, 用于double, float, string, char*
size_t __hash_bit(const void* ptr, size_t len)
{
    const size_t m = 0x5bd1e995;
    size_t seed = 14695981039346656037ull;
    size_t hash = seed ^ len;
    const char* buf = static_cast<const char*>(ptr);

    // 一次将4个字节混合到散列中
    while(len >= 4)
    {
        size_t k = unaligned_load(buf, len);

        k *= m;
        k ^= k >> 24;
        k *= m;
        hash *= m;
        hash ^= k;
        buf += 4;
        len -= 4;
    }

    size_t k;

    //处理输入数组的最后几个字节
    switch(len)
    {
        case 3:
            k = static_cast<unsigned char>(buf[2]);
            hash ^= k << 16;
        case 2:
            k = static_cast<unsigned char>(buf[1]);
            hash ^= k << 8;
        case 1:
            k = static_cast<unsigned char>(buf[0]);
            hash ^= k;
            hash *= m;
    };

    // 混合散列
    hash ^= hash >> 13;
    hash *= m;
    hash ^= hash >> 15;
    return hash;
}

// 整数, 长整数使用这个哈希函数, 比murmurhash 函数快4到5倍
inline size_t __hash_long( const long long Key )                // ((ax+b)mod p ) mod M 通用散列函数.
{
    if( 0 == Key )
        return 0;

    const unsigned long long DIGS      = 31;
    const unsigned long long mersennep = ( 1 << DIGS ) - 1LL;  // 梅森素数

    unsigned long long hashVal = static_cast<unsigned long long>( 3 ) * Key + 7ULL;

    hashVal = ( ( hashVal >> DIGS ) + ( hashVal & mersennep ) ); // 梅森素数求余技巧, 速度比直接%快4倍以上
                                                                 // [hashVal/(2^DIGS)] + hashVal % (2^DIGS)
    if( hashVal >= mersennep )
        hashVal -= mersennep;

    return static_cast<size_t>( hashVal );
}

template<>
struct hash<long> {
    size_t operator()( const long long Key )
    {
        return __hash_long(Key);
    }
};

template<>
struct hash<unsigned long> {
    size_t operator()( const long long Key )
    {
        return __hash_long(Key);
    }
};

template<>
struct hash<long long> {
    size_t operator()( const long long Key )
    {
        return __hash_long(Key);
    }
};

template<>
struct hash<unsigned long long> {
    size_t operator()( const long long Key )
    {
        return __hash_long(Key);
    }
};

template<class Key>
struct hash<Key*>{
    size_t operator()(Key* p) const noexcept
    {
        return reinterpret_cast<size_t>(p);
    }
};

template<>
struct hash<bool> {
    size_t operator()( const bool Key )
    {
        return static_cast<size_t>(Key);
    }
};


template<>
struct hash<short>{
    size_t operator() ( const long long Key ) const
    {
        return __hash_long(Key);
    }
};

template<>
struct hash<unsigned short>{
    size_t operator() ( const long long Key ) const
    {
        return __hash_long(Key);
    }
};

template<>
struct hash<int>{
    size_t operator() ( const long long Key ) const
    {
        return __hash_long(Key);
    }
};

template<>
struct hash<unsigned int>{
    size_t operator() ( const long long Key ) const
    {
        return __hash_long(Key);
    }
};

template<>
struct hash<float>{
    size_t operator()(const float& val)
    {
        return val == 0.0f ? 0 : __hash_bit((const unsigned char*)&val, sizeof(float));
    }

};

template<>
struct hash<double>{
    size_t operator()(const double& val)
    {
        return val == 0.0f ? 0 : __hash_bit((const unsigned char*)&val, sizeof(double));
    }

};

template<>
struct hash<long double>{
    size_t operator()(const long double& val)
    {
        return val == 0.0f ? 0 : __hash_bit((const unsigned char*)&val, sizeof(long double));
    }
};

template<>
struct hash<std::string> {
    size_t operator()( const std::string & Key )
    {
        unsigned long long hashValue = 0;

        for( char ch : Key )
            hashValue = 37 * hashValue + ch;

        return static_cast<size_t>(hashValue);
    }
};

template<>
struct hash<const char*> {
    size_t operator()( const char* Key )
    {
        return __hash_bit(Key, strlen(Key));
    }
};

template<>
struct hash<char*> {
    size_t operator()( const char* Key )
    {
        return __hash_bit(Key, strlen(Key));
    }

};

}

namespace mystd{

// 使用SFINAE技术来检查传入的类型是否为std::pair
template <class Object, Object v>
struct integral_constant
{
  static constexpr Object value = v;
};

template <bool b>
using bool_constant = integral_constant<bool, b>;

using true_type  = bool_constant<true>;
using false_type = bool_constant<false>;

template<class Object>
struct is_pair final : false_type {};

template<class Object1,class Object2>
struct is_pair<std::pair<Object1, Object2>> final : true_type{} ;

template <class Object>
size_t distance(Object first, Object last)
{
  size_t n = 0;
  while (first != last){
    ++first;
    ++n;
  }
  return n;
}


// 获取pair的first成员
template<class Object, bool>
struct value_traits_imp {
    using key_type    = Object;
    using mapped_type = Object;
    using value_type  = Object;

    template<class Object1>
    static const key_type& get_key( const Object1& value )
    {
        return value;
    }

    template<class Object1>
    static const key_type& get_value( const Object1& value )
    {
        return value;
    }
};

template <class Object>
struct value_traits_imp<Object, true> {
    // 移除 const volatile
    using key_type    = typename std::remove_cv<typename Object::first_type>::type;
    using mapped_type = typename Object::second_type;
    using value_type  = Object;

    template<class Object1>
    static const key_type& get_key( const Object1& value)
    {
        return value.first;
    }

    template<class Object1>
    static const value_type& get_value( const Object1& value )
    {
        return value;
    }
};

template <class Object>
struct value_traits{
    static constexpr bool is_map = mystd::is_pair<Object>::value;

    using value_traits_type = value_traits_imp<Object, is_map>;

    using key_type    = typename value_traits_type::key_type;
    using mapped_type = typename value_traits_type::mapped_type;
    using value_type  = typename value_traits_type::value_type;

    template<class Object1>
    static const key_type& get_key(const Object1& value)
    {
        return value_traits_type::get_key(value);
    }

    template<class Object1>
    static const key_type& get_value(const Object1& value)
    {
        return value_traits_type::get_key(value);
    }
};


//====================================================================================================

static constexpr unsigned long long PRIME_NUM = 99;
// bucket 使用的大小
static constexpr size_t prime_list[] = {
  101ull, 173ull, 263ull, 397ull, 599ull, 907ull, 1361ull, 2053ull, 3083ull,
  4637ull, 6959ull, 10453ull, 15683ull, 23531ull, 35311ull, 52967ull, 79451ull,
  119179ull, 178781ull, 268189ull, 402299ull, 603457ull, 905189ull, 1357787ull,
  2036687ull, 3055043ull, 4582577ull, 6873871ull, 10310819ull, 15466229ull,
  23199347ull, 34799021ull, 52198537ull, 78297827ull, 117446801ull, 176170229ull,
  264255353ull, 396383041ull, 594574583ull, 891861923ull, 1337792887ull,
  2006689337ull, 3010034021ull, 4515051137ull, 6772576709ull, 10158865069ull,
  15238297621ull, 22857446471ull, 34286169707ull, 51429254599ull, 77143881917ull,
  115715822899ull, 173573734363ull, 260360601547ull, 390540902329ull,
  585811353559ull, 878717030339ull, 1318075545511ull, 1977113318311ull,
  2965669977497ull, 4448504966249ull, 6672757449409ull, 10009136174239ull,
};

inline size_t next_prime( size_t n )
{
    const size_t* first = prime_list;
    const size_t* last = prime_list + PRIME_NUM;
    const size_t* pos = std::lower_bound(first, last, n);
    return pos  == last ? *(last-1) : *pos;
}

//====================================================================================================

template<class Object, class hashFun, class keyEqual>
class hashtable{
  public:
    struct iterator;
    struct const_iterator;
    struct const_local_iterator;
    struct local_iterator;
    struct hashtableNode;

  public:
    using value_traits_type = value_traits<Object>;
    using key_type          = typename value_traits_type::key_type;
    using mapped_type       = typename value_traits_type::mapped_type;
    using value_type        = typename value_traits_type::value_type;
    // using key_equal         = keyEqual;

    using node_ptr_type = hashtableNode*;
    using bucket_type   = std::vector<node_ptr_type>;

    std::allocator<Object>        data_alloc;
    std::allocator<hashtableNode> node_alloc;

  private:
    hashtableNode*               node_ptr;
    std::vector<hashtableNode *> theBucket;  // 动态扩充更方便
    size_t                       theSize;
    size_t                       theBucketSize;
    float                        theLoadFactor;
    hashFun                      theHash;
    keyEqual                     equal;

  private:
    bool is_equal( const key_type& key1, const key_type& key2  )
    {
        return equal( key1, key2 );
    }

    bool is_equal( const key_type& key1, const key_type& key2  ) const
    {
        return equal( key1, key2 );
    }

    iterator Begin() noexcept
    {
        for( size_t n = 0; n < theBucketSize; ++n )
            if( theBucket[ n ] )
                return iterator(theBucket[ n ], this );

        return iterator( nullptr, this );
    }


    const_iterator Begin() const noexcept
    {
        for( size_t n = 0; n < theBucketSize; ++n )
            if( theBucket[ n ] )
                return const_iterator( theBucket[ n ], const_cast<hashtable*>(this));

        return const_iterator(nullptr, const_cast<hashtable*>(this));
    }

  public:
    explicit hashtable( size_t bucket_count,
                        const hashFun& hashfun = hashFun(),
                        const keyEqual& equal = keyEqual() )
            :theSize( 0 ), theLoadFactor( 1.0f ), theHash(hashfun), equal(equal)
    {
        init( bucket_count );
    }

    hashtable( const hashtable& rhs )
            :theHash(rhs.theHash), equal( rhs.equal )
    {
        copy_init( rhs );
    }

    hashtable( hashtable&& rhs ) noexcept
            :theSize(rhs.theSize),
             theBucketSize(rhs.theBucketSize),
             theLoadFactor(rhs.theLoadFactor),
             theHash(rhs.theHash),
             equal(rhs.equal)
    {
        theBucket         = std::move( rhs.theBucket );
        rhs.theBucketSize = 0;
        rhs.theSize       = 0;
        rhs.theLoadFactor = 0.0f;
    }

    hashtable& operator=(const hashtable& rhs )
    {
        if( this != &rhs ){
            hashtable tmp( rhs );
            swap( tmp );
        }
        return *this;
    }

    hashtable& operator=(hashtable&& rhs ) noexcept
    {
        hashtable tmp( std::move( rhs ));
        swap( tmp );
        return *this;
    }

    ~hashtable()
    {
        clear();
        theBucketSize = 0;
        theLoadFactor = 0;

    }

  public:
    iterator begin() noexcept
    {
        return Begin();
    }

    const_iterator begin() const noexcept
    {
        return Begin();
    }

    iterator end() noexcept
    {
        return iterator( nullptr, this );
    }

    const_iterator end() const noexcept
    {
        return const_iterator(nullptr, const_cast<hashtable*>(this));
    }

    const_iterator cbegin() const noexcept
    {
        return begin();
    }

    const_iterator cend() const noexcept
    {
        return end();
    }

    // 容器相关的操作
    bool empty() const noexcept
    {
        return theSize == 0;
    }

    size_t size() const noexcept
    {
        return theSize;
    }

    size_t max_size() const noexcept
    {
        return static_cast<size_t>(-1);
    }

    // 修改容器
    template<class ...Args>
    iterator emplace_multi( Args&& ...args )
    {
        auto np = create_node(std::forward<Args>(args)...);
        try
        {
            if ((float)(theSize + 1) > (float)theBucketSize * max_load_factor())
                rehash(theSize + 1);
        }
        catch (...)
        {
            destroy_node(np);
            throw;
        }
        return insert_node_multi(np);
    }


    template <class ... Args>
    std::pair<iterator, bool> emplace_unique( Args&& ... args )
    {
        node_ptr_type np = nullptr;
        try
        {
            np = create_node(std::forward<Args>(args)...);
            if ((float)(theSize + 1) > (float)theBucketSize * max_load_factor())
                rehash(theSize + 1);
        }
        catch (...)
        {
            destroy_node(np);
            throw;
        }
        return insert_node_unique(np);
    }

    template <class... Args>
    iterator emplace_multi_use_hint(const_iterator , Args &&...args)
    {
      return emplace_multi(std::forward<Args>(args)...);
    }

    template <class... Args>
    iterator emplace_unique_use_hint(const_iterator , Args &&...args)
    {
      return emplace_unique(std::forward<Args>(args)...).first;
    }

    iterator insert_multi_noresize( const value_type& value )
    {
        const auto n = hash( value_traits_type::get_key( value ));
        auto first = theBucket[ n ];
        auto tmp = create_node( value );
        for( auto cur = first; cur; cur = cur -> next )
            if( is_equal(value_traits_type::get_key(cur -> value ), value_traits_type::get_key(value))) {
                tmp -> next = cur -> next;
                cur -> next = tmp;
                ++theSize;
                return iterator( tmp , this );
            }

        tmp -> next = first;
        theBucket[ n ] = tmp;
        ++theSize;
        return iterator(tmp, this );
    }
    std::pair<iterator, bool> insert_unique_noresize( const value_type& value )
    {
        const auto n = hash( value_traits_type::get_key( value ));
        auto first = theBucket[ n ];
        for( auto cur = first; cur; cur = cur -> next )
            if( is_equal( value_traits_type::get_key( cur -> value), value_traits_type::get_key(value)))
                return std::make_pair(iterator(cur , this) , false);

        auto tmp = create_node( value );
        tmp -> next = first;
        theBucket[ n ] = tmp;
        ++theSize;
        return std::make_pair(iterator(tmp, this) , true);

    }

    iterator insert_multi( const value_type& value )
    {
        rehash_if_need( 1 );
        return insert_multi_noresize(value);
    }

    iterator insert_multi(value_type&& value)
    {
        return emplace_multi(std::move(value));
    }

    std::pair<iterator, bool> insert_unique( const value_type& value )
    {
        rehash_if_need( 1 );
        return insert_unique_noresize(value);
    }

    std::pair<iterator, bool> insert_unique( value_type&& value )
    {
        return emplate_unique( std::move( value ));
    }

    iterator insert_multi_use_hint(const value_type &value)
    {
      return insert_multi(value);
    }
    iterator insert_multi_use_hint(value_type &&value)
    {
        return emplace_multi(std::move(value));
    }

    iterator insert_unique_use_hint(const value_type &value)
    {
      return insert_unique(value).first;
    }
    iterator insert_unique_use_hint(value_type &&value)
    {
      return emplace_unique(std::move(value));
    }

    template<class Object1>
    void insert_multi( Object1 first, Object1 last )
    {
        copy_insert_multi( first, last );
    }

    template<class Object1>
    void insert_unique(Object1 first, Object1 last)
    {
        copy_insert_unique(first, last);
    }

    //erase /clear
    void erase( const_iterator position )
    {
        auto p = position.node_ptr;
        if( p ){
            const auto n = hash( value_traits_type::get_key( p -> value));
            auto cur = theBucket[ n ];
            if( cur == p ) {
                theBucket[ n ] = cur -> next;
                destroy_node(cur);
                --theSize;
            }else{
                auto next = cur -> next;
                while( next ){
                    if( next == p ) {
                        cur -> next = next -> next;
                        destroy_node( next );
                        --theSize;
                        break;
                    }
                    else {
                        cur = next;
                        next = cur -> next;
                    }
                }
            }
        }
    }

    void erase( const_iterator first, const_iterator last )
    {
        if( first.node_ptr == last.node_ptr )
            return;
        auto first_bucket = first.node_ptr
                            ? hash(value_traits_type::get_key(first.node_ptr ->value))
                            : theBucketSize;
        auto last_bucket = last.node_ptr
                           ? hash( value_traits_type::get_key(last.node_ptr -> value ))
                           : theBucketSize;

        if( first_bucket == last_bucket ){
            erase_bucket( first_bucket, first.node_ptr, last.node_ptr );
        }
        else {
            erase_bucket(first_bucket, first.node_ptr, nullptr );
            for( auto n = first_bucket + 1; n < last_bucket; ++n ) {
                if( theBucket[ n ] != nullptr )
                    erase_bucket(n, nullptr );
            }
            if( last_bucket != theBucketSize )
                erase_bucket(last_bucket, last.node_ptr);
        }
    }

    size_t erase_multi( const key_type& key )
    {
        auto p = equal_range_multi(key);
        if( p.first.node_ptr != nullptr ){
            erase(p.first, p.second);
            return mystd::distance(p.first, p.second);
        }
        return 0;
    }

    size_t erase_unique( const key_type& key )
    {
        const auto n = hash( key );
        auto first = theBucket[ n ];
        if( first ) {
            if( is_equal(value_traits_type::get_key(first -> value), key)) {
                    theBucket[ n ] = first -> next;
                    destroy_node(first);
                    --theSize;
                    return -1;
            } else {
                auto next = first -> next;
                while( next ) {
                    if( is_equal(value_traits_type::get_key(next->value), key)){
                        first -> next = next -> next;
                        destroy_node( next );
                        --theSize;
                        return 1;
                    }
                    first = next;
                    next = first->next;
                }
            }
        }
        return 0;
    }

    void clear()
    {
        if( theSize != 0 ) {
            for( size_t i = 0; i < theBucketSize; ++i ) {
                node_ptr_type cur = theBucket[ i ];
                while( cur != nullptr ) {
                    node_ptr_type next = cur -> next;
                    destroy_node( cur );
                    cur = next;
                }
                theBucket[ i ] = nullptr;
            }
            theSize = 0;
        }
    }

    void swap( hashtable& rhs ) noexcept
    {
        if( this != &rhs ) {
            theBucket.swap(rhs.theBucket);
            std::swap(theBucketSize, rhs.theBucketSize);
            std::swap(theSize, rhs.theSize);
            std::swap(theLoadFactor, rhs.theLoadFactor);
            std::swap(theHash, rhs.theHash);
            std::swap(equal, rhs.equal);
        }
    }

    // 查找
    size_t count( const key_type& key ) const
    {
        const auto n = hash( key );
        size_t result = 0;
        for( node_ptr_type cur = theBucket[ n ]; cur; cur = cur -> next ) {
            if( is_equal(value_traits_type::get_key(cur -> value ), key )) {
                ++result;
            }
        }

        return result;
    }

    iterator find( const key_type& key )
    {
        const auto n = hash( key );
        node_ptr_type first = theBucket[ n ];
        for(; first && !is_equal(value_traits_type::get_key(first->value), key); first = first -> next ) {}
        return iterator( first, this );
    }

    const_iterator find( const key_type& key ) const
    {
        const auto n = hash( key );
        node_ptr_type first = theBucket[ n ];
        for(; first && !is_equal(value_traits_type::get_key(first->value), key); first = first->next ) {}
        return const_iterator(first, const_cast<hashtable*>(this));
    }

    std::pair<iterator, iterator> equal_range_multi( const key_type& key )
    {
        const auto n = hash( key );

        for( node_ptr_type first = theBucket[ n ]; first; first = first -> next ) {
            if( is_equal( value_traits_type::get_key(first -> value), key)) {
                for( node_ptr_type second = first -> next; second ; second = second -> next ) {
                    if( !is_equal(value_traits_type::get_key(second -> value), key))
                        return std::make_pair(iterator(first, this), iterator(second, this));
                }
                for( auto m = n + 1; m < theBucketSize; ++m ) {
                    if( theBucket[ m ] )
                        return std::make_pair(iterator(first, this), iterator(theBucket[m], this));
                }
                                return std::make_pair(iterator(first, this) , end());
            }
        }

        return std::make_pair(end(), end());
    }

    std::pair<const_iterator, const_iterator> equal_range_multi(const key_type& key) const
    {
        const auto n = hash( key );
        for( node_ptr_type first = theBucket[n]; first; first = first -> next ) {
            if( is_equal(value_traits_type::get_key(first -> valuek), key)) {
                for( node_ptr_type second = first -> next; second; second = second -> next ) {
                    if( !is_equal(value_traits_type::get_key(second -> value), key))
                        return std::make_pair(const_iterator(first, const_cast<hashtable*>(this)),
                                              const_iterator(second, const_cast<hashtable*>(this)));
                }

                for( auto m = n+ 1; m < theBucketSize; ++m ) {
                    if( theBucket[ m ] )
                        return std::make_pair(const_iterator(first, const_cast<hashtable*>(this)),
                                              const_iterator(theBucket[m], const_cast<hashtable*>(this)));
                }

                return std::make_pair(const_iterator(first, const_cast<hashtable*>(this)), cend());

            }
        }
        return std::make_pair(cend(),cend());
    }

    std::pair<iterator, iterator> equal_range_unique( const key_type& key )
    {
        const auto n = hash( key );
        for( node_ptr_type first = theBucket[ n ]; first; first = first -> next ) {
            if( is_equal(value_traits_type::get_key(first->value), key)) {
                if( first -> next )
                    return std::make_pair(iterator(first, this), iterator(first -> next, this) );

                for( auto m = n + 1; m < theBucketSize; ++m ) {
                    if( theBucket[m])
                        return std::make_pair(iterator(first,this) , iterator(theBucket[m], this));
                }
                return std::make_pair(iterator(first,this),  end());
            }
        }

        return std::make_pair(end(), end());
    }

    std::pair<const_iterator, const_iterator> equal_range_unique(const key_type& key ) const
    {
        const auto n = hash( key );
        for( node_ptr_type first = theBucket[n]; first; first = first -> next ) {
            if( is_equal(value_traits_type::get_key(first->value), key)) {
                if( first -> next )
                    return std::make_pair(const_iterator(first, const_cast<hashtable*>(this)),
                                          const_iterator(theBucket[first -> next], const_cast<hashtable*>(this)));

                for( auto m = n +1; m < theBucketSize; ++m ) {
                    if( theBucket[m])
                        return std::make_pair(const_iterator(first, const_cast<hashtable*>(this)),
                                              const_iterator(theBucket[m], const_cast<hashtable*>(this)));
                }

                return std::make_pair(const_iterator(first, const_cast<hashtable*>(this)),
                                      const_iterator(cend(), const_cast<hashtable*>(this)));

            }
        }
        return std::make_pair(cend(), cend());
    }

    // bucket interface
    local_iterator begin( size_t n ) noexcept
    {
        assert(n < theSize );
        return theBucket[n];
    }

    local_iterator end( size_t n ) noexcept
    {
        assert( n < theSize );
        return nullptr;
    }

    const_local_iterator begin( size_t n ) const noexcept
    {
        assert( n < theSize );
        return theBucket[n];
    }
    const_local_iterator cbegin( size_t n ) const noexcept
    {
        assert( n < theSize );
        return theBucket[ n ];
    }

    const_local_iterator end( size_t n ) const noexcept
    {
        assert( n < theSize );
        return nullptr;
    }

    const_local_iterator cend( size_t n ) const noexcept
    {
        assert( n < theSize );
        return nullptr;
    }

    size_t bucket_count() const noexcept
    {
        return theBucketSize;
    }

    size_t bucket_size( size_t n ) const noexcept
    {
        size_t result = 0;
        for( auto cur = theBucket[n]; cur; cur = cur -> next )
            ++result;

        return result;
    }

    size_t bucket( const key_type& key ) const
    {
        return hash(key);
    }

    //hash policy
    float load_factor() const noexcept
    {
        return theBucketSize != 0 ? static_cast<float>(theSize) / theBucketSize : 0.0f;
    }

    float max_load_factor() const noexcept
    {
        return theLoadFactor;
    }

    void max_load_factor( float n )
    {
        assert( n < 0.0 || n != theLoadFactor );
        theLoadFactor = n;
    }

    void rehash( size_t count )
    {
        auto n = next_prime( count );
        if( n > theBucketSize ){
            replace_bucket( n );
        }
        else {
            if( static_cast<float>(theSize)/static_cast<float>(n) < max_load_factor() - 0.25f &&
                static_cast<float>(n) < static_cast<float>(theBucketSize)*0.75)
                replace_bucket(n) ;
        }
    }

    void reserve(size_t count )
    {
        rehash(static_cast<size_t>((float)count / max_load_factor() + 0.5f));
    }

  private:
    void init( size_t n )
    {
        const auto bucket_nums = next_prime(n);
        try{
          theBucket.reserve(bucket_nums);
          theBucket.assign(bucket_nums, nullptr);
        }
        catch(...)
        {
            theBucketSize = 0;
            theSize = 0;
            throw;
        }
        theBucketSize = theBucket.size();
    }

    void copy_init( const hashtable& ht )
    {
        theBucketSize = 0;
        theBucket.reserve(ht.theBucketSize);
        theBucket.assign(ht.theBucketSize, nullptr);
        try{
            for( size_t i = 0; i < ht.theBucketSize; ++i ){
                node_ptr_type cur = ht.theBucket[i];
                if( cur ) {
                    auto copy = create_node( cur -> value );
                    theBucket[ i ] = copy;
                    for( auto next = cur -> next; next; cur = next, next = cur -> next ) {
                        copy -> next = create_node( next -> value );
                        copy = copy -> next;
                    }
                    copy -> next = nullptr;
                }
            }
            theBucketSize = ht.theBucketSize;
            theLoadFactor = ht.theLoadFactor;
            theSize = ht.theSize;
        }
        catch( ... )
        {
            clear();
        }
    }

    template<class ... Args >
    node_ptr_type create_node(Args&& ... args )
    {
        node_ptr_type tmp = nullptr;
        try{
            tmp = node_alloc.allocate(1);
            // std::allocator_traits<std::allocator<hashtableNode>>::construct(node_alloc,&(tmp -> value),std::forward<Args>(args)... );
            std::allocator_traits<std::allocator<hashtableNode>>::construct(node_alloc,std::addressof(tmp -> value),std::forward<Args>(args)... );
            tmp -> next = nullptr;
            return tmp;
        }
        catch(...){
            node_alloc.deallocate(tmp, 1);
            throw;
        }

    }

    void destroy_node(node_ptr_type node )
    {
        if( node != nullptr ) {
            std::allocator_traits<std::allocator<hashtableNode>>::destroy(node_alloc, std::addressof(node->value));
            // node_alloc.deallocate(node, sizeof(hashtableNode));
            node_alloc.deallocate(node, 1);
            // node -> next = nullptr;
            // node = nullptr;
        }
    }

    size_t hash( const key_type& key, size_t n ) const
    {
        return theHash(key) % n;
    }

    size_t hash( const key_type& key ) const
    {
        return theHash(key) % theBucketSize;
    }

    void rehash_if_need(size_t n )
    {
        if( static_cast<float>(theSize + n ) > static_cast<float>(theBucketSize) * max_load_factor() )
            rehash( theSize + n );
    }


    //insert
    template<class Object1>
    void copy_insert_multi( Object1 first, Object1 last )
    {
        rehash_if_need(mystd::distance( first, last));
        for(;first != last; ++first)
            insert_multi_noresize(*first);
    }

    template <class Object1>
    void copy_insert_unique(Object1 first, Object1 last )
    {
        rehash_if_need(mystd::distance(first,  last));
        for(; first != last; ++first )
            insert_unique_noresize(*first);
    }

    std::pair<iterator, bool> insert_node_unique(node_ptr_type np )
    {
        const auto n = hash(value_traits_type::get_key( np -> value));
        auto cur = theBucket[ n ];
        if( cur == nullptr ) {
            theBucket[ n ] = np;
            ++theSize;
            return std::make_pair(iterator(np, this), true);
        }

        for(; cur; cur = cur -> next ) {
            if( is_equal( value_traits_type::get_key(cur -> value), value_traits_type::get_key(np -> value))){
                destroy_node(np);
                return std::make_pair( iterator(cur, this), false);
            }
        }

        np -> next  = theBucket[ n ];
        theBucket[ n ] = np;
        ++theSize;
        return std::make_pair(iterator(np, this), true);
    }

    iterator insert_node_multi(node_ptr_type np)
    {
        const auto n = hash(value_traits_type::get_key(np->value));
        auto cur = theBucket[n];
        if (cur == nullptr)
        {
            theBucket[n] = np;
            ++theSize;
            return iterator(np, this);
        }
        for (; cur; cur = cur->next)
        {
            if (is_equal(value_traits_type::get_key(cur->value), value_traits_type::get_key(np->value))) {
                np->next = cur->next;
                cur->next = np;
                ++theSize;
                return iterator(np, this);
            }
        }
        np->next = theBucket[n];
        theBucket[n] = np;
        ++theSize;
        return iterator(np, this);
    }

    void replace_bucket(size_t bucket_count )
    {
        bucket_type bucket( bucket_count );

        if( theSize != 0 ) {
            for( size_t i = 0; i < theBucketSize; ++i ) {
                for( auto first = theBucket[ i ]; first; first = first -> next ) {
                    auto tmp = create_node( first -> value );
                    const auto n = hash( value_traits_type::get_key(first->value), bucket_count );
                    auto f = bucket[ n ];
                    bool is_inserted = false;
                    for( auto cur = f; cur; cur = cur -> next ) {
                        if( is_equal( value_traits_type::get_key(cur ->value), value_traits_type::get_key(first -> value))) {
                            tmp -> next = cur -> next;
                            cur -> next  = tmp;
                            is_inserted = true;
                            break;

                        }
                    }

                    if( !is_inserted ){
                        tmp -> next = f;
                        bucket[ n ] = tmp;
                    }
                }
            }
            theBucket.swap( bucket );
            theBucketSize = bucket_count;

            for( auto i : bucket ) {
                for( auto j = i; j ;  ) {
                    auto tmp = j -> next;
                    destroy_node(j);
                    j = tmp;
                }
            }
        }
    }

    void erase_bucket(size_t n, node_ptr_type first, node_ptr_type last )
    {
        auto cur = theBucket[ n ];
        if( cur == first ){
            erase_bucket(n , last);
        }
        else {
            node_ptr_type next = cur -> next;
            for(; next != first; cur = next, next = cur -> next ) {}
            while( next != last ) {
                cur -> next = next -> next;
                destroy_node(next );
                next = cur -> next;
                --theSize;
            }
        }
    }

    void erase_bucket(size_t n, node_ptr_type last)
    {
        auto cur = theBucket[ n ];
        while( cur != last ) {
            auto next = cur -> next;
            destroy_node( cur );
            cur = next;
            --theSize;
        }
        theBucket[n] = last;
    }


  public:

    // 分离链接法的节点
    struct hashtableNode{
        Object         value;
        hashtableNode* next;

        hashtableNode() = default;
        hashtableNode( const Object& n ) : value(n), next(nullptr){}

        hashtableNode( const hashtableNode& node ) : value( node.value), next(node.next){}
        hashtableNode( hashtableNode&& node ) : value( std::move(node.value)), next(node.next)
        {
            node.next = nullptr;
        }
        friend class hashtable<Object, hashFun, keyEqual>;

    };

    struct iterator {
        using contain_ptr_type       = hashtable<Object, hashFun, keyEqual>*;

        hashtableNode*   node_ptr;      //迭代器指向的节点
        contain_ptr_type hashtable_ptr; //保持与容器的连接

        iterator() = default;

        iterator( hashtableNode* n , contain_ptr_type t )
        {
            node_ptr = n;
            hashtable_ptr = t;
        }

        iterator( const iterator& rhs )
        {
            node_ptr = rhs.node_ptr;
            hashtable_ptr = rhs.hashtable_ptr;
        }

        iterator( const const_iterator& rhs )
        {
            node_ptr = rhs.node_ptr;
            hashtable_ptr = rhs.hashtable_ptr;
        }

        iterator& operator=( const iterator& rhs )
        {
            if( this != &rhs ) {
                node_ptr = rhs.node_ptr;
                hashtable_ptr = rhs.hashtable_ptr;
            }
            return *this;
        }

        iterator& operator=( const const_iterator& rhs )
        {
            if( this != &rhs ) {
                node_ptr = rhs.node_ptr;
                hashtable_ptr = rhs.hashtable_ptr;
            }
            return *this;
        }

        Object& operator*() const { return node_ptr -> value; }

        Object* operator->() const  { return &(operator*());}

        // 找到下一个非空的数据对
        iterator& operator++()
        {
            // 不能使用static_assert, static_assert的条件必须在编译时确定其结果, 指针的值是在运行时确定的
            assert( node_ptr != nullptr);
            const hashtableNode* old_ptr = node_ptr;
            node_ptr = node_ptr -> next;
            // 如果为空, 那么指向下一个非空bucket的起始处
            if( node_ptr == nullptr ) {
                auto index = hashtable_ptr -> hash(value_traits<Object>::get_key( old_ptr -> value ));

                while( !node_ptr && ++index < hashtable_ptr -> theBucketSize)
                    node_ptr = hashtable_ptr -> theBucket[ index ];
            }
            return *this;
        }

        iterator operator++(int)
        {
            iterator tmp = *this;
            ++*this;
            return tmp;
        }

        bool operator==(const iterator& rhs)
        { return node_ptr == rhs.node_ptr; }

        bool operator!=(const iterator& rhs)
        { return node_ptr != rhs.node_ptr; }
        friend class hashtable<Object, hashFun, keyEqual>;
    };

    struct const_iterator{
        using node_ptr_type          = hashtableNode*;
        using contain_ptr_type       = hashtable<Object, hashFun, keyEqual>*;

        node_ptr_type    node_ptr;  //迭代器指向的节点
        contain_ptr_type hashtable_ptr; //保持与容器的连接

        const_iterator() = default;
        const_iterator( node_ptr_type n , contain_ptr_type t )
        {
            node_ptr      = n;
            hashtable_ptr = t;
        }

        const_iterator( const iterator& rhs )
        {
            node_ptr      = rhs.node_ptr;
            hashtable_ptr = rhs.hashtable_ptr;
        }

        const_iterator( const const_iterator & rhs )
        {
            node_ptr      = rhs.node_ptr;
            hashtable_ptr = rhs.hashtable_ptr;
        }

        const_iterator& operator=( const iterator& rhs )
        {
            if( this != &rhs ) {
                node_ptr      = rhs.node_ptr;
                hashtable_ptr = rhs.hashtable_ptr;
            }
            return *this;
        }

        const_iterator operator=( const const_iterator & rhs )
        {
            if( this != &rhs ) {
                node_ptr      = rhs.node_ptr;
                hashtable_ptr = rhs.hashtable_ptr;
            }
            return *this;
        }

        const Object& operator*() const { return node_ptr -> value;}
        const Object* operator->() const { return &(operator*()); }

        const_iterator& operator++()
        {
            assert( node_ptr != nullptr );
            const node_ptr_type old = node_ptr;

            node_ptr = node_ptr -> next;

            if( node_ptr == nullptr ) {
                auto index = hashtable_ptr ->  hash(value_traits<Object>::get_key( old -> value ));

                while( !node_ptr && ++index < hashtable_ptr ->  theBucketSize  )
                    node_ptr = hashtable_ptr -> theBucket[ index ];
            }
            return *this;
        }

        const_iterator operator++( int )
        {
            const_iterator tmp = *this;
            ++*this;
            return tmp;
        }

        bool operator==(const const_iterator& rhs)
        { return node_ptr == rhs.node_ptr; }

        bool operator!=(const const_iterator& rhs)
        { return node_ptr != rhs.node_ptr; }
    };


    struct local_iterator {

        hashtableNode* node_ptr;

        local_iterator( hashtableNode* n ):node_ptr(n) {}

        local_iterator(const local_iterator& rhs ):node_ptr(rhs.node_ptr) {}

        local_iterator(const const_local_iterator& rhs ): node_ptr(rhs.node_ptr) {}

        Object& operator*() const { return node_ptr -> value; };
        Object* operator->() const { return &(operator*());}

        local_iterator& operator++()
        {
            assert( node_ptr != nullptr );
            node_ptr = node_ptr -> next;
            return *this;
        }

        local_iterator operator++( int )
        {
            local_iterator tmp{*this};
            ++*this;
            return tmp;
        }

        bool operator==( const local_iterator& rhs ) const { return node_ptr == rhs.node_ptr; }
        bool operator!=( const local_iterator& rhs ) const { return node_ptr != rhs.node_ptr; }
    };

    struct const_local_iterator {

        hashtableNode* node_ptr;

        const_local_iterator( hashtableNode* n ):node_ptr(n) {}

        const_local_iterator(const local_iterator& rhs ):node_ptr(rhs.node_ptr) {}

        const_local_iterator(const const_local_iterator& rhs ): node_ptr(rhs.node_ptr) {}

        const Object& operator*() const { return node_ptr -> value; };
        const Object* operator->() const { return &(operator*());}

        const_local_iterator& operator++()
        {
            assert( node_ptr != nullptr );
            node_ptr = node_ptr -> next;
            return *this;
        }

        const_local_iterator operator++( int )
        {
            const_local_iterator tmp{*this};
            ++*this;
            return tmp;
        }

        bool operator==( const const_local_iterator& rhs ) const { return node_ptr == rhs.node_ptr; }
        bool operator!=( const const_local_iterator& rhs ) const { return node_ptr != rhs.node_ptr; }
    };
};

template <class Object, class hashFun, class keyEqual>
void swap( hashtable<Object, hashFun, keyEqual> & lhs,
           hashtable<Object, hashFun, keyEqual>& rhs ) noexcept
{
    lhs.swap(rhs);
}

}

#endif
