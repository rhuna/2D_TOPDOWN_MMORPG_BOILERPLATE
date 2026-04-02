#pragma once

#include "net/NetworkClient.h"
#include "world/World.h"
#include <memory>

// Game is the high-level application shell.
//
// Responsibilities:
// - create the window
// - drive the main loop
// - own the World object
// - own the optional multiplayer client connection
//
// Non-responsibilities:
// - game rules
// - tile drawing
// - combat logic
// - minimap logic
//
// Those belong to the World class.
class Game {
public:
    // Run the complete application lifecycle.
    void Run();

private:
    // The world is heap-allocated here mainly to keep Game tiny and explicit.
    std::unique_ptr<World> world_;

    // Optional prototype networking client.
    // If no server is running, the overworld still works in single-player mode.
    NetworkClient network_;
};
