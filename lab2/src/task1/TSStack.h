#ifndef __L2TSStack__
#define __L2TSStack__
#include <mutex>
#include <vector>
template <typename T>
class TSStack
{
public:
    TSStack(size_t capacity = 200)
        : data_(capacity){};
    TSStack(const TSStack<T>& other)
    {
        std::lock_guard<std::mutex> lk(other.m_);
        data_ = other.data_;
    };
    TSStack(const TSStack<T>&& other) = delete;
    TSStack<T>& operator=(const TSStack<T>& other)
    {
        std::lock(m_, other.m_);
        data_ = other.data_;
    };

    ~TSStack() = default;
    void push(T value)
    {
        std::lock_guard<std::mutex> lk(m_);
        data_.emplace_back(value);
    }
    void pop(T& value)
    {
        std::lock_guard<std::mutex> lk(m_);
        if (data_.empty())
            throw std::runtime_error("Attemptimg to pop from empty stack");
        value = std::move(data_.back());
        data_.pop_back();
    };
    [[nodiscard]] T pop() // WHYNOT?????????????
    {
        std::lock_guard<std::mutex> lk(m_);
        if (data_.empty())
            throw std::runtime_error("Attemptimg to pop from empty stack");
        T top = std::move(data_.back());
        data_.pop_back();
        return top; // copy elission/ std::move()?
    };
    // void clear();
    // void capacity() const;
    size_t size() const
    {
        std::lock_guard<std::mutex> lk(m_);
        return data_.size();
    };

private:
    std::vector<T>     data_;
    mutable std::mutex m_;
};

#endif // __L2TSStack__