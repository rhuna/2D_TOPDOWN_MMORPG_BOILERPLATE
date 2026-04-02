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

    // Try to connect to the local prototype server.
    // The game remains fully playable even when this connection fails.
    network_.Connect();

    while (!WindowShouldClose()) {
        const float dt = GetFrameTime();

        // Update all simulation/gameplay state first.
        world_->Update(dt);

        // Send the local player's latest world position to the server.
        if (network_.IsConnected()) {
            Vector2 playerPosition = world_->GetPlayerPosition();
            network_.SendPosition(playerPosition.x, playerPosition.y);

            // Pull the latest remote-player snapshot and hand it to the world
            // so the world renderer can draw those players.
            world_->SetRemotePlayers(network_.GetRemotePlayers(), network_.GetLocalPlayerId());
        }

        // Then render the frame.
        BeginDrawing();
        ClearBackground(BLACK);
        world_->Draw();
        EndDrawing();
    }

    // Shut down networking before textures/window are destroyed.
    network_.Disconnect();

    // Explicit reset before CloseWindow keeps destruction order obvious.
    world_.reset();
    CloseWindow();
}
