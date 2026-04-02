#include "core/Game.h"
#include "world/World.h"
#include "net/NetworkClient.h"
#include <memory>
#include <raylib.h>

void Game::Run()
{
    const int screenWidth = 1024;
    const int screenHeight = 768;

    InitWindow(screenWidth, screenHeight, "TopDownMMOStarter - Networked Sprite Build");
    SetTargetFPS(60);

    world_ = std::make_unique<World>();

    // Must match the server port
    networkClient_.Connect("127.0.0.1", 7777);

    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();

        world_->Update(dt);

        Vector2 playerPos = world_->GetPlayerPosition();
        networkClient_.SendMove(playerPos.x, playerPos.y);

        if (networkClient_.IsConnected())
        {
            world_->UpdateRemotePlayers(
                networkClient_.GetRemotePlayers(),
                networkClient_.GetLocalId(),
                dt);
        }
        else
        {
            world_->UpdateRemotePlayers({}, -1, dt);
        }

        BeginDrawing();
        ClearBackground(BLACK);
        world_->Draw();
        EndDrawing();
    }

    networkClient_.Disconnect();
    world_.reset();
    CloseWindow();
}