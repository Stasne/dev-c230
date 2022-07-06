#include "task4.h"

#include <utils.h>

#include <algorithm>
#include <chrono>
#include <future>
#include <iostream>
#include <memory>
#include <string_view>
#include <syncstream>
#include <thread>
#include <vector>
namespace task4
{
    std::osyncstream out(std::cout);
    constexpr size_t COUNT{ 50000 };
    struct Buffer
    {
        int*   data;
        size_t size;
    };
    using Vec            = Buffer;
    using promise_vector = std::promise<Vec>;
    using future_vector  = std::shared_future<Vec>;
    void producer(promise_vector& promise, size_t size, std::future<void>&& finish)
    {
        auto vector = new int[COUNT];
        std::generate(&vector[0], &vector[COUNT - 1], utils::rand<int>);
        promise.set_value({ .data = vector, .size = COUNT });
        finish.wait();
        delete[] vector;
    }

    void Task4::operator()()
    {
        promise_vector     promise;
        std::promise<void> finish;
        auto               fut_finish = finish.get_future();
        future_vector      producerF  = promise.get_future();
        std::thread(producer, std::ref(promise), COUNT, std::move(fut_finish)).detach();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        auto sum = std::async(
            std::launch::async, [](auto ft_vec) {
                auto& vec = ft_vec.get();
                return std::accumulate(&vec.data[0], &vec.data[vec.size - 1], 0);
            },
            producerF);

        auto min = std::async(
            std::launch::async, [](auto ft_vec) {
                auto& vec = ft_vec.get();
                return *std::min_element(&vec.data[0], &vec.data[vec.size - 1]);
            },
            producerF);

        auto max = std::async(
            std::launch::async, [](auto ft_vec) {
                auto& vec = ft_vec.get();
                return *std::max_element(&vec.data[0], &vec.data[vec.size - 1]);
            },
            producerF);
        out << " SUMIS: " << sum.get() << std::endl;
        out << " MIN: " << std::dec << min.get() << std::endl;
        out << " MAX: " << std::dec << max.get() << std::endl;
        finish.set_value();

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}