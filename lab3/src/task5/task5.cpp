#include "task5.h"

#include <utils.h>

#include <algorithm>
#include <chrono>
#include <execution>
#include <future>
#include <iostream>
#include <map>
#include <string_view>
#include <syncstream>
#include <thread>
#include <vector>

using namespace std;
osyncstream out(std::cout);

std::string RandomString(const int len)
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i)
    {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    return tmp_s;
}
void PrintRandomString(ostream& stream)
{
    stream << this_thread::get_id() << "str: " << RandomString(10) << '\n';
    return;
}

void Task5::operator()()
{
    auto ThreadsCount = thread::hardware_concurrency();

    packaged_task<decltype(PrintRandomString)> task(PrintRandomString);
    vector<future<void>>                       results;
    results.reserve(ThreadsCount);
    out << "ThreadsCount: " << ThreadsCount << '\n';
    for (size_t i = 0; i < ThreadsCount; ++i)
    {
        results.emplace_back(task.get_future());
        std::thread(std::ref(task), std::ref(out)).detach();
        task.reset();
    }
    for (auto& res : results)
        res.wait();
}