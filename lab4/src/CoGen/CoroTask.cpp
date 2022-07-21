#include "CoroTask.h"

#include <algorithm>
#include <iostream>
#include <vector>

#include "Generator.h"

Generator<int> coIntGen(int start, int step)
{
    int val = start;
    while (val < 23)
    {
        co_yield val;
        val += step;
    }
    // co_return val;
}

void CoroTask::operator()()
{

    constexpr size_t Count{ 50000 };
    auto             gen = coIntGen(1, 1);
    std::cout << "CoroTask" << std::endl;
    for (auto el : gen)
    {
        std::cout << " " << el; // std::endl;
    }
}
