#pragma once
#ifndef THREADPOOL_HH
#define THREADPOOL_HH

#include <memory>
#include <queue>
#include <deque>
#include <mutex>
#include <functional>
#include <future>
#include <vector>
#include <thread>
#include <condition_variable>
#include <iostream>
#include <atomic>
#include <type_traits>


#ifndef _WIN32
    #define WINDOWS_MACRO
#else
    #ifdef threadpool_EXPORTS
        #define WINDOWS_MACRO __declspec(dllexport) //库项目调用
    #else
        #define WINDOWS_MACRO __declspec(dllimport) //调用库项目调用
    #endif
#endif

// 线程安全队列
template <class T>
class SafeQueue{
  private:
    std::queue<T, std::deque<T>> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::atomic_bool m_stop;

  public:
    SafeQueue(): m_stop{false}{}
    ~SafeQueue()
    {
        stop();
    }

    bool empty()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    size_t size()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

    // 入队操作, 接受左值和右值
    template<typename U>
    void push( U&& item)
    {
        {
            using U1 = typename std::remove_reference<U>::type;
            static_assert(std::is_same<U1, T>::value == true,
                          "SafeQueue element type is different from SafeQueue");
            std::unique_lock<std::mutex> lock(m_mutex);
            m_queue.push(std::forward<U>(item));
        }
        m_condition.notify_one();
    }

    // 出队操作, 在此实现线程的阻塞和唤醒
    bool pop( T& item )
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait(lock, [ this ](){
            return m_stop || !m_queue.empty();
        });

        if( m_stop && m_queue.empty() )
            return false;

        item = m_queue.front();
        m_queue.pop();
        return true;
    }


    // 唤醒所有线程, 停止入队操作
    void stop()
    {
        if( false == m_stop ){
            m_stop = true;
            m_condition.notify_all();
        }
    }
};

class WINDOWS_MACRO ThreadPool{
  private:
    std::vector<std::thread*>  m_workers;
    // std::vector<std::thread>  m_workers;
    SafeQueue<std::function<void()>> m_tasks;
    std::condition_variable m_condition;
    std::mutex m_mutex;
    std::atomic_bool m_shutdown;

    // 关闭操作
    void shutdown();


  public:

    explicit ThreadPool( const int n  = std::thread::hardware_concurrency());

    ~ThreadPool();

    ThreadPool(const ThreadPool& t ) = delete;
    ThreadPool( ThreadPool&& t ) = delete;
    ThreadPool &operator=(const ThreadPool& t ) = delete;
    ThreadPool &operator=(ThreadPool&& t ) = delete;


    // 提交任务函数,接受类普通成员函数,lambda表达式,普通函数等一切可调用对象
    template<typename Function, typename ... Args>
    auto submit( Function&& f, Args&& ... args) -> std::future<decltype(std::forward<Function>(f)(std::forward<Args>(args)...))>
    {
        using std::forward;
        using std::function;
        using std::bind;
        using std::packaged_task;
        function<decltype(std::forward<Function>(f)(std::forward<Args>(args)...))()> func =
                bind( std::forward<Function>(f), std::forward<Args>(args)...);
        // 连接函数和参数定义，特殊函数类型，避免左右值错误

        auto ptr = std::make_shared
                   <packaged_task<decltype(std::forward<Function>(f)(std::forward<Args>(args)...))()>>(func);

        function<void()> element = [ptr]() {
            (*ptr)();
        };

        m_tasks.push(element);
        m_condition.notify_one();

        return ptr -> get_future();
    }
};

#endif
