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

// Latest server snapshot for one remote player.
struct RemoteSnapshot {
    int id = 0;
    float x = 0.0f;
    float y = 0.0f;
};

class NetworkClient {
public:
    NetworkClient();
    ~NetworkClient();

    bool Connect(const char* host = "127.0.0.1", const char* port = "54000");
    void Disconnect();
    void SendPosition(float x, float y);
    std::unordered_map<int, RemoteSnapshot> GetSnapshots() const;
    int GetLocalId() const;
    bool IsConnected() const;

private:
    void BeginRead();

    asio::io_context io_;
    asio::ip::tcp::socket socket_;
    std::thread ioThread_;
    Packet readBuffer_{};

    mutable std::mutex snapshotMutex_;
    std::unordered_map<int, RemoteSnapshot> snapshots_;

    mutable std::mutex idMutex_;
    int localId_ = 0;
    bool connected_ = false;
};
