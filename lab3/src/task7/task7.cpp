#include "task7.h"

#include <utils.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <execution>
#include <future>
#include <iostream>
#include <map>
#include <string_view>
#include <syncstream>
#include <thread>
#include <vector>

#include "RQueue.h"

using namespace std;
using Type = char;
void reader(RQueue<Type>& queue, stop_token st)
{
    try
    {
        while (not st.stop_requested())
        {
            std::osyncstream out(std::cout);
            auto             ch = queue.pop();
            std::this_thread::sleep_for(std::chrono::milliseconds(120));
            out << ch << ' ';
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what();
    }
    std::cout << " - reader " << std::this_thread::get_id() << std::endl;
}
void writer(RQueue<Type>& queue, stop_token st)
{
    try
    {
        while (not st.stop_requested())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            queue.push(utils::rand<char>());
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what();
    }
    std::cout << " - writer " << std::this_thread::get_id() << std::endl;
}
void Task7::operator()()
{
    cout << "Task 7" << std::endl;
    RQueue<char>     rqueue(200);
    std::stop_source ss;
    constexpr size_t ReadersCount{ 3 };
    constexpr size_t WritersCount{ 10 };
    for (size_t i = 0; i < ReadersCount; ++i)
    {
        std::jthread(reader, std::ref(rqueue), ss.get_token()).detach();
    }
    for (size_t i = 0; i < WritersCount; ++i)
    {
        std::jthread(writer, std::ref(rqueue), ss.get_token()).detach();
    }

    while (true)
    {
        auto c = std::cin.get();
        if (c == 'c')
            break;
        std::cout << "\nR size: " << rqueue.size() << '\n';
    }
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    // std::cout << "\nR size: " << rqueue.size() << '\n';
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    // std::cout << "\nR size: " << rqueue.size() << '\n';
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    ss.request_stop();
    std::this_thread::sleep_for(std::chrono::seconds(1));
}