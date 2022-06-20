#include "task1.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <string>
#include <thread>
#include <vector>

namespace fs = std::filesystem;

void makeUppercaseCopy(const std::string& file)
{
    const static std::string COPY_PREFIX{ "uc_" };
    std::string              outputFilename{ "out/" + std::string(file) };
    outputFilename.replace(outputFilename.find_last_of('/'), 1, '/' + COPY_PREFIX);

    std::ifstream reader(file.data());
    std::ofstream writer(outputFilename, std::ios_base::trunc);
    std::transform(std::istreambuf_iterator<char>(reader), std::istreambuf_iterator<char>(),
                   std::ostreambuf_iterator<char>(writer), toupper);
}

void makeUppercaseCopy_interval(const std::vector<std::string>& files, size_t from, size_t to)
{
    for (size_t i = from; i < to; ++i)
    {
        makeUppercaseCopy(files[i]);
    }
}

void Task1::task1a(std::vector<std::string>& files)
{
    for (const auto& file : files)
    {
        std::thread t{ makeUppercaseCopy, std::ref(file) };
        t.join();
    }
}
void Task1::task1b(std::vector<std::string>& files)
{
    std::vector<std::thread> threads;
    threads.reserve(files.size());
    for (const auto file : files)
        threads.emplace_back(makeUppercaseCopy, std::ref(file));

    for (auto& th : threads)
        th.join();
}
void Task1::task1c(std::vector<std::string>& files)
{
    const uint64_t T_COUNT = std::thread::hardware_concurrency();

    size_t groupSize = files.size() / T_COUNT;

    std::vector<std::thread> threads;
    threads.reserve(T_COUNT);
    for (size_t i = 0; i < T_COUNT; ++i)
    {
        size_t lastIndex{ 0 };
        if (i == T_COUNT - 1)
            lastIndex = files.size();
        else
            lastIndex = i * groupSize + groupSize + 1;

        threads.emplace_back(std::thread(makeUppercaseCopy_interval, std::ref(files), i * groupSize, lastIndex));
    }

    for (size_t i = 0; i < T_COUNT; ++i)
        threads[i].join();
}
void Task1::operator()()
{
    std::cout << "Task 1" << std::endl;
    // prepare vector of file names
    std::vector<std::string> files{};
    const std::string        path = "texts";
    for (const auto& entry : fs::directory_iterator(path))
    {
        // if file extension is not .txt, skip it
        if (entry.path().extension() != ".txt")
            continue;
        files.push_back(entry.path().c_str());
    }

    auto funcWrapper = [&](std::function<void(std::vector<std::string>&)> func) {
        auto start = std::chrono::steady_clock::now();
        func(files);
        auto end = std::chrono::steady_clock::now();
        auto ms  = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        auto us  = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "\t FINISHED. Elapsed: " << ms << "ms " << us % 1000 << "us" << std::endl;
    };

    std::cout << "1.a. sequential" << std::endl;
    funcWrapper(std::bind(&Task1::task1a, this, std::placeholders::_1));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "1.b. simultaneously" << std::endl;
    funcWrapper(std::bind(&Task1::task1b, this, std::placeholders::_1));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "1.c. core threads" << std::endl;
    funcWrapper(std::bind(&Task1::task1c, this, std::placeholders::_1));
}
