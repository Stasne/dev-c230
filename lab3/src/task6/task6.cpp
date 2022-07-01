#include "task6.h"

#include <utils.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <execution>
#include <future>
#include <iostream>
#include <map>
#include <string_view>
#include <syncstream>
#include <thread>
#include <vector>

#include "ThreadPool.h"

using namespace std;

void dummyTask(std::ostream& out)
{
    std::vector<unsigned> vec;
    vec.reserve(500000000);
    std::generate(vec.begin(), vec.end(), utils::rand<unsigned>);
    std::transform(vec.begin(), vec.end(), vec.begin(), [](auto& el) { return (el * 792 + 523); });
}
void Task6::operator()()
{
    cout << "Task 6 " << std::endl;
    constexpr size_t               TaskCount{ 1000 };
    std::vector<std::future<void>> results;
    results.reserve(TaskCount);

    // Простой последовательный запуск
    {
        std::osyncstream out(std::cout);
        auto             sequencedExec = [&] {
            for (size_t i = 0; i < TaskCount; ++i)
                dummyTask(std::ref(out));
        };
        out << "seq:";
        utils::time(sequencedExec, out);
    }
    // Запуск в тредпуле
    {
        std::osyncstream out(std::cout);
        auto             tpoolExec = [&]() {
            ThreadPool pool(std::thread::hardware_concurrency());
            for (size_t i = 0; i < TaskCount; ++i)
            {
                results.emplace_back(pool.push_task(std::packaged_task<void(void)>([&] { dummyTask(std::ref(out)); })));
            }
            for (auto& res : results)
                res.wait();
        };
        out << "pool:";
        utils::time(tpoolExec, out);
    }

    // // Запуск с помощью std::async
    {
        results.clear();
        std::osyncstream out(std::cout);
        auto             asyncExec = [&]() {
            for (size_t i = 0; i < TaskCount; ++i)
            {
                results.emplace_back(std::async(std::launch::async, [&] { dummyTask(std::ref(out)); }));
            }
            for (auto& res : results)
                res.wait();
        };
        out << "async:";
        utils::time(asyncExec, out);
    }
}