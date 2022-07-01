#include "task7.h"

#include <utils.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <execution>
#include <future>
#include <iostream>
#include <map>
#include <string_view>
#include <syncstream>
#include <thread>
#include <vector>
#include "RQueue.h"
using namespace std;

void Task7::operator()()
{
    cout << "Task 7" << std::endl;
    RQueue<int, 500> rqueue;
}