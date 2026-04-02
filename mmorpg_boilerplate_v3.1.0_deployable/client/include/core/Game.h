#pragma once

#include "net/NetworkClient.h"
#include "world/World.h"
#include "raylib.h"
#include <memory>

class World;

class Game {
public:
    void Run();
private:
    std::unique_ptr<World> world_;
    NetworkClient networkClient_;
};
