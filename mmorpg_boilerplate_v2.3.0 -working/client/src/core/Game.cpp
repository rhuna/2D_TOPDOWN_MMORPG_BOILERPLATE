#include "core/Game.h"

#include <memory>
#include <raylib.h>

// Run the main game loop.
//
// Flow:
// 1. Create the window.
// 2. Create the world.
// 3. Repeatedly update and draw until the user closes the window.
// 4. Release resources and shut down cleanly.
void Game::Run() {
    const int screenWidth = 1024;
    const int screenHeight = 768;

    InitWindow(screenWidth, screenHeight, "TopDownMMOStarter - Clean Commented Build");
    SetTargetFPS(60);

    // Create the world after the window exists, so any texture loads happen
    // only after raylib is fully initialized.
    world_ = std::make_unique<World>();

    while (!WindowShouldClose()) {
        const float dt = GetFrameTime();

        // Update all simulation/gameplay state first.
        world_->Update(dt);

        // Then render the frame.
        BeginDrawing();
        ClearBackground(BLACK);
        world_->Draw();
        EndDrawing();
    }

    // Explicit reset before CloseWindow keeps destruction order obvious.
    world_.reset();
    CloseWindow();
}
