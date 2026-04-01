#include "Game.h"
#include <raylib.h>

void Game::Run() {
    const int screenWidth = 1024;
    const int screenHeight = 768;

    InitWindow(screenWidth, screenHeight, "TopDownMMOStarter");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        world_.Update(dt);

        BeginDrawing();
        ClearBackground(BLACK);
        world_.Draw();
        EndDrawing();
    }

    CloseWindow();
}
