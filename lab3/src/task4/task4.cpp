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

constexpr size_t COUNT{ 50000 };
using spVec          = std::shared_ptr<std::vector<int>>;
using promise_vector = std::promise<spVec>;
using future_vector  = std::shared_future<spVec>;
void producer(promise_vector& promise, size_t size)
{
    auto vector = std::make_shared<std::vector<int>>(COUNT);
    std::generate(vector->begin(), vector->end(), utils::rand<int>);
    promise.set_value(vector);
}

void Task4::operator()()
{
    promise_vector promise;
    future_vector  producerF = promise.get_future();
    std::thread(producer, std::ref(promise), COUNT).detach();
    auto sum = std::async(
        std::launch::async, [](auto ft_vec) {
            auto& vec = ft_vec.get();
            return std::accumulate(vec->begin(), vec->end(), 0);
        },
        producerF);

    auto min = std::async(
        std::launch::async, [](auto ft_vec) {
            auto& vec = ft_vec.get();
            return *std::min_element(vec->begin(), vec->end());
        },
        producerF);

    auto max = std::async(
        std::launch::async, [](auto ft_vec) {
            auto& vec = ft_vec.get();
            return *std::max_element(vec->begin(), vec->end());
        },
        producerF);
    std::cout << " SUMIS: " << sum.get() << std::endl;
    std::cout << " MIN: " << min.get() << std::endl;
    std::cout << " MAX: " << max.get() << std::endl;
}