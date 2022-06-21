#include "task4.h"

#include <random_generator.h>

#include <cmath>
#include <iostream>
#include <thread>
#include <vector>

static size_t ThreadsCount{ 1 };

size_t threadCountialog()
{
    const size_t HW_COUNT = std::thread::hardware_concurrency();
    size_t       threads{ HW_COUNT };
    std::cout << "Hardware concurrency: " << HW_COUNT << std::endl;
    std::cout << "Enter desired threads count ..." << std::endl;
    std::cin >> threads;
    return threads;
}
template <typename Source, typename Dest, typename Pred>
void abs_fix(const Source& src, Dest& dst, Pred pred)
{
    size_t n = std::size(src) < std::size(dst) ? std::size(src) : std::size(dst);

    auto srcBegin  = std::begin(src);
    auto destBegin = std::begin(dst);
    auto groupSize = n > ThreadsCount ? n / ThreadsCount : n;
    // auto applyPredToInterval = [&](std::iterator_traits<srcBegin>::value_type src, std::iterator_traits<destBegin>::value_type dst, size_t count) {
    auto applyPredToInterval = [&](auto src, auto dst, size_t count) {
        for (size_t i = 0; i < count; ++i)
        {
            *dst = pred(*src);
            ++dst;
            ++src;
        }
    };

    std::vector<std::jthread> threads;
    threads.reserve(ThreadsCount - 1);
    for (size_t i = 0; i < ThreadsCount - 1; ++i)
    {
        threads.emplace_back(applyPredToInterval, srcBegin, destBegin, groupSize);
        std::advance(srcBegin, groupSize);
        std::advance(destBegin, groupSize);
    }
    threads.emplace_back(applyPredToInterval, srcBegin, destBegin, groupSize + n % groupSize);
}
void Task4::operator()()
{
    std::cout << "Task 4" << std::endl;

    ThreadsCount = threadCountialog();

    const size_t     NUM_COUNT = 5;
    std::vector<int> data(NUM_COUNT + 4);
    std::generate(data.begin(), data.end(), [&]() { return RandomGenerator<int>::take(); });
    // std::generate(data.begin(), data.end(), RandomGenerator<int>::take());
    std::vector<int> target(NUM_COUNT);

    // how to pass threads count like this?
    //????????????????????????????????????
    // abs<thread_count>(src, dest, pred);

    abs_fix(data, target, [](decltype(target)::value_type val) { return std::abs(val); });
    // VS ????????????????????????????????????
    // abs_fix(data, target, [](auto val) { return std::abs(val); });
}