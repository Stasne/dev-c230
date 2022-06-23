#include "task3.h"

#include <chrono>
#include <iostream>
#include <semaphore>
#include <string_view>
#include <thread>
#include <vector>

#include "TSSingletone.h"

std::mutex mutex;
void Task3::operator()()
{
    constexpr const size_t    ThreadsCount{ 5 };
    std::vector<std::jthread> threads;
    for (size_t i = 0; i < ThreadsCount; ++i)
    {
        threads.emplace_back([]() {
            auto&                       instance = TSSingletone::instance();
            std::lock_guard<std::mutex> lock(mutex);
            std::cout << "creator: " << instance.creatorId() << std::endl;
            std::cout << "obj: " << &instance << std::endl;
        });
    }
    for (auto& thread : threads)
        thread.join();
}