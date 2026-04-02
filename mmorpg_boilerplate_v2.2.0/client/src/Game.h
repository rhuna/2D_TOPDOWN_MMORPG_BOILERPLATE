#pragma once

#include "raylib.h"
#include "NetworkClient.h"

// Example game class.
// Keep your existing world, map, camera, and gameplay members.
// The important addition is the NetworkClient member.
class Game
{
public:
    Game();

    // Initialize window/game state/network.
    void Init();

    // One frame of update logic.
    void Update(float dt);

    // One frame of rendering.
    void Render();

    // Cleanup if needed.
    void Shutdown();

    // Main loop helper if you use one.
    void Run();

private:
    // Local player world position.
    Vector2 playerPos_;

    // Camera that follows the local player.
    Camera2D camera_;

    // Networking client for multiplayer sync.
    NetworkClient network_;

    // Example movement speed in world units per second.
    float playerSpeed_ = 180.0f;
};