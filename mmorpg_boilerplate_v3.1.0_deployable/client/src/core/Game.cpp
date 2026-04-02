#include "core/Game.h"
#include "world/World.h"
#include "net/NetworkClient.h"
#include <memory>
#include <raylib.h>
#include <fstream>
#include <string>

namespace
{
    struct ClientConfig
    {
        std::string host = "127.0.0.1";
        unsigned short port = 7777;
    };

    ClientConfig LoadClientConfig(const std::string &path)
    {
        ClientConfig config;

        std::ifstream file(path);
        if (!file.is_open())
        {
            return config;
        }

        std::string line;
        while (std::getline(file, line))
        {
            const std::size_t eq = line.find('=');
            if (eq == std::string::npos)
                continue;

            const std::string key = line.substr(0, eq);
            const std::string value = line.substr(eq + 1);

            if (key == "host" && !value.empty())
            {
                config.host = value;
            }
            else if (key == "port" && !value.empty())
            {
                try
                {
                    int parsed = std::stoi(value);
                    if (parsed > 0 && parsed <= 65535)
                    {
                        config.port = static_cast<unsigned short>(parsed);
                    }
                }
                catch (...)
                {
                    // Keep default port if parsing fails.
                }
            }
        }

        return config;
    }
}

void Game::Run()
{
    const int screenWidth = 1024;
    const int screenHeight = 768;

    InitWindow(screenWidth, screenHeight, "TopDownMMOStarter - Networked Sprite Build");
    SetTargetFPS(60);

    world_ = std::make_unique<World>();

    const ClientConfig config = LoadClientConfig("client_config.txt");
    networkClient_.Connect(config.host, config.port);

    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();

        world_->Update(dt);

        if (IsKeyPressed(KEY_F))
        {
            networkClient_.SendAttack();
        }

        if (IsKeyPressed(KEY_ENTER))
        {
            networkClient_.SendChat("Hello world!");
        }

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

        auto chat = networkClient_.GetChatMessages();
        int y = 20;
        for (const auto &msg : chat)
        {
            DrawText(msg.c_str(), 20, y, 18, WHITE);
            y += 22;
        }

        EndDrawing();
    }

    networkClient_.Disconnect();
    world_.reset();
    CloseWindow();
}