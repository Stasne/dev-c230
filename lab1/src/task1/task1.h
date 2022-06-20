#ifndef __TASK1_H__
#define __TASK1_H__
#include <iosfwd>
#include <string_view>
#include <vector>
class Task1
{
public:
    Task1()  = default;
    ~Task1() = default;
    void operator()();

private:
    void task1a(std::vector<std::string>& files);
    void task1b(std::vector<std::string>& files);
    void task1c(std::vector<std::string>& files);
};

#endif // __TASK1_H__