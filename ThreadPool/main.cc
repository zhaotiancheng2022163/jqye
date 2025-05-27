#include "ThreadPool.hh"
#include <iostream>
#include <memory>
#include "test.hh"
#include <chrono>


int main()
{
    std::mutex mtx;
    ThreadPool pool(12);

#ifdef FUNCTION_TEST
    function_test( pool );
#endif
#ifdef CLASS_TEST
	class_test( pool );
#endif
#ifdef LAMBDA_TEST
    lambda_test( pool );
#endif

    mtx.lock();
    std::cout << "\n\n ===the test in main=== \n\n";
    mtx.unlock();

    int result;
    for (int i = 1; i <= 3; ++i)
        for (int j = 1; j <= 10; ++j)
        {
            pool.submit(multiply, std::ref(result), i, j);
        }

    return 0;
}
