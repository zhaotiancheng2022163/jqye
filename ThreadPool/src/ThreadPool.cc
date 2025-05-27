#include "ThreadPool.hh"
#include <atomic>

ThreadPool::ThreadPool( int num ) : m_workers(num), m_shutdown(false)
{
    for( int i = 0; i < num; i++ )
        m_workers[ i ] = new std::thread([this](){
            while(1){
                {
                    std::function<void()> func;
                    if(!m_tasks.pop(func))
                        break;
                    // return;  // 这里也可以用return

                    if(func)
                        func(); // ps: 为了使func()有意义,需要有这样一个作用域,每次运行后销毁
                }
            }
        });
}


ThreadPool::~ThreadPool()
{
    if( false == m_shutdown )
        shutdown();
}

void ThreadPool::shutdown()
{
    m_shutdown = true;

    m_tasks.stop();

    for( std::thread* threads : m_workers){
        if( threads != nullptr && threads -> joinable()){
            threads -> join();
        }
    }

    for( std::thread* threads : m_workers){
            delete threads;
            threads = nullptr;
    }

    std::cout << "Shutdown the ThreadPool ...\n";
}
