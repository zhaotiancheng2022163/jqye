#pragma once
#ifndef TEST_HH
#define TEST_HH

#include "ThreadPool.hh"

#ifndef _WIN32
    #define WINDOWS_MACRO
#else
    #ifdef test_EXPORTS
        #define WINDOWS_MACRO __declspec(dllexport) //库项目调用
    #else
        #define WINDOWS_MACRO __declspec(dllimport) //调用库项目调用
    #endif
#endif


// 设置线程睡眠时间, 模拟数据处理过程
void simulate_hard_computation();

// 结果返回
int multiply( int &res, const int a, const int b);

// 为在Windows编译可通过, 使用WINDOWS_MACRO宏
struct WINDOWS_MACRO Test{
    int multiply( int &res, const int a, const int b);
};

// 测试普通函数
void function_test( ThreadPool& pool );

// 测试lambda表达式
void lambda_test( ThreadPool& pool );

// 测试类的普通成员函数
void class_test(ThreadPool& pool );


#endif
