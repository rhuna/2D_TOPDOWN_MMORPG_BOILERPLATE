#include "net/NetworkClient.h"
#include <iostream>
using asio::ip::tcp;

NetworkClient::NetworkClient() : socket_(io_) {}
NetworkClient::~NetworkClient() { Disconnect(); }

bool NetworkClient::Connect(const char* host, const char* port) {
    try {
        tcp::resolver resolver(io_);
        auto endpoints = resolver.resolve(host, port);
        asio::connect(socket_, endpoints);
        connected_ = true;
        BeginRead();
        ioThread_ = std::thread([this]() { io_.run(); });
        std::cout << "Connected to " << host << ":" << port << "\n";
        return true;
    } catch (const std::exception& ex) {
        std::cerr << "Network connect failed: " << ex.what() << "\n";
        connected_ = false;
        return false;
    }
}

void NetworkClient::Disconnect() {
    if (!connected_) return;
    connected_ = false;
    try { socket_.close(); } catch (...) {}
    io_.stop();
    if (ioThread_.joinable()) ioThread_.join();
}

void NetworkClient::SendPosition(float x, float y) {
    if (!connected_) return;
    Packet packet{};
    packet.type = PacketType::Move;
    packet.x = x;
    packet.y = y;
    try {
        asio::write(socket_, asio::buffer(&packet, sizeof(Packet)));
    } catch (const std::exception& ex) {
        std::cerr << "SendPosition failed: " << ex.what() << "\n";
    }
}

std::unordered_map<int, RemoteSnapshot> NetworkClient::GetSnapshots() const {
    std::lock_guard<std::mutex> lock(snapshotMutex_);
    return snapshots_;
}

int NetworkClient::GetLocalId() const {
    std::lock_guard<std::mutex> lock(idMutex_);
    return localId_;
}

bool NetworkClient::IsConnected() const {
    return connected_;
}

void NetworkClient::BeginRead() {
    asio::async_read(socket_, asio::buffer(&readBuffer_, sizeof(Packet)),
        [this](std::error_code ec, std::size_t) {
            if (!ec) {
                if (readBuffer_.type == PacketType::Join) {
                    std::lock_guard<std::mutex> lock(idMutex_);
                    localId_ = readBuffer_.id;
                } else if (readBuffer_.type == PacketType::State) {
                    std::lock_guard<std::mutex> lock(snapshotMutex_);
                    snapshots_[readBuffer_.id] = RemoteSnapshot{readBuffer_.id, readBuffer_.x, readBuffer_.y};
                }
                BeginRead();
            } else {
                std::cerr << "Network read ended.\n";
            }
        });
}
