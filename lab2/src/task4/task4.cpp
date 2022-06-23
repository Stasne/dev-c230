#include "task4.h"

#include <chrono>
#include <iostream>
#include <string_view>
#include <syncstream>
#include <thread>
#include <vector>

thread_local std::string wilkommen{ "Howdy ho? " };

void ConcatAndOut(const std::string& str)
{
    auto out = std::osyncstream(std::cout);
    out << str << " before & : " << &wilkommen << std::endl;
    wilkommen += str;
    out << str << " after & : " << &wilkommen << std::endl; // outs at destructor
}
void Task4::operator()()
{
    constexpr size_t         ThreadsCount{ 5 };
    std::vector<std::thread> threads;
    threads.reserve(ThreadsCount);
    for (size_t i = 0; i < ThreadsCount; ++i)
    {
        threads.emplace_back(ConcatAndOut, std::to_string(i));
    }
    for (auto& th : threads)
        th.join();
}