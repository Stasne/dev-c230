#ifndef __RANDOM_GENERATOR_H__
#define __RANDOM_GENERATOR_H__
#include <algorithm>
#include <random>

class RandomGenerator
{
public:
    template <typename T>
    static T take()
    {
        static std::random_device               rnd_device;
        static std::mt19937                     mersenne_engine{ rnd_device() }; // Generates random integers
        static std::uniform_int_distribution<T> dist{ std::numeric_limits<T>::min(), std::numeric_limits<T>::max() };
        return dist(mersenne_engine);
    }
};

#endif // __RANDOM