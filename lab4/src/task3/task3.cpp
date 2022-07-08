#include "task3.h"

#include <utils.h>

#include <chrono>
#include <condition_variable>
#include <future>
#include <iostream>
#include <mutex>
#include <semaphore>
#include <string_view>
#include <thread>
#include <vector>

#include "SpinMutex.h"
template <typename T>
class Aatomic
{
public:
    Aatomic() { ++cnt_; }
    ~Aatomic() { --cnt_; }
    size_t count() const { return cnt_.load(); }

private:
    T                                 data_;
    inline static std::atomic<size_t> cnt_;
};
template <typename T, typename Mut>
class Alock
{
public:
    Alock()
    {
        std::lock_guard<Mut> lk(m_);
        ++cnt_;
    }
    ~Alock()
    {
        std::lock_guard<Mut> lk(m_);
        --cnt_;
    }
    size_t count() const { return cnt_; }

private:
    Mut                  m_;
    T                    data_;
    inline static size_t cnt_;
};

void Task3::operator()()
{
    auto createSome = [&](auto val) {
        auto spv = new decltype(val)();
        // std::this_thread::sleep_for(std::chrono::milliseconds(20));
    };
    constexpr size_t               Count = 13256;
    std::vector<std::future<void>> ftrs;
    ftrs.reserve(Count);
    using MutexType = SpinMutex;
    Alock<int, MutexType> tester;

    auto LockTest = [&](auto mutex) {
        for (size_t i = 0; i < Count; ++i)
            // if (i % 2)
            ftrs.emplace_back(std::async(std::launch::async, [&] { createSome(Alock<int, decltype(mutex)>()); }));
        // else
        // ftrs.emplace_back(std::async(std::launch::async, [&] { createSome(Alock<float, decltype(mutex)>()); }));
        for (auto& f : ftrs)
            f.get();
    };
    auto AtomicTest = [&] {
        for (size_t i = 0; i < Count; ++i)
            ftrs.emplace_back(std::async(std::launch::async, [&] { createSome(Aatomic<int>()); }));

        for (auto& f : ftrs)
            f.get();
    };

    utils::time([&] { LockTest(std::mutex()); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ftrs.clear();
    utils::time([&] { LockTest(SpinMutex()); });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ftrs.clear();
    utils::time(AtomicTest);
}