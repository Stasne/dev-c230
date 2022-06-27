#include "task3.h"

#include <chrono>
#include <condition_variable>
#include <iostream>
#include <semaphore>
#include <string_view>
#include <thread>
#include <vector>

std::mutex              m;
std::condition_variable cvPing, cvPong;

auto speed = std::chrono::milliseconds(400);
auto acc   = std::chrono::milliseconds(10);

void PingPongPlayer(std::condition_variable& me, std::condition_variable& buddy, std::stop_token referee, std::string_view yell)
{
    while (not referee.stop_requested())
    {
        std::unique_lock<std::mutex> lk(m);
        me.wait(lk);
        std::cout << yell << std::endl;
        std::this_thread::sleep_for(speed);
        speed -= acc;
        buddy.notify_one();
    }
}

void Task3::operator()()
{
    std::stop_source       ss;
    constexpr const size_t PlayersCount{ 2 };

    std::vector<std::jthread> players;
    players.reserve(PlayersCount);

    players.emplace_back(PingPongPlayer, std::ref(cvPing), std::ref(cvPong), ss.get_token(), "ping");
    players.emplace_back(PingPongPlayer, std::ref(cvPong), std::ref(cvPing), ss.get_token(), "pong");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    cvPing.notify_one();
    std::cin.get();
    ss.request_stop();
}