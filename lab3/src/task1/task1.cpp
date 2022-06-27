#include "task1.h"

#include <stdio.h>
#include <utils.h>

#include <algorithm>
#include <chrono>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>

void Task1::operator()()
{
    std::cout << "Task 1 " << std::endl;
    const size_t COUNT{ 50000 };

    const size_t                  ThreadsCount = std::thread::hardware_concurrency();
    size_t                        groupSize    = COUNT / ThreadsCount;
    std::vector<int>              vec(COUNT);
    std::vector<std::future<int>> results(ThreadsCount);
    std::generate(vec.begin(), vec.end(), utils::rand<int>);
    uint64_t summ{ 0 };
    std::cout << "simple run " << std::endl;
    utils::time([&] { summ = std::accumulate(vec.begin(), vec.end(), summ); });
    std::cout << summ << std::endl;
    auto accumulate_parallel = [&]() {
        for (size_t i = 0; i < ThreadsCount - 1; ++i)
            results[i] = std::async(std::launch::async, [&] { return std::accumulate(vec.begin() + groupSize * i, vec.begin() + groupSize * (i + 1), 0); });

        results[ThreadsCount - 1] = std::async(std::launch::async, [&] { return std::accumulate(vec.begin() + groupSize * (ThreadsCount - 2), vec.end(), 0); });

        for (auto& res : results)
            summ += res.get();
    };

    std::cout << "Parallel " << std::endl;
    utils::time([&] { accumulate_parallel(); });
    std::cout << summ << std::endl;
}
