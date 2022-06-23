#include "task5.h"

#include <random_generator.h>

#include <algorithm>
#include <chrono>
#include <execution>
#include <iostream>
#include <map>
#include <string_view>
#include <syncstream>
#include <thread>
#include <vector>
void Task5::operator()()
{
    constexpr size_t COUNT{ 500000 };

    using Type = int;
    std::vector<Type> vec(COUNT);
    std::vector<Type> res(COUNT);

    std::generate(vec.begin(), vec.end(), RandomGenerator<Type>::take);
    auto modifier_unary = [](auto& el) { return el % 6782; };
    auto runtime        = [&](std::string label, auto foo) {
        std::cout << label << std::endl;
        auto start = std::chrono::steady_clock::now();
        foo();
        auto end     = std::chrono::steady_clock::now();
        auto elapsed = (end - start);
        std::cout << "\ttime: ";
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count() << "ms" << std::endl;
    };

    runtime("Transform Parallel", [&]() { std::transform(std::execution::par, vec.begin(), vec.end(), res.begin(), modifier_unary); });
    runtime("Transform ParUnseq", [&]() { std::transform(std::execution::par_unseq, vec.begin(), vec.end(), res.begin(), modifier_unary); });
    runtime("Transform Sequentl", [&]() { std::transform(std::execution::seq, vec.begin(), vec.end(), res.begin(), modifier_unary); });
    runtime("Transform Unsequen", [&]() { std::transform(std::execution::unseq, vec.begin(), vec.end(), res.begin(), modifier_unary); });
    std::cout << std::endl;
    runtime("Sort Parallel", [&]() { std::sort(std::execution::par, vec.begin(), vec.end(), [](auto& l, auto& r) { return l < r; }); });
    runtime("Sort ParUnseq", [&]() { std::sort(std::execution::par_unseq, vec.begin(), vec.end(), [](auto& l, auto& r) { return l < r; }); });
    runtime("Sort Sequentl", [&]() { std::sort(std::execution::seq, vec.begin(), vec.end(), [](auto& l, auto& r) { return l < r; }); });
    runtime("Sort Unsequen", [&]() { std::sort(std::execution::unseq, vec.begin(), vec.end(), [](auto& l, auto& r) { return l < r; }); });

    // for (const auto& el : vec)
    //     std::cout << " " << el;
    // std::cout << std::endl;
    // for (const auto& el : res)
    //     std::cout << " " << el;
    // std::cout << std::endl;
}