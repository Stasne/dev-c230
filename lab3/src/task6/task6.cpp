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

static atomic<int> cnt;
void               dummyTask()
{
    std::vector<int> vec;
    vec.reserve(50000);
    std::generate(vec.begin(), vec.end(), utils::rand<int>);
    std::transform(vec.begin(), vec.end(), vec.begin(), [](auto& el) { return el % 3789; });
    ++cnt;
}
void Task6::operator()()
{
    cout << "Task 6 " << std::endl;

    constexpr size_t               TaskCount{ 10000 };
    std::vector<std::future<void>> results;
    results.reserve(TaskCount);

    // WHY I CANT ??
    /*
    std::packaged_task<void(void)> task(dummyTask);
    auto                           fut = task.get_future();
    pool.push_task(std::ref(task));
    task.reset();
    */

    //Простой последовательный запуск
    auto sequencedExec = [&] {
        for (size_t i = 0; i < TaskCount; ++i)
            dummyTask();
    };
    utils::time(sequencedExec);
    // Запуск в тредпуле
    auto tpoolExec = [&]() {
        ThreadPool pool(std::thread::hardware_concurrency());
        for (size_t i = 0; i < TaskCount; ++i)
        {
            results.emplace_back(pool.push_task(std::packaged_task<void(void)>(dummyTask)));
            std::cout << ".";
        }
        for (auto& res : results)
            res.wait();
    };
    utils::time(tpoolExec);

    // Запуск с помощью std::async
    results.clear();
    auto asyncExec = [&]() {
        for (size_t i = 0; i < TaskCount; ++i)
        {
            results.emplace_back(std::async(std::launch::async, dummyTask));
        }
        for (auto& res : results)
            res.wait();
    };
    utils::time(asyncExec);
}