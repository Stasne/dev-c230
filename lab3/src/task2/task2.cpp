#include "task2.h"

#include <chrono>
#include <future>
#include <iostream>
#include <semaphore>
#include <string_view>
#include <thread>
#include <vector>

void Task2::operator()()
{
    // WIP

    std::future<int> res;
    while (true)
    {
        auto fut = std::async(std::launch::async, []() {
            std::string str;
            std::getline(std::cin, str);
            int result = std::stoi(str);
            if (result != std::stof(str))
                throw std::runtime_error("Float NUMBER");
            return std::stoi(str);
        });
        try
        {
            auto async_res = fut.get();
            std::cout << "async res: " << async_res << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cerr << "exception: " << e.what() << '\n';
            break;
        }
    }
}