
#pragma once

#define ASIO_STANDALONE
#include <asio.hpp>

#include <raylib.h>
#include <mutex>
#include <thread>
#include <unordered_map>

#include "Protocol.h"

using asio::ip::tcp;

// Lightweight network client used by the playable overworld client.
//
// Responsibilities:
// - connect to the prototype server
// - receive a server-assigned player id
// - send local player position updates
// - keep a thread-safe snapshot of other connected player states
class NetworkClient {
public:
    NetworkClient();
    ~NetworkClient();

    // Attempt to connect to a local server running on 127.0.0.1:54000.
    // Returns true on success.
    bool Connect();

    // Shut down the connection and background io thread.
    void Disconnect();

    // Send the client's current world position to the server.
    void SendPosition(float x, float y);

    // Thread-safe copy of the latest player positions known by the client.
    std::unordered_map<int, Vector2> GetRemotePlayers();

    // The server assigns this id shortly after connect.
    // Returns 0 until the Join packet arrives.
    int GetLocalPlayerId() const;

    // Returns whether the client is currently connected.
    bool IsConnected() const;

private:
    // Start the async read loop.
    void StartRead();

private:
    asio::io_context io_;
    tcp::socket socket_;
    std::thread ioThread_;

    Packet readBuffer_{};

    mutable std::mutex remotePlayersMutex_;
    std::unordered_map<int, Vector2> remotePlayers_;

    mutable std::mutex idMutex_;
    int localPlayerId_ = 0;

    bool connected_ = false;
};
