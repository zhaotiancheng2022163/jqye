#ifndef UNORDERED_MAP_TEST_HH
#define UNORDERED_MAP_TEST_HH

// unordered_map test : 测试 unordered_map, unordered_multimap 的接口与它们 insert 的性能

#include <unordered_map>
#include "../MySTL/unordered_map.hh"
#include "test.hh"


namespace mystd
{
namespace test
{
namespace unordered_map_test
{

// pair 的宏定义
#define PAIR_U    std::pair<int, int>

// map 的遍历输出
#define MAP_COUT(m) do { \
    std::string m_name = #m; \
    std::cout << " " << m_name << " :"; \
    for (auto it : m)    std::cout << " <" << it.first << "," << it.second << ">"; \
    std::cout << std::endl; \
} while(0)

// map 的函数操作
#define MAP_FUN_AFTER(con, fun) do { \
    std::string str = #fun; \
    std::cout << " After " << str << " :" << std::endl; \
    fun; \
    MAP_COUT(con); \
} while(0)

// map 的函数值
#define MAP_VALUE(fun) do { \
    std::string str = #fun; \
    auto it = fun; \
    std::cout << " " << str << " : <" << it.first << "," << it.second << ">\n"; \
} while(0)

void unordered_map_test()
{
  std::cout << "[===============================================================]" << std::endl;
  std::cout << "[-------------- Run container test : unordered_map -------------]" << std::endl;
  std::cout << "[-------------------------- API test ---------------------------]" << std::endl;
  std::vector<PAIR_U> v;
  for (int i = 0; i < 5; ++i)
    v.push_back(PAIR_U(5 - i, 5 - i));
  mystd::unordered_map<int, int> um1;
  mystd::unordered_map<int, int> um2(520);
  mystd::unordered_map<int, int> um3(520, mystd::hash<int>());
  mystd::unordered_map<int, int> um4(520, mystd::hash<int>(), std::equal_to<int>());
  mystd::unordered_map<int, int> um5(v.begin(), v.end());
  mystd::unordered_map<int, int> um6(v.begin(), v.end(), 100);
  mystd::unordered_map<int, int> um7(v.begin(), v.end(), 100, mystd::hash<int>());
  mystd::unordered_map<int, int> um8(v.begin(), v.end(), 100, mystd::hash<int>(), std::equal_to<int>());
  mystd::unordered_map<int, int> um9(um5);
  mystd::unordered_map<int, int> um10(std::move(um5));
  mystd::unordered_map<int, int> um11;
  um11 = um6;
  mystd::unordered_map<int, int> um12;
  um12 = std::move(um6);
  mystd::unordered_map<int, int> um13{ PAIR_U(1,1),PAIR_U(2,3),PAIR_U(3,3) };
  mystd::unordered_map<int, int> um14;
  um14 = { PAIR_U(1,1),PAIR_U(2,3),PAIR_U(3,3) };

  MAP_FUN_AFTER(um1, um1.emplace(1, 1));
  MAP_FUN_AFTER(um1, um1.insert(PAIR_U(2, 2)));
  MAP_FUN_AFTER(um1, um1.insert(um1.end(), PAIR_U(3, 3)));
  MAP_FUN_AFTER(um1, um1.insert(v.begin(), v.end()));
  MAP_FUN_AFTER(um1, um1.erase(um1.begin()));
  MAP_FUN_AFTER(um1, um1.erase(um1.begin(), um1.find(3)));
  MAP_FUN_AFTER(um1, um1.erase(1));
  std::cout << std::boolalpha;
  FUN_VALUE(um1.empty());
  std::cout << std::noboolalpha;
  FUN_VALUE(um1.size());
  FUN_VALUE(um1.bucket_count());
  FUN_VALUE(um1.bucket(1));
  FUN_VALUE(um1.bucket_size(um1.bucket(5)));
  MAP_FUN_AFTER(um1, um1.clear());
  MAP_FUN_AFTER(um1, um1.swap(um7));
  MAP_VALUE(*um1.begin());
  FUN_VALUE(um1.at(1));
  FUN_VALUE(um1[1]);
  std::cout << std::boolalpha;
  FUN_VALUE(um1.empty());
  std::cout << std::noboolalpha;
  FUN_VALUE(um1.size());
  FUN_VALUE(um1.bucket_count());
  FUN_VALUE(um1.bucket(1));
  FUN_VALUE(um1.bucket_size(um1.bucket(1)));
  MAP_FUN_AFTER(um1, um1.reserve(1000));
  FUN_VALUE(um1.size());
  FUN_VALUE(um1.bucket_count());
  for( int i = 0; i < um1.bucket_count(); i++ ) {
      if( 0 !=  um1.bucket_size(i) )
          std::cout << " um1.bucket_size(" << i << ")" << " : " << um1.bucket_size(i) << "\n";
  }

  MAP_FUN_AFTER(um1, um1.rehash(150));
  FUN_VALUE(um1.bucket_count());
  FUN_VALUE(um1.count(1));
  MAP_VALUE(*um1.find(3));
  FUN_VALUE(um1.load_factor());
  FUN_VALUE(um1.max_load_factor());
  MAP_FUN_AFTER(um1, um1.max_load_factor(1.5f));
  FUN_VALUE(um1.max_load_factor());
  PASSED;
#if PERFORMANCE_TEST_ON
  std::cout << "[--------------------- Performance Testing ---------------------]" << std::endl;
  std::cout << "|---------------------|-------------|-------------|-------------|" << std::endl;
  std::cout << "|       emplace       |";
#if LARGER_TEST_DATA_ON
  MAP_EMPLACE_TEST(unordered_map, SCALE_M(LEN1), SCALE_M(LEN2), SCALE_M(LEN3));
#else
  MAP_EMPLACE_TEST(unordered_map, SCALE_S(LEN1), SCALE_S(LEN2), SCALE_S(LEN3));
#endif
  std::cout << std::endl;
  std::cout << "|---------------------|-------------|-------------|-------------|" << std::endl;
  PASSED;
#endif
  std::cout << "[-------------- End container test : unordered_map -------------]" << std::endl;
}

void unordered_multimap_test()
{
  std::cout << "[===============================================================]" << std::endl;
  std::cout << "[----------- Run container test : unordered_multimap -----------]" << std::endl;
  std::cout << "[-------------------------- API test ---------------------------]" << std::endl;
  std::vector<PAIR_U> v;
  for (int i = 0; i < 5; ++i)
    v.push_back(PAIR_U(5 - i, 5 - i));
  mystd::unordered_multimap<int, int> um1;
  mystd::unordered_multimap<int, int> um2(520);
  mystd::unordered_multimap<int, int> um3(520, mystd::hash<int>());
  mystd::unordered_multimap<int, int> um4(520, mystd::hash<int>(), std::equal_to<int>());
  mystd::unordered_multimap<int, int> um5(v.begin(), v.end());
  mystd::unordered_multimap<int, int> um6(v.begin(), v.end(), 100);
  mystd::unordered_multimap<int, int> um7(v.begin(), v.end(), 100, mystd::hash<int>());
  mystd::unordered_multimap<int, int> um8(v.begin(), v.end(), 100, mystd::hash<int>(), std::equal_to<int>());
  mystd::unordered_multimap<int, int> um9(um5);
  mystd::unordered_multimap<int, int> um10(std::move(um5));
  mystd::unordered_multimap<int, int> um11;
  um11 = um6;
  mystd::unordered_multimap<int, int> um12;
  um12 = std::move(um6);
  mystd::unordered_multimap<int, int> um13{ PAIR_U(1,1),PAIR_U(2,3),PAIR_U(3,3) };
  mystd::unordered_multimap<int, int> um14;
  um14 = { PAIR_U(1,1),PAIR_U(2,3),PAIR_U(3,3) };

  MAP_FUN_AFTER(um1, um1.emplace(1, 1));
  MAP_FUN_AFTER(um1, um1.insert(PAIR_U(2, 2)));
  MAP_FUN_AFTER(um1, um1.insert(um1.end(), PAIR_U(3, 3)));
  MAP_FUN_AFTER(um1, um1.insert(v.begin(), v.end()));
  MAP_FUN_AFTER(um1, um1.erase(um1.begin()));
  MAP_FUN_AFTER(um1, um1.erase(um1.begin(), um1.find(3)));
  MAP_FUN_AFTER(um1, um1.erase(1));
  std::cout << std::boolalpha;
  FUN_VALUE(um1.empty());
  std::cout << std::noboolalpha;
  FUN_VALUE(um1.size());
  FUN_VALUE(um1.bucket_count());
  FUN_VALUE(um1.bucket(1));
  FUN_VALUE(um1.bucket_size(um1.bucket(5)));
  MAP_FUN_AFTER(um1, um1.clear());
  MAP_FUN_AFTER(um1, um1.swap(um7));
  MAP_VALUE(*um1.begin());
  std::cout << std::boolalpha;
  FUN_VALUE(um1.empty());
  std::cout << std::noboolalpha;
  FUN_VALUE(um1.size());
  FUN_VALUE(um1.bucket_count());
  FUN_VALUE(um1.bucket(1));
  FUN_VALUE(um1.bucket_size(um1.bucket(1)));
  MAP_FUN_AFTER(um1, um1.reserve(1000));
  FUN_VALUE(um1.size());
  FUN_VALUE(um1.bucket_count());
  for( int i = 0; i < um1.bucket_count(); i++ ) {
      if( 0 !=  um1.bucket_size(i) )
          std::cout << " um1.bucket_size(" << i << ")" << " : " << um1.bucket_size(i) << "\n";
  }
  MAP_FUN_AFTER(um1, um1.rehash(150));
  FUN_VALUE(um1.bucket_count());
  FUN_VALUE(um1.count(1));
  MAP_VALUE(*um1.find(3));
  FUN_VALUE(um1.load_factor());
  FUN_VALUE(um1.max_load_factor());
  MAP_FUN_AFTER(um1, um1.max_load_factor(1.5f));
  FUN_VALUE(um1.max_load_factor());
  PASSED;
#if PERFORMANCE_TEST_ON
  std::cout << "[--------------------- Performance Testing ---------------------]" << std::endl;
  std::cout << "|---------------------|-------------|-------------|-------------|" << std::endl;
  std::cout << "|       emplace       |";
#if LARGER_TEST_DATA_ON
  MAP_EMPLACE_TEST(unordered_multimap, SCALE_M(LEN1), SCALE_M(LEN2), SCALE_M(LEN3));
#else
  MAP_EMPLACE_TEST(unordered_multimap, SCALE_S(LEN1), SCALE_S(LEN2), SCALE_S(LEN3));
#endif
  std::cout << std::endl;
  std::cout << "|---------------------|-------------|-------------|-------------|" << std::endl;
  PASSED;
#endif
  std::cout << "[----------- End container test : unordered_multimap -----------]" << std::endl;
}

}
}
}
#endif // !MYTINYSTL_UNORDERED_MAP_TEST_H_

