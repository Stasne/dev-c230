#include "task1.h"

#include <random_generator.h>
#include <stdio.h>

#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

#include "TSStack.h"
std::mutex                      MEx;
std::vector<std::exception_ptr> Exceptions;

template <typename T>
void producerThread(TSStack<T>& stack, std::stop_token stoken)
{
    try
    {
        while (not stoken.stop_requested())
        {
            auto v = RandomGenerator<T>::take();
            stack.push(v);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
    catch (...)
    {
        std::lock_guard<std::mutex> lk(MEx);
        Exceptions.push_back(std::current_exception());
    }
}
template <typename T>
void consumerThread(TSStack<T>& stack, std::stop_source& stoken)
{
    try
    {
        while (not stoken.stop_requested())
        {
            T val;
            stack.pop(val);
            std::cout << "st sz:" << stack.size() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    catch (...)
    {
        std::lock_guard<std::mutex> lk(MEx);
        Exceptions.push_back(std::current_exception());
        stoken.request_stop();
    }
}
void Task1::operator()()
{
    std::cout << "Task 1 " << std::endl;
    using stype = int;
    TSStack<stype> stack;

    const size_t ThreadsCount{ 15 };

    std::vector<std::jthread> threads;
    threads.reserve(ThreadsCount);
    std::stop_source stopper;
    for (size_t i = 0; i < ThreadsCount; ++i)
    {
        threads.emplace_back(producerThread<stype>, std::ref(stack), stopper.get_token());
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::jthread consumer(consumerThread<stype>, std::ref(stack), std::ref(stopper));

    consumer.join();
    for (auto& ex : Exceptions)
    {
        try
        {
            if (ex != nullptr)
                std::rethrow_exception(ex);
        }
        catch (const std::exception& e)
        {
            std::cerr << "EXCEPTION: " << e.what() << '\n';
        }
    }
}
