#include <random>
#include <mutex>
#include <random>
#include <mutex>
#include <functional>
#include <iostream>
#include "test.hh"


std::mutex mtx;

std::random_device rd; // 真实随机数产生器

std::mt19937 mt(rd()); //生成计算随机数mt

std::uniform_int_distribution<int> dist(1, 5); //生成1到5之间的离散均匀分布数

auto rnd = std::bind(dist, mt);

// 设置线程睡眠时间, 模拟数据处理过程
void simulate_hard_computation()
{
    std::this_thread::sleep_for(std::chrono::seconds( rnd()));
}

// 结果返回
int multiply( int &res, const int a, const int b)
{
    mtx.lock();
    std::cout << "the thread id is " << std::this_thread::get_id() << '\n';

    mtx.unlock();
    simulate_hard_computation();
    res = a * b;
    mtx.lock();
    std::cout << a << " * " << b << " = " << res << std::endl;
    mtx.unlock();
    return res;
}

int Test::multiply( int &res, const int a, const int b)
{
    mtx.lock();
    std::cout << "the thread id is " << std::this_thread::get_id() << '\n';
    mtx.unlock();
    simulate_hard_computation();
    mtx.lock();
    res = a * b;
    std::cout << a << " * " << b << " = " << res << std::endl;
    mtx.unlock();
    return res;
}

void function_test( ThreadPool& pool )
{
    mtx.lock();
	std::cout << "\n\n === The test for function === \n\n";
    mtx.unlock();
    int result = 0;

    // 提交乘法操作，总共30个
    for (int i = 1; i <= 3; ++i)
        for (int j = 1; j <= 10; ++j)
        {
            pool.submit(multiply, std::ref(result), i, j);
        }

    int multiply_res = 0;

    // 使用return参数提交函数
    auto futures = pool.submit(multiply, std::ref(multiply_res), 5, 3);

    // 等待乘法输出完成
    int future_result = futures.get();
    std::cout << "Last future_result = " << future_result << std::endl;
    std::cout << "Last multiply_result = " << multiply_res << std::endl;

}

void lambda_test( ThreadPool& pool )
{
    mtx.lock();
	std::cout << "\n\n === The test for lambda === \n\n";
    mtx.unlock();
    int result = 0;

    // 提交乘法操作，总共30个
    for (int i = 1; i <= 3; ++i)
        for (int j = 1; j <= 10; ++j)
        {
            pool.submit( [&result](const int a,const int b) -> int {
                mtx.lock();
                std::cout << "the thread id is " << std::this_thread::get_id() << '\n';
                mtx.unlock();
                simulate_hard_computation();
                mtx.lock();
                result = a * b;
                std::cout << a << " * " << b << " = " << result << std::endl;
                mtx.unlock();
                return result;
            }, i, j);
        }

    int multiply_res = 0;

    // 使用return参数提交函数
    auto futures = pool.submit([&multiply_res](const int a,const int b) -> int {
        std::lock_guard<std::mutex> lock(mtx);
                std::cout << "the thread id is " << std::this_thread::get_id() << '\n';
                simulate_hard_computation();
                multiply_res = a * b;
                std::cout << a << " * " << b << " = " << multiply_res << std::endl;
                return multiply_res;
            }, 5, 3);


    // 等待乘法输出完成
    int future_result = futures.get();
    std::cout << "Last future_result = " << future_result << std::endl;
    std::cout << "Last multiply_result = " << multiply_res << std::endl;

}

void class_test(ThreadPool& pool )
{
    mtx.lock();
	std::cout << "\n\n=== The test for class === \n\n";
    mtx.unlock();
    Test test;

    int result = 0;

    for( int i = 0; i < 3; i++ )
        for( int j = 10; j > 0 ; j-- )
            pool.submit( std::bind(&Test::multiply, &test, std::ref(result) , i, j) );    // 注意, 这里必须传递一个绑定器, 不然和submit的定义不同, 编译器报错
    int multiply_res = 0;

    auto future = pool.submit( std::bind(&Test::multiply, &test, std::ref(multiply_res) , 3, 5) );

    int future_result = future.get();

    std::cout << "Last future_result = " << future_result << std::endl;
    std::cout << "Last multiply_result = " << multiply_res << std::endl;

}
