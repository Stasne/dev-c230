#ifndef __THREADPOOL__
#define __THREADPOOL__
#include <condition_variable>
#include <deque>
#include <future>
#include <mutex>
#include <vector>

class ThreadPool
{
public:
    ThreadPool(size_t threadCount);
    ~ThreadPool();
    ThreadPool(const ThreadPool& other)            = delete;
    ThreadPool& operator=(const ThreadPool& other) = delete;

    using TaskType = void(void);
    [[nodiscard]] std::future<void> push_task(std::packaged_task<TaskType>&&);
    void                            push_task(std::packaged_task<TaskType>&);
    void                            stop() const;

private:
    void poller();

private:
    std::vector<std::jthread>                threads_;
    std::deque<std::packaged_task<TaskType>> tasks_;
    mutable std::stop_source                 ss_;
    mutable std::condition_variable          cv_;
    std::mutex                               m_queue_;
};
#endif // __THREADPOOL__