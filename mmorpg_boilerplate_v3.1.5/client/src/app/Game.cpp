#include "app/Game.h"
#include "game/world/World.h"
#include <memory>
#include <raylib.h>
#include <fstream>
#include <string>
#include <cctype>

Game::Game() = default;
Game::~Game() = default;
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

    SetExitKey(KEY_NULL);

    world_ = std::make_unique<World>();

    const ClientConfig config = LoadClientConfig("client_config.txt");
    networkClient_.Connect(config.host, config.port);

    while (!WindowShouldClose())
    {
        const float dt = GetFrameTime();

        world_->Update(dt);

        // If the world has a blocking UI open (shop, inventory, quest log,
        // equipment, branching-choice dialog), do not let Enter also toggle chat.
        const bool blockingWorldUi = world_->IsBlockingUiOpen();

        // Toggle chat mode with Enter only when no blocking world UI is open.
        if (!blockingWorldUi && IsKeyPressed(KEY_ENTER))
        {
            if (!chatActive_)
            {
                chatActive_ = true;
            }
            else
            {
                if (!chatInput_.empty())
                {
                    networkClient_.SendChat(chatInput_);
                    chatInput_.clear();
                }
                chatActive_ = false;
            }
        }

        // Cancel chat input.
        if (chatActive_ && IsKeyPressed(KEY_ESCAPE))
        {
            chatInput_.clear();
            chatActive_ = false;
        }

        // Backspace deletes one character.
        if (chatActive_ && IsKeyPressed(KEY_BACKSPACE) && !chatInput_.empty())
        {
            chatInput_.pop_back();
        }

        // Collect typed characters while chat is active.
        if (chatActive_)
        {
            int key = GetCharPressed();
            while (key > 0)
            {
                // Printable ASCII range.
                if (key >= 32 && key <= 126 && chatInput_.size() < 80)
                {
                    chatInput_.push_back(static_cast<char>(key));
                }
                key = GetCharPressed();
            }
        }
        else
        {
            // Gameplay inputs only when not typing and not inside a blocking UI.
            if (!blockingWorldUi && IsKeyPressed(KEY_F))
            {
                networkClient_.SendAttack();
            }
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

        // Chat history.
        auto chat = networkClient_.GetChatMessages();
        int y = 20;
        for (const auto &msg : chat)
        {
            DrawText(msg.c_str(), 20, y, 18, WHITE);
            y += 22;
        }

        // Chat input box.
        if (chatActive_)
        {
            const int boxX = 16;
            const int boxY = screenHeight - 165;
            const int boxW = screenWidth - 32;
            const int boxH = 34;

            DrawRectangle(boxX, boxY, boxW, boxH, Fade(BLACK, 0.80f));
            DrawRectangleLines(boxX, boxY, boxW, boxH, SKYBLUE);

            std::string prompt = "Chat: " + chatInput_ + "_";
            DrawText(prompt.c_str(), boxX + 10, boxY + 8, 20, WHITE);

            DrawText("Enter = send, Esc = cancel",
                     boxX,
                     boxY - 22,
                     18,
                     LIGHTGRAY);
        }
        else
        {
            DrawText("Press Enter to chat",
                     16,
                     screenHeight - 165,
                     18,
                     LIGHTGRAY);
        }

        EndDrawing();
    }

    networkClient_.Disconnect();
    world_.reset();
    CloseWindow();
}