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
atomic<size_t> readers{ 0 };
atomic<size_t> writers{ 0 };

void reader(RQueue<Type>& queue, stop_token st)
{
    try
    {
        while (not st.stop_requested())
        {
            std::osyncstream out(std::cout);
            auto             ch = queue.pop();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            out << ch << ' ';
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what();
    }
    std::cout << " - reader " << std::this_thread::get_id() << std::endl;
    --readers;
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
    --writers;
}
void Task7::operator()()
{
    cout << "Task 7" << std::endl;
    RQueue<Type>     rqueue(200);
    std::stop_source ss;
    constexpr size_t ReadersCount{ 3 };
    constexpr size_t WritersCount{ 10 };
    for (size_t i = 0; i < ReadersCount; ++i)
    {
        std::jthread(reader, std::ref(rqueue), ss.get_token()).detach();
        ++readers;
    }
    for (size_t i = 0; i < WritersCount; ++i)
    {
        std::jthread(writer, std::ref(rqueue), ss.get_token()).detach();
        ++writers;
    }
    RQueue<Type> sq(0);
    while (true)
    {
        auto c = std::cin.get();
        if (c == 'q') // quit
            break;
        if (c == 'w') // switch queue with empty
            sq = std::move(rqueue);
        if (c == 'r') // threads status
        {
            std::cout << "Reader threads: " << readers.load() << '\n';
            std::cout << "Writer threads: " << writers.load() << '\n';
        }
        std::cout << "\nR storage: " << std::dec << rqueue.size() << "/" << rqueue.capacity() << '\n';
    }

    ss.request_stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}