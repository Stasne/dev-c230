#include "TSStack.h"

template <class T>
TSStack<T>::TSStack(const TSStack<T>& other)
{
    std::lock_guard<std::mutex> lk(other.m_);
    data_ = other.data_;
}
template <class T>
TSStack<T>& TSStack<T>::operator=(const TSStack<T>& other)
{
    std::lock(m_, other.m_);
    data_ = other.data_;
}
// template <class T>
// void TSStack<T>::push(T value)
// {
//     std::lock_guard<std::mutex> lk(m_);
//     data_.emplace_back(value);
// }
// template <class T>
// void TSStack<T>::pop(T& value)
// {
//     std::lock_guard<std::mutex> lk(m_);
//     if (data_.empty())
//         throw;
//     value = std::move(data_.back());
//     data_.pop_back();
// }
// template <class T>
// T TSStack<T>::pop() // Why not???????????????????????????
// {
//     std::lock_guard<std::mutex> lk(m_);
//     if (data_.empty())
//         throw;
//     T top = std::move(data_.back());
//     data_.pop_back();
//     return top; // copy elission/ std::move()?
// }

// template <class T>
// void TSStack<T>::clear()
// {
//     std::lock_guard<std::mutex> lk(m_);
//     data_.clear();
// }
// template <class T>
// void TSStack<T>::capacity() const
// {
//     return data_.capacity();
// }
// template <class T>
// void TSStack<T>::size() const
// {

// }