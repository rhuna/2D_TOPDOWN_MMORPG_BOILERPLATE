#define ASIO_STANDALONE
#include <asio.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
#include "Protocol.h"
using asio::ip::tcp;

struct PlayerState {
    int id = 0;
    float x = 100.0f;
    float y = 100.0f;
};

class Session;
std::unordered_map<int, PlayerState> g_players;
std::vector<std::shared_ptr<Session>> g_sessions;

class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(tcp::socket socket) : socket_(std::move(socket)) {}
    void Start() {
        SendWelcome();
        BroadcastAllStates();
        Read();
    }
    void Send(const Packet& packet) {
        asio::async_write(socket_, asio::buffer(&packet, sizeof(Packet)), [](std::error_code, std::size_t) {});
    }
    int id = 0;
private:
    void SendWelcome() {
        Packet welcome{};
        welcome.type = PacketType::Join;
        welcome.id = id;
        welcome.x = g_players[id].x;
        welcome.y = g_players[id].y;
        Send(welcome);
    }
    void Read() {
        auto self = shared_from_this();
        asio::async_read(socket_, asio::buffer(&buffer_, sizeof(Packet)), [this, self](std::error_code ec, std::size_t) {
            if (!ec) {
                if (buffer_.type == PacketType::Move) {
                    g_players[id].x = buffer_.x;
                    g_players[id].y = buffer_.y;
                    BroadcastAllStates();
                }
                Read();
            } else {
                std::cout << "Client disconnected: " << id << "\n";
                g_players.erase(id);
            }
        });
    }
    void BroadcastAllStates() {
        for (const auto& [playerId, player] : g_players) {
            Packet packet{};
            packet.type = PacketType::State;
            packet.id = playerId;
            packet.x = player.x;
            packet.y = player.y;
            for (auto& session : g_sessions) {
                session->Send(packet);
            }
        }
    }
    tcp::socket socket_;
    Packet buffer_{};
};

int main() {
    try {
        asio::io_context io;
        tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 54000));
        int nextId = 1;
        std::cout << "Prototype server listening on port 54000\n";
        std::function<void()> acceptLoop;
        acceptLoop = [&]() {
            acceptor.async_accept([&](std::error_code ec, tcp::socket socket) {
                if (!ec) {
                    auto session = std::make_shared<Session>(std::move(socket));
                    session->id = nextId++;
                    g_players[session->id] = PlayerState{session->id, 100.0f + 20.0f * static_cast<float>(session->id), 100.0f};
                    g_sessions.push_back(session);
                    session->Start();
                    std::cout << "Client connected with id " << session->id << "\n";
                }
                acceptLoop();
            });
        };
        acceptLoop();
        io.run();
    } catch (const std::exception& ex) {
        std::cerr << "Server error: " << ex.what() << "\n";
        return 1;
    }
    return 0;
}
