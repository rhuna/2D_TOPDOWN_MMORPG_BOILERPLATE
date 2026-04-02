#include "Game.h"

#include "raylib.h"
#include <iostream>

Game::Game()
{
    playerPos_ = {400.0f, 300.0f};

    camera_.target = playerPos_;
    camera_.offset = {640.0f / 2.0f, 360.0f / 2.0f};
    camera_.rotation = 0.0f;
    camera_.zoom = 2.0f;
}

void Game::Init()
{
    // If your main.cpp already creates the window, remove this line
    // and keep window creation in main.cpp only.
    InitWindow(1280, 720, "Upgraded Overworld Multiplayer");

    SetTargetFPS(60);

    // Connect to the multiplayer server.
    network_.Connect();
}

void Game::Update(float dt)
{
    // ----- Local movement -----
    Vector2 move = {0.0f, 0.0f};

    if (IsKeyDown(KEY_W))
        move.y -= 1.0f;
    if (IsKeyDown(KEY_S))
        move.y += 1.0f;
    if (IsKeyDown(KEY_A))
        move.x -= 1.0f;
    if (IsKeyDown(KEY_D))
        move.x += 1.0f;

    // Normalize diagonal movement so it is not faster.
    float len = sqrtf(move.x * move.x + move.y * move.y);
    if (len > 0.0f)
    {
        move.x /= len;
        move.y /= len;
    }

    playerPos_.x += move.x * playerSpeed_ * dt;
    playerPos_.y += move.y * playerSpeed_ * dt;

    // ----- Camera follow -----
    camera_.target = playerPos_;

    // ----- Send our position to the server -----
    network_.SendPosition(playerPos_.x, playerPos_.y);

    // Keep your existing update logic here too:
    // - chunk streaming
    // - world interaction
    // - quests
    // - combat
    // - minimap state
    // - zoom controls
}

void Game::Render()
{
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode2D(camera_);

    // --------------------------------------------------------
    // KEEP YOUR EXISTING OVERWORLD RENDERING HERE
    // --------------------------------------------------------
    // Example placeholder ground:
    DrawRectangle(-5000, -5000, 10000, 10000, DARKGREEN);

    // Draw your local player.
    DrawCircleV(playerPos_, 10.0f, RED);

    // Draw remote players from network state.
    auto others = network_.GetRemotePlayers();
    for (const auto &[id, remote] : others)
    {
        // Right now server also sends us our own state back.
        // So you may see a blue square/blue player overlapping yourself.
        // That is normal for this prototype.
        DrawRectangle(
            static_cast<int>(remote.x - 10.0f),
            static_cast<int>(remote.y - 10.0f),
            20,
            20,
            BLUE);

        // Draw small id label above remote player.
        DrawText(
            TextFormat("P%d", id),
            static_cast<int>(remote.x - 12.0f),
            static_cast<int>(remote.y - 28.0f),
            12,
            WHITE);
    }

    EndMode2D();

    // Simple HUD text.
    DrawText("WASD to move", 20, 20, 20, WHITE);
    DrawText("Red = you, Blue = network players", 20, 48, 20, WHITE);

    EndDrawing();
}

void Game::Shutdown()
{
    network_.Disconnect();

    // If your main.cpp handles CloseWindow(), remove this line there or here
    // so it only happens once.
    CloseWindow();
}

void Game::Run()
{
    Init();

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        Update(dt);
        Render();
    }

    Shutdown();
}