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
        : capacity_(size)
        , size_(0)
        , head_(0)
        , tail_(0)
        , data_(std::make_unique<T[]>(capacity_))
    {
    }
    ~RQueue()
    {
        stopRequested_.test_and_set();
        readyPop_.notify_all();
        readyPush_.notify_all();
    }
    RQueue(RQueue&& other)
    {
        std::unique_lock<std::mutex> rlk(other.mPop_, std::defer_lock);
        std::unique_lock<std::mutex> wlk(other.mPush_, std::defer_lock);
        std::lock(rlk, wlk);
        swap(*this, other);
    }
    friend void swap(RQueue& l, RQueue& r)
    {
        r.size_.exchange(l.size_.exchange(r.size_));
        r.head_.exchange(l.head_.exchange(r.head_));
        r.tail_.exchange(l.tail_.exchange(r.tail_));
        std::swap(l.capacity_, r.capacity_);
        std::swap(l.data_, r.data_);
    }
    RQueue& operator=(RQueue&& other)
    {
        std::unique_lock<std::mutex> rlk(other.mPop_, std::defer_lock);
        std::unique_lock<std::mutex> wlk(other.mPush_, std::defer_lock);
        std::unique_lock<std::mutex> mrlk(mPop_, std::defer_lock);
        std::unique_lock<std::mutex> mwlk(mPush_, std::defer_lock);
        std::lock(rlk, wlk, mwlk, mrlk);
        swap(*this, other);
        return *this;
    }
    [[nodiscard]] T pop()
    {
        std::unique_lock<std::mutex> lock(mPop_);
        if (not readyPop_.wait_for(lock, std::chrono::milliseconds(PopTimeout_ms), [&] { return (size_.load()) and (not stopRequested_.test()); }))
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
        if (not readyPush_.wait_for(lock, std::chrono::milliseconds(PushTimeout_ms), [&] { return (size_.load() != capacity_) and (not stopRequested_.test()); }))
            throw std::runtime_error("No space/readers");

        data_[head_] = std::move(val);
        ++size_;
        readyPop_.notify_one();
        head_ = (head_ + 1) % capacity_;
    };

    size_t size() const { return size_.load(); }
    size_t capacity() const { return capacity_; }

private:
    static constexpr size_t PopTimeout_ms{ 50 };
    static constexpr size_t PushTimeout_ms{ 50 };
    std::atomic<size_t>     size_{};
    size_t                  capacity_{};
    std::atomic<size_t>     head_{}, tail_{};
    std::unique_ptr<T[]>    data_;
    std::atomic_flag        stopRequested_;
    std::condition_variable readyPop_, readyPush_;
    std::mutex              mPop_, mPush_;
};

#endif // __RQUEUE__