#include "task1.h"

#include <stdio.h>
#include <utils.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <future>
#include <iostream>
#include <iterator>
#include <latch>
#include <map>
#include <mutex>
#include <sstream>
#include <thread>
#include <unordered_map>
static const std::string Text{ "Notes: Here the barrier count is the number of worker threads. It is guaranteed that each thread will pop precisely one workerFn off of the queue and handle it. Once a thread has popped one off of the queue, it will wait in barrier.count_down_and_wait() until all the other copies of workerFn have been popped off by other threads, so there is no chance of it popping another one off.I used a shared pointer to the barrier so that it will be destroyed automatically once all the work items are done. This wasn't an issue with the latch because there we could just make it a local variable in the producer thread function, because it waits until the worker threads have used the latch (it calls latch.wait()). Here the producer thread doesn't wait for the barrier so we need to manage the memory in a different way. If you did want the original producer thread to wait until the barrier has been finished, that's fine, it can call count_down_and_wait() too, but you will obviously need to pass worker_count + 1 to the barrier's constructor. (And then you wouldn't need to use a shared pointer for the barrier.) If other work items are being pushed onto the queue at the same time, that's fine too, although it will potentially waste time as some threads will just be sitting there waiting for the barrier to be acquired while other threads are distracted by other work before they acquire the barrier.!!! DANGER !!!The last bullet point about other working being pushed onto the queue being fine  is only the case if that other work doesn't also use a barrier! If you have two different producer threads putting work items with a barrier on to the same queue and those items are interleaved, then some threads will wait on one barrier and others on the other one, and neither will ever reach the required wait count - DEADLOCK. One way to avoid this is to only ever use barriers like this from a single thread, or even to only ever use one barrier in your whole program (this sounds extreme but is actually quite a common strategy, as barriers are often used for one-time initialisation on startup). Another option, if the thread queue you're using supports it, is to atomically push all work items for the barrier onto the queue at once so they're never interleaved with any other work items. (This won't work with the moodycamel queue, which supports pushing multiple items at once but doesn't guarantee that they won't be interleved with items pushed on by other threads.)" };

void Task1::operator()()
{
    std::unordered_map<char, std::atomic<size_t>> counts;
    std::vector<std::string>                      words;
    std::istringstream                            iss(Text);
    copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), back_inserter(words));
    std::latch finish{ 26 };
    auto       countStartLetters = [&](char c) {
        for (const auto& w : words)
        {
            if (w.starts_with(c) or w.starts_with(std::toupper(c)))
            {
                ++counts[c];
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        finish.count_down();
    };
    for (char c = 'a'; c <= 'z'; ++c)
        std::thread(countStartLetters, c).detach();
    std::cout << "w8ing till finish\n";
    finish.wait();
}
