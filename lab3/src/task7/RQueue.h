#ifndef __RQUEUE__
#define __RQUEUE__
#include <atomic>
#include <condition_variable>
#include <memory>
#include <vector>
template <typename T>
class RQueue
{
public:
    RQueue(const size_t size)
        : capacity_(size > 2 ? size : 2)
        , size_(0)
        , head_(0)
        , tail_(0)
        , data_(std::make_unique<T[]>(capacity_))
    {
    }

    [[nodiscard]] T pop()
    {
        std::unique_lock<std::mutex> lock(mPop_);
        if (not readyPop_.wait_for(lock, std::chrono::milliseconds(100), [&] { return size_.load(); }))
            throw std::runtime_error("No writers");
        auto val = std::move(data_[tail_]);
        --size_;
        readyPush_.notify_one();
        tail_ = (tail_ + 1) % capacity_;
        return val;
    };
    void push(T val)
    {
        std::unique_lock<std::mutex> lock(mPush_);
        if (not readyPush_.wait_for(lock, std::chrono::milliseconds(100), [&] { return size_.load() != capacity_; }))
            throw std::runtime_error("No space/readers");

        data_[head_] = std::move(val);
        ++size_;
        readyPop_.notify_one();
        head_ = (head_ + 1) % capacity_;
    };
    size_t size() const { return size_.load(); }

private:
    std::atomic<size_t>  size_;
    size_t               capacity_;
    std::atomic<size_t>  head_, tail_;
    std::unique_ptr<T[]> data_;

    std::condition_variable readyPop_, readyPush_;
    std::mutex              mPop_, mPush_;
};

#endif // __RQUEUE__