#include "task6.h"

#include <algorithm>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

#include "random_generator.h"

namespace multithread
{
    int64_t find(uint32_t val, const std::vector<uint32_t>& data, size_t thread_count = std::thread::hardware_concurrency())
    {
        // TODO: std::ranges - can we use it to pass vector's intervals to threads?
        std::vector<std::jthread> threads;
        int64_t                   res{ -1 };
        std::mutex                mutex;
        std::stop_source          ss;
        size_t                    groupSize = data.size() / thread_count;

        auto intervalFind = [&](size_t start, size_t end) {
            if (end > data.size())
                end = data.size();

            for (size_t i = start; i < end; ++i)
            {
                if (ss.stop_requested())
                {
                    std::cout << std::this_thread::get_id() << ": stop requested" << std::endl;
                    break;
                }
                if (data[i] == val)
                {
                    std::lock_guard<std::mutex> lock(mutex);
                    if (ss.stop_requested()) // Надо ли оно? Прост не нравится, что несклько присваиваний происходит
                        break;

                    ss.request_stop();
                    std::cout << std::this_thread::get_id() << ": Found value " << val << " at index " << i << "\n\tset request_stop()" << std::endl;
                    res = i;
                }
            }
        };

        for (size_t i = 0; i < thread_count; ++i)
        {
            size_t lastIndex = i == thread_count - 1 ? data.size() : groupSize * (i + 1);
            threads.emplace_back(intervalFind, i * groupSize, (lastIndex));
        }
        for (auto& th : threads)
            th.join();

        return res;
    }
}

void Task6::operator()()
{
    std::cout << "Task 6" << std::endl;

    const size_t NUM_COUNT = 200000000; // 0000;

    std::vector<uint32_t> data(NUM_COUNT);

    // fill with randoms
    std::generate(data.begin(), data.end(), [&]() { return RandomGenerator<uint32_t>::take(); });
    const uint32_t VAL2FIND = RandomGenerator<uint32_t>::take(); // data[43];

    std::cout << std::dec << "FINDING: " << VAL2FIND << std::endl;

    auto start = std::chrono::steady_clock::now(); // timer start

    auto found = multithread::find(VAL2FIND, data);

    auto end = std::chrono::steady_clock::now(); // timer stop

    if (found >= 0)
        std::cout << std::dec << "Found[" << found << "] = " << data[found] << std::endl;
    else
        std::cout << "Not found" << std::endl;

    auto s  = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "(elapsed: " << s << "s " << ms << "ms " << us % 1000 << "us)" << std::endl;
}