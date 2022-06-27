#ifndef __L2TSStack__
#define __L2TSStack__
#include <mutex>
#include <shared_mutex>
#include <vector>
template <typename T>
class TSStack
{
public:
    TSStack(size_t capacity = 200)
        : data_(capacity){};
    TSStack(const TSStack<T>& other)
    {
        // shared mutex
        std::shared_lock<std::shared_mutex> lk(other.m_);
        data_ = other.data_;
        size_ = other.size_;
    };
    TSStack(TSStack<T>&& other)
    {
        std::lock_guard<std::shared_mutex> lk(other.m_); // required?
        data_.swap(other.data_);
        size_ = other.size_;
    };
    TSStack<T>& operator=(const TSStack<T>& other)
    {
        // shared_mutex
        std::unique_lock<std::shared_mutex> lk(m_, std::defer_lock);
        std::shared_lock                    shlk(other.m_, std::defer_lock);
        // wrap mutexes
        std::lock(lk, shlk);
        data_ = other.data_;
    };
    // move operator=()
    TSStack<T>& operator=(TSStack<T>&& other)
    {
        std::lock_guard<std::shared_mutex> shlk(other.m_);
        data_.swap(other.data_);
    }

    ~TSStack() = default;
    void push(T value)
    {
        std::lock_guard<std::shared_mutex> lk(m_);
        data_.emplace_back(std::move(value));
    }
    void pop(T& value)
    {
        std::lock_guard<std::shared_mutex> lk(m_);
        if (data_.empty())
            throw std::runtime_error("Attemptimg to pop from empty stack");
        value = std::move(data_.back());
        data_.pop_back();
    };
    [[nodiscard]] T pop()
    {
        std::lock_guard<std::shared_mutex> lk(m_);
        if (data_.empty())
            throw std::runtime_error("Attemptimg to pop from empty stack");
        T top = std::move_if_noexcept(data_.back()); // rework to -> if no except
        data_.pop_back();
        return top;
    };

    size_t size() const
    {
        std::shared_lock lk(m_);
        return data_.size();
    };

private:
    std::vector<T>            data_;
    size_t                    size_;
    size_t                    capacity_;
    mutable std::shared_mutex m_;
};

#endif // __L2TSStack__