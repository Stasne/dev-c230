#ifndef __TSSINGLETONE__
#define __TSSINGLETONE__
#include <cstdlib>
#include <string>
#include <thread>
class TSSingletone
{
public:
    static TSSingletone& instance()
    {
        static thread_local TSSingletone me;
        return me;
    }
    auto creatorId() { return id_; }

    TSSingletone(const TSSingletone& other)             = delete;
    TSSingletone(const TSSingletone&& other)            = delete;
    TSSingletone& operator=(const TSSingletone& other)  = delete;
    TSSingletone& operator=(const TSSingletone&& other) = delete;

private:
    TSSingletone() { id_ = std::this_thread::get_id(); }

    std::string     data_;
    std::thread::id id_;
};

#endif // __TSSINGLETONE__