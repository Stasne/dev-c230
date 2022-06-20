#include "task3.h"

#include <chrono>
#include <iostream>
#include <ratio>
#include <thread>
void Task3::operator()()
{
    std::cout << "Task 3" << std::endl;
    static const std::string Alphabet{ "ABCDEFGHIJKLMNOPQRSTUVWXYZ" };

    std::chrono::duration<double, std::ratio<1, 60>> duration{ Alphabet.size() };
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::milliseconds{ 1 });

    for (std::size_t i = 0; i < Alphabet.size(); ++i)
    {
        std::cout << Alphabet[i] << std::endl;
        std::this_thread::sleep_for(duration--);
    }
    std::cout << std::endl;
}