#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOGDI
#define NOGDI
#endif

#ifndef NOUSER
#define NOUSER
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#include <windows.h>
#define ASIO_STANDALONE
#include <asio.hpp>
#include "Protocol.h"
#include <mutex>
#include <thread>
#include <unordered_map>
#include <string>
#include <vector>
#include <atomic>

struct RemotePlayer
{
    int id{};
    float x{};
    float y{};
    int hp{20};
    std::string name;
};

class NetworkClient
{
public:
    NetworkClient();
    ~NetworkClient();

    bool Connect(const std::string &host, unsigned short port);
    void Disconnect();

    bool IsConnected() const;
    void SendMove(float x, float y);

    int GetLocalId() const;
    std::unordered_map<int, RemotePlayer> GetRemotePlayers() const;

    void SendAttack();
    void SendChat(const std::string &text);
    std::vector<std::string> GetChatMessages() const;

private:
    void ReadLoop();
    void HandleLine(const std::string &line);

private:
    asio::io_context ioContext_;
    asio::ip::tcp::socket socket_;
    std::thread readThread_;

    std::atomic<bool> connected_{false};
    int localId_ = -1;

    mutable std::mutex playersMutex_;
    std::unordered_map<int, RemotePlayer> remotePlayers_;

    mutable std::mutex chatMutex_;
    std::vector<std::string> chatMessages_;
};