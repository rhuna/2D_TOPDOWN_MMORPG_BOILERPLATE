#include "core/Game.h"
#include <memory>
#include <raylib.h>

void Game::Run() {
    const int screenWidth = 1024;
    const int screenHeight = 768;

    InitWindow(screenWidth, screenHeight, "TopDownMMOStarter - Networked Sprite Build");
    SetTargetFPS(60);

    world_ = std::make_unique<World>();

    // Attempt to connect to the local prototype server.
    // If it is not running, the build still behaves as a single-player overworld.
    network_.Connect("127.0.0.1", "54000");

    while (!WindowShouldClose()) {
        const float dt = GetFrameTime();
        world_->Update(dt);

        if (network_.IsConnected()) {
            Vector2 playerPos = world_->GetPlayerPosition();
            network_.SendPosition(playerPos.x, playerPos.y);
            world_->UpdateRemotePlayers(network_.GetSnapshots(), network_.GetLocalId(), dt);
        } else {
            world_->UpdateRemotePlayers({}, 0, dt);
        }

        BeginDrawing();
        ClearBackground(BLACK);
        world_->Draw();
        EndDrawing();
    }

    network_.Disconnect();
    world_.reset();
    CloseWindow();
}
