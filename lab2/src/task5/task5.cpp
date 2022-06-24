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
    constexpr size_t COUNT{ 5000000 };

    using Type = uint64_t;
    std::vector<Type> vec(COUNT);
    std::vector<Type> res(COUNT);

    std::generate(vec.begin(), vec.end(), RandomGenerator<Type>::take);
    auto modifier_unary = [](auto& el) { return el + 1; };
    auto modifier       = [](auto& el) { ++el; };
    auto runtime        = [&](std::string label, auto foo) {
        std::cout << label;
        auto start = std::chrono::steady_clock::now();
        foo();
        auto end     = std::chrono::steady_clock::now();
        auto elapsed = (end - start);
        std::cout << "\ttime: ";
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count() << " ms ";
        std::cout << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() % 1000 << " us" << std::endl;
    };

    std::cout << "Transfrom" << std::endl;
    runtime("\tParallel", [&] { std::transform(std::execution::par, vec.begin(), vec.end(), res.begin(), modifier_unary); });
    runtime("\tParUnseq", [&] { std::transform(std::execution::par_unseq, vec.begin(), vec.end(), res.begin(), modifier_unary); });
    runtime("\tSequentl", [&] { std::transform(std::execution::seq, vec.begin(), vec.end(), res.begin(), modifier_unary); });
    runtime("\tUnsequen", [&] { std::transform(std::execution::unseq, vec.begin(), vec.end(), res.begin(), modifier_unary); });
    std::cout << "Sort" << std::endl;
    runtime("\tno policy", [&] { std::sort(vec.begin(), vec.end(), [](auto& l, auto& r) { return l < r; }); });
    std::generate(vec.begin(), vec.end(), RandomGenerator<Type>::take);
    runtime("\tParallel", [&] { std::sort(std::execution::par, vec.begin(), vec.end(), [](auto& l, auto& r) { return l < r; }); });
    std::generate(vec.begin(), vec.end(), RandomGenerator<Type>::take);
    runtime("\tParUnseq", [&] { std::sort(std::execution::par_unseq, vec.begin(), vec.end(), [](auto& l, auto& r) { return l < r; }); });
    std::generate(vec.begin(), vec.end(), RandomGenerator<Type>::take);
    runtime("\tSequentl", [&] { std::sort(std::execution::seq, vec.begin(), vec.end(), [](auto& l, auto& r) { return l < r; }); });
    std::generate(vec.begin(), vec.end(), RandomGenerator<Type>::take);
    runtime("\tUnsequen", [&] { std::sort(std::execution::unseq, vec.begin(), vec.end(), [](auto& l, auto& r) { return l < r; }); });

    std::cout << "++ for_each " << std::endl;
    runtime("\tno policy ", [&] { std::for_each(vec.begin(), vec.end(), modifier); });
    runtime("\tParunseq", [&] { std::for_each(std::execution::par_unseq, vec.begin(), vec.end(), modifier); });
    runtime("\tParallel", [&] { std::for_each(std::execution::par, vec.begin(), vec.end(), modifier); });
    runtime("\tSequentl", [&] { std::for_each(std::execution::seq, vec.begin(), vec.end(), modifier); });
    runtime("\tUnSequen", [&] { std::for_each(std::execution::unseq, vec.begin(), vec.end(), modifier); });

    // for (const auto& el : vec)
    //     std::cout << " " << el;
    // std::cout << std::endl;
    // for (const auto& el : res)
    //     std::cout << " " << el;
    // std::cout << std::endl;
}