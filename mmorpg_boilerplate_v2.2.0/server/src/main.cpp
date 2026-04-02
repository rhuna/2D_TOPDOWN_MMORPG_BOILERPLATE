#define ASIO_STANDALONE
#include <asio.hpp>

#include <functional>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#include "../../shared/Protocol.h"

using asio::ip::tcp;

// Represents one player on the server.
struct Player
{
    int id = 0;
    float x = 100.0f;
    float y = 100.0f;
};

class Session;

// Global containers for this simple prototype.
// In a larger project, you would wrap these in a Server class.
std::unordered_map<int, Player> g_players;
std::vector<std::shared_ptr<Session>> g_sessions;

// One client connection.
class Session : public std::enable_shared_from_this<Session>
{
public:
    explicit Session(tcp::socket socket)
        : socket_(std::move(socket))
    {
    }

    // Start reading packets from this client.
    void Start()
    {
        Read();
    }

    // Send one packet to this client.
    void Send(const Packet &p)
    {
        // Note:
        // This is okay for a simple prototype, but in a production server
        // you would use a send queue so packet memory stays alive safely.
        asio::async_write(
            socket_,
            asio::buffer(&p, sizeof(Packet)),
            [](std::error_code /*ec*/, std::size_t /*bytes*/)
            {
                // For now we ignore write errors in this simple prototype.
            });
    }

    int id = 0;

private:
    tcp::socket socket_;
    Packet buffer_{};

    // Read one fixed-size Packet from the socket.
    void Read()
    {
        auto self = shared_from_this();

        asio::async_read(
            socket_,
            asio::buffer(&buffer_, sizeof(Packet)),
            [this, self](std::error_code ec, std::size_t /*bytes*/)
            {
                if (!ec)
                {
                    // Client is telling us its new position.
                    if (buffer_.type == MOVE)
                    {
                        g_players[id].x = buffer_.x;
                        g_players[id].y = buffer_.y;

                        // After receiving a movement update, broadcast
                        // all known player states to every connected client.
                        BroadcastAllStates();
                    }

                    // Keep reading more packets from this client.
                    Read();
                }
                else
                {
                    // Client disconnected or socket failed.
                    std::cout << "Player disconnected ID: " << id << "\n";

                    // Remove player from state map.
                    g_players.erase(id);

                    // Remove this session from the session list.
                    for (auto it = g_sessions.begin(); it != g_sessions.end(); ++it)
                    {
                        if ((*it).get() == this)
                        {
                            g_sessions.erase(it);
                            break;
                        }
                    }

                    // Tell everyone else about the updated world state.
                    BroadcastAllStates();
                }
            });
    }

    // Send every known player state to every session.
    void BroadcastAllStates()
    {
        for (const auto &[playerId, player] : g_players)
        {
            Packet statePacket{};
            statePacket.type = STATE;
            statePacket.id = playerId;
            statePacket.x = player.x;
            statePacket.y = player.y;

            for (auto &session : g_sessions)
            {
                session->Send(statePacket);
            }
        }
    }
};

int main()
{
    try
    {
        asio::io_context io;

        // Listen on port 54000 on all local interfaces.
        tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 54000));

        int nextId = 1;

        std::cout << "Server running on port 54000\n";

        // Async accept loop.
        std::function<void()> acceptLoop;
        acceptLoop = [&]()
        {
            acceptor.async_accept(
                [&](std::error_code ec, tcp::socket socket)
                {
                    if (!ec)
                    {
                        auto session = std::make_shared<Session>(std::move(socket));
                        session->id = nextId++;

                        // Add a default player entry for this new connection.
                        g_players[session->id] = Player{
                            session->id,
                            100.0f + 20.0f * static_cast<float>(session->id),
                            100.0f};

                        g_sessions.push_back(session);
                        session->Start();

                        std::cout << "Player connected ID: " << session->id << "\n";
                    }

                    // Continue accepting more clients forever.
                    acceptLoop();
                });
        };

        acceptLoop();
        io.run();
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Server exception: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}