#ifndef __RQUEUE__
#define __RQUEUE__
#include <atomic>
#include <condition_variable>
#include <vector>

template <typename T, unsigned N>
class RQueue
{
public:
    RQueue(const size_t size = N) { buff_.reserve(size); }
    //     std::condition_variable& cvPop() {return readyRead_;};
    //     std::condition_variable& cvPush() {return readyPush_;};
    // private:
    //     std::atomic<T*> head, tail;
    std::vector<T> buff_;
    //     std::condition_variable readyPop_, readyPush_;
    //     std::mutex  mPop_, mPush_;
};

#endif // __RQUEUE__