#include "task2.h"

#include <chrono>
#include <iostream>
#include <semaphore>
#include <string_view>
#include <thread>
#include <vector>

std::counting_semaphore<1> Sping(0);
std::counting_semaphore<1> Spong(0);

auto speed = std::chrono::milliseconds(400);
auto acc   = std::chrono::milliseconds(10);
void PingPongPlayer(std::counting_semaphore<1>& me, std::counting_semaphore<1>& buddy, std::stop_token referee, std::string_view yell)
{
    while (not referee.stop_requested())
    {
        me.acquire();
        std::cout << yell << std::endl;
        std::this_thread::sleep_for(speed);
        speed -= acc;
        buddy.release();
    }
}
void Task2::operator()()
{
    std::stop_source       ss;
    constexpr const size_t PlayersCount{ 2 };

    std::vector<std::jthread> players;
    players.reserve(PlayersCount);

    players.emplace_back(PingPongPlayer, std::ref(Sping), std::ref(Spong), ss.get_token(), "ping");
    players.emplace_back(PingPongPlayer, std::ref(Spong), std::ref(Sping), ss.get_token(), "pong");
    Sping.release();
    std::cin.get();
    ss.request_stop();
}