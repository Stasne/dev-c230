#ifndef __SPINMUTEX__
#define __SPINMUTEX__

#include <atomic>
class SpinMutex
{
public:
    void lock()
    {
        while (flag_.test_and_set(std::memory_order_acquire))
            ;
    }
    void unlock() { flag_.clear(std::memory_order_release); }
    bool trylock() { return !flag_.test_and_set(std::memory_order_acquire); }

private:
    std::atomic_flag flag_;
};

#endif //__SPINMUTEX__