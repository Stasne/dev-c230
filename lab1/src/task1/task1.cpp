#include "task1.h"

#include <algorithm>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

namespace fs = std::filesystem;
void makeUppercaseCopy(std::vector<std::string>& files, size_t from = 0, size_t to = 1)
{
    assert(files.size() >= to);
    assert(from < to);
    const static std::string COPY_PREFIX{ "uc_" };

    std::ifstream reader;
    std::ofstream writer;

    auto readToBuff = [&](const std::string& filename) -> std::vector<char> {
        reader.open(filename.data(), std::ios_base::in);
        if (reader.fail())
        {
            std::cerr << "Failed to open file for reading" << filename << std::endl;
        }
        std::vector<char> buff((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());
        reader.close();
        return buff;
    };

    auto writeToFile = [&](const std::vector<char>& buff, const std::string& filename) {
        writer.open(filename, std::ios_base::trunc);
        if (writer.fail())
        {
            std::cerr << "Failed to open file for writing" << filename << std::endl;
        }
        writer.write(buff.data(), buff.size());
        writer.close();
    };

    for (size_t i = from; i < to; ++i)
    {
        // read
        auto buff = readToBuff(files[i]);
        // transform
        std::transform(buff.begin(), buff.end(), buff.begin(), ::toupper);
        // redirect + rename
        std::string outputFilename{ "out/" + files[from] };
        outputFilename.replace(outputFilename.find_last_of('/'), 1, '/' + COPY_PREFIX);
        // write
        writeToFile(buff, outputFilename);
    }
}

void Task1::task1a(std::vector<std::string>& files) // why can't const?
{
    for (size_t i = 0; i < files.size(); ++i)
    {
        std::thread t{ makeUppercaseCopy, std::ref(files), i, i + 1 };
        t.join();
    }
}
void Task1::task1b(std::vector<std::string>& files)
{
    std::vector<std::thread> threads;
    for (size_t i = 0; i < files.size(); ++i)
    {
        threads.push_back(std::thread(makeUppercaseCopy, std::ref(files), i, i + 1));
    }

    for (size_t i = 0; i < files.size(); ++i)
        threads[i].join();
}
void Task1::task1c(std::vector<std::string>& files)
{
    const uint64_t T_COUNT = std::thread::hardware_concurrency();

    // std::cout << "\t threads count: " << T_COUNT << std::endl;
    size_t groupSize = files.size() / T_COUNT;

    // auto                     start = std::chrono::steady_clock::now();
    std::vector<std::thread> threads;
    for (size_t i = 0; i < T_COUNT; ++i)
    {
        size_t lastIndex{ 0 };
        if (i == T_COUNT - 1)
            lastIndex = files.size();
        else
            lastIndex = i * groupSize + groupSize + 1;

        threads.push_back(std::thread(makeUppercaseCopy, std::ref(files), i * groupSize, lastIndex));
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
        files.push_back(entry.path());
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
    funcWrapper(std::bind(&Task1::task1a, this, std::placeholders::_1)); // confusing
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "1.b. simultaneously" << std::endl;
    funcWrapper(std::bind(&Task1::task1b, this, std::placeholders::_1));
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "1.c. core threads" << std::endl;
    funcWrapper(std::bind(&Task1::task1c, this, std::placeholders::_1));
}
