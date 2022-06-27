#ifndef __UTILS__
#define __UTILS__
#include <algorithm>
#include <chrono>
#include <functional>
#include <iostream>
#include <random>

namespace utils
{
    // template <typename... T>
    // void time(std::function<void(T... args)> args)
    void time(std::function<void()> func)
    {
        auto start = std::chrono::steady_clock::now();
        func();
        auto end = std::chrono::steady_clock::now();
        auto ms  = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        auto us  = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "\t Elapsed: " << ms << "ms " << us % 1000 << "us" << std::endl;
    };

    template <typename T>
    static T rand()
    {
        static std::random_device               rnd_device;
        static std::mt19937                     mersenne_engine{ rnd_device() };
        static std::uniform_int_distribution<T> dist{ std::numeric_limits<T>::min(), std::numeric_limits<T>::max() };
        return dist(mersenne_engine);
    }
};

#endif // __UTILS__