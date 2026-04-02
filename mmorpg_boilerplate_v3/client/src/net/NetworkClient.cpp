
#include "net/NetworkClient.h"

#include <iostream>

NetworkClient::NetworkClient()
    : socket_(io_) {
}

NetworkClient::~NetworkClient() {
    Disconnect();
}

bool NetworkClient::Connect() {
    try {
        tcp::resolver resolver(io_);
        auto endpoints = resolver.resolve("127.0.0.1", "54000");

        asio::connect(socket_, endpoints);
        connected_ = true;

        // Start receiving packets before the io thread begins.
        StartRead();

        ioThread_ = std::thread([this]() {
            io_.run();
        });

        std::cout << "Connected to prototype MMO server.\n";
        return true;
    } catch (const std::exception& ex) {
        std::cerr << "Connect failed: " << ex.what() << "\n";
        connected_ = false;
        return false;
    }
}

void NetworkClient::Disconnect() {
    if (!connected_) {
        return;
    }

    connected_ = false;

    try {
        socket_.close();
    } catch (...) {
        // Prototype code intentionally ignores socket shutdown errors.
    }

    io_.stop();

    if (ioThread_.joinable()) {
        ioThread_.join();
    }
}

void NetworkClient::SendPosition(float x, float y) {
    if (!connected_) {
        return;
    }

    Packet packet{};
    packet.type = PacketType::Move;
    packet.id = GetLocalPlayerId();
    packet.x = x;
    packet.y = y;

    try {
        asio::write(socket_, asio::buffer(&packet, sizeof(Packet)));
    } catch (const std::exception& ex) {
        std::cerr << "SendPosition failed: " << ex.what() << "\n";
    }
}

std::unordered_map<int, Vector2> NetworkClient::GetRemotePlayers() {
    std::lock_guard<std::mutex> lock(remotePlayersMutex_);
    return remotePlayers_;
}

int NetworkClient::GetLocalPlayerId() const {
    std::lock_guard<std::mutex> lock(idMutex_);
    return localPlayerId_;
}

bool NetworkClient::IsConnected() const {
    return connected_;
}

void NetworkClient::StartRead() {
    asio::async_read(
        socket_,
        asio::buffer(&readBuffer_, sizeof(Packet)),
        [this](std::error_code ec, std::size_t /*bytes*/) {
            if (!ec) {
                if (readBuffer_.type == PacketType::Join) {
                    std::lock_guard<std::mutex> lock(idMutex_);
                    localPlayerId_ = readBuffer_.id;
                } else if (readBuffer_.type == PacketType::State) {
                    std::lock_guard<std::mutex> lock(remotePlayersMutex_);
                    remotePlayers_[readBuffer_.id] = Vector2{readBuffer_.x, readBuffer_.y};
                }

                StartRead();
            } else if (connected_) {
                std::cerr << "Server read failed or disconnected.\n";
            }
        });
}
