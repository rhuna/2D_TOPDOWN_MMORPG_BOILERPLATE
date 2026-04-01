#pragma once

#include "World.h"
#include <memory>

class Game {
public:
    void Run();

private:
    std::unique_ptr<World> world_;
};
