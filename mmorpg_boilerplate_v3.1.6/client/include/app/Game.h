#pragma once

#include "game/world/World.h"
#include "network/NetworkClient.h"
#include <memory>
#include <string>

class Game
{
public:
    Game();
    ~Game();

    void Run();

private:
    std::unique_ptr<World> world_;
    NetworkClient networkClient_;

    std::string chatInput_;
    bool chatActive_ = false;
};