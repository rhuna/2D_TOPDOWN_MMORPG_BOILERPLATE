#include "Game.h"
#include <raylib.h>
#include <memory>

void Game::Run() {
    const int screenWidth = 1024;
    const int screenHeight = 768;

    InitWindow(screenWidth, screenHeight, "TopDownMMOStarter - Tileset Build");
    SetTargetFPS(60);

    world_ = std::make_unique<World>();

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        world_->Update(dt);

        BeginDrawing();
        ClearBackground(BLACK);
        world_->Draw();
        EndDrawing();
    }

    world_.reset();
    CloseWindow();
}
