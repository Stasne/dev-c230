#include <chrono>
#include <iostream>
#include <semaphore>
#include <string_view>
#include <thread>
#include <vector>

#include "task2.h"

std::counting_semaphore<1> S(1);

auto speed = std::chrono::milliseconds(400);
auto acc   = std::chrono::milliseconds(10);
void PingPongPlayer(std::stop_token referee, std::string_view yell)
{
    while (not referee.stop_requested())
    {
        S.acquire();
        std::cout << yell << std::endl;
        std::this_thread::sleep_for(speed);
        speed -= acc;
        S.release();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
void Task2::operator()()
{
    std::stop_source ss;
    const size_t     PlayersCount{ 2 };

    std::vector<std::jthread> players;
    players.reserve(PlayersCount);

    for (size_t i = 0; i < PlayersCount; ++i)
    {
        players.emplace_back(PingPongPlayer, ss.get_token(), i % 2 ? "pong" : "ping");
    }
    std::cin.get();
    ss.request_stop();
}