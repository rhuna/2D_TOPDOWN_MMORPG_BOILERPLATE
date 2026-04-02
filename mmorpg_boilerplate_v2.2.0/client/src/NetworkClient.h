#pragma once

#define ASIO_STANDALONE
#include <asio.hpp>

#include <mutex>
#include <thread>
#include <unordered_map>

#include "../../shared/Protocol.h"

using asio::ip::tcp;

// Data we store for another player in the world.
struct RemotePlayer
{
    float x = 0.0f;
    float y = 0.0f;
};

// Handles connecting to the game server, sending our position,
// and receiving positions for other players.
class NetworkClient
{
public:
    NetworkClient();
    ~NetworkClient();

    // Connect to server at 127.0.0.1:54000
    bool Connect();

    // Disconnect cleanly.
    void Disconnect();

    // Send our current position to the server.
    void SendPosition(float x, float y);

    // Snapshot of other players, safe to copy from render/update thread.
    std::unordered_map<int, RemotePlayer> GetRemotePlayers();

private:
    // Begin async read loop.
    void StartRead();

    asio::io_context io_;
    tcp::socket socket_;
    std::thread ioThread_;

    Packet readBuffer_{};

    // Protect shared remote-player state because reads happen
    // on the asio thread while rendering happens on main thread.
    std::mutex remotePlayersMutex_;
    std::unordered_map<int, RemotePlayer> remotePlayers_;

    bool connected_ = false;
};