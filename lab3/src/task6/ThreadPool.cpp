#include "ThreadPool.h"

#include <iostream>
#include <syncstream>

ThreadPool::ThreadPool(size_t count)
{
    threads_.reserve(count);
    for (size_t i = 0; i < count; ++i)
    {
        threads_.emplace_back(&ThreadPool::poller, this);
    }
}
ThreadPool::~ThreadPool()
{
    stop();
}
void ThreadPool::poller()
{
    thread_local size_t counter = 0;
    auto                st      = ss_.get_token();
    // while (not ss_.stop_requested()) //works bad
    while (not st.stop_requested())
    {
        std::unique_lock<std::mutex> lk(m_queue_);
        if (not tasks_.empty())
        {
            auto task = std::move(tasks_.front());
            tasks_.pop_front();
            lk.unlock();
            task();
            ++counter;
            lk.lock();
        }
        cv_.wait(lk, [&] {
            // return (!tasks_.empty() || ss_.stop_requested()); // works bad
            return (!tasks_.empty() || st.stop_requested());
        });
    }
    // std::osyncstream osync(std::cout);
    // std::cout << std::this_thread::get_id() << ": made " << std::dec << counter << " tasks\n";
}
std::future<void> ThreadPool::push_task(std::packaged_task<TaskType>&& task)
{
    std::lock_guard<std::mutex> lock(m_queue_);

    auto fut = task.get_future();
    tasks_.emplace_back(std::move(task));
    cv_.notify_one();
    return fut; // auto-move
}

void ThreadPool::push_task(std::packaged_task<TaskType>& task)
{
    std::lock_guard<std::mutex> lock(m_queue_);
    tasks_.emplace_back(std::move(task));
    cv_.notify_one();
}

void ThreadPool::stop() const
{
    ss_.request_stop();
    cv_.notify_all();
}