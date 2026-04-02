#include "NetworkClient.h"

#include <iostream>

NetworkClient::NetworkClient()
    : socket_(io_)
{
}

NetworkClient::~NetworkClient()
{
    Disconnect();
}

bool NetworkClient::Connect()
{
    try
    {
        // Resolve local server.
        tcp::resolver resolver(io_);
        auto endpoints = resolver.resolve("127.0.0.1", "54000");

        // Connect socket.
        asio::connect(socket_, endpoints);
        connected_ = true;

        // Start async read loop before the io thread runs.
        StartRead();

        // Run asio on its own background thread.
        ioThread_ = std::thread([this]()
                                { io_.run(); });

        std::cout << "Connected to server.\n";
        return true;
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Connect failed: " << ex.what() << "\n";
        connected_ = false;
        return false;
    }
}

void NetworkClient::Disconnect()
{
    if (!connected_)
    {
        return;
    }

    connected_ = false;

    try
    {
        socket_.close();
    }
    catch (...)
    {
        // Ignore shutdown errors in prototype code.
    }

    io_.stop();

    if (ioThread_.joinable())
    {
        ioThread_.join();
    }
}

void NetworkClient::SendPosition(float x, float y)
{
    if (!connected_)
    {
        return;
    }

    Packet packet{};
    packet.type = MOVE;
    packet.id = 0; // Server assigns real ids; client does not need one yet.
    packet.x = x;
    packet.y = y;

    try
    {
        // Synchronous write is fine for this simple prototype.
        asio::write(socket_, asio::buffer(&packet, sizeof(Packet)));
    }
    catch (const std::exception &ex)
    {
        std::cerr << "SendPosition failed: " << ex.what() << "\n";
    }
}

std::unordered_map<int, RemotePlayer> NetworkClient::GetRemotePlayers()
{
    std::lock_guard<std::mutex> lock(remotePlayersMutex_);
    return remotePlayers_;
}

void NetworkClient::StartRead()
{
    asio::async_read(
        socket_,
        asio::buffer(&readBuffer_, sizeof(Packet)),
        [this](std::error_code ec, std::size_t /*bytes*/)
        {
            if (!ec)
            {
                if (readBuffer_.type == STATE)
                {
                    std::lock_guard<std::mutex> lock(remotePlayersMutex_);
                    remotePlayers_[readBuffer_.id] = RemotePlayer{
                        readBuffer_.x,
                        readBuffer_.y};
                }

                // Continue reading forever.
                StartRead();
            }
            else
            {
                std::cerr << "Read failed or disconnected.\n";
            }
        });
}