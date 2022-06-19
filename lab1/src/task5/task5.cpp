#include "task5.h"

#include <stdio.h>

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
void yell_async(std::stop_token stoken, std::string_view msg, std::mutex& m)
{
    auto     th_id = std::this_thread::get_id();
    uint64_t cnt{ 0 };
    while (not stoken.stop_requested())
    {
        ++cnt;
        std::lock_guard<std::mutex> lk(m);
        std::cout << "Id: " << th_id << " msg: " << msg << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(20)); // Чтоб остальные потоки закончили выводить свои крики
    std::lock_guard<std::mutex> lk(m);
    std::cout << "'" << msg << "' printed: \t\t" << cnt << std::endl;
}

void Task5::operator()()
{
    std::cout << "Task 5" << std::endl;
    static const std::vector<std::string> Phrases{
        "Hello", "Mad World", "Hawa you?", "I am fine", "Thank you", "ByeBye", "Good bye", "C you later", "Good night"
    };
    std::stop_source ss;
    std::mutex       m;
    std::cout << "Сейчас начнётся... Чтобы прекратить - нажите Enter" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    for (const std::string_view str : Phrases)
    {
        std::thread th(yell_async, ss.get_token(), str, std::ref(m));
        th.detach();
    }
    std::cin.get();
    ss.request_stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // для красоты вывода
    std::cout << std::endl
              << "request_stop() установлен. Потоки должны умереть.\n"
              << std::endl;

    std::this_thread::sleep_for(std::chrono::seconds(2));
}