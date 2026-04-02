#include <asio.hpp>
#include <unordered_map>
#include <vector>
#include <thread>
#include <mutex>
#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>

using asio::ip::tcp;

struct PlayerState
{
    int id{};
    float x{};
    float y{};
    int hp{20};
    std::string name;
};

std::mutex g_mutex;
std::unordered_map<int, PlayerState> g_players;
std::vector<std::shared_ptr<tcp::socket>> g_clients;
int g_nextId = 1;

std::string BuildSnapshot()
{
    std::lock_guard<std::mutex> lock(g_mutex);

    std::ostringstream out;
    out << "SNAPSHOT ";
    for (const auto& [id, p] : g_players)
    {
        out << p.id << "," << p.x << "," << p.y
            << "," << p.hp << "," << p.name << ";";
    }
    out << "\n";
    return out.str();
}

void BroadcastSnapshot()
{
    const std::string msg = BuildSnapshot();

    std::lock_guard<std::mutex> lock(g_mutex);
    for (auto& client : g_clients)
    {
        if (client && client->is_open())
        {
            std::error_code ec;
            asio::write(*client, asio::buffer(msg), ec);
        }
    }
}

void HandleClient(std::shared_ptr<tcp::socket> socket)
{
    int myId = 0;

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        myId = g_nextId++;
        g_players[myId] = PlayerState{
            myId, 200.0f, 200.0f, 20,
            "P" + std::to_string(myId)
        };
        g_clients.push_back(socket);
    }

    {
        std::string welcome = "ID " + std::to_string(myId) + "\n";
        asio::write(*socket, asio::buffer(welcome));
    }

    BroadcastSnapshot();

    try
    {
        asio::streambuf buffer;

        while (true)
        {
            std::size_t bytes = asio::read_until(*socket, buffer, '\n');
            (void)bytes;

            std::istream input(&buffer);
            std::string line;
            std::getline(input, line);

            // Expected: MOVE x y
            if (line.rfind("MOVE ", 0) == 0)
            {
                std::istringstream iss(line.substr(5));
                float x = 0.0f;
                float y = 0.0f;
                iss >> x >> y;

                {
                    std::lock_guard<std::mutex> lock(g_mutex);
                    auto it = g_players.find(myId);
                    if (it != g_players.end())
                    {
                        it->second.x = x;
                        it->second.y = y;
                    }
                }

                BroadcastSnapshot();
            }
            else if (line == "ATTACK")
            {
                std::lock_guard<std::mutex> lock(g_mutex);

                auto attackerIt = g_players.find(myId);
                if (attackerIt != g_players.end())
                {
                    PlayerState &attacker = attackerIt->second;

                    for (auto &[otherId, target] : g_players)
                    {
                        if (otherId == myId)
                            continue;

                        float dx = target.x - attacker.x;
                        float dy = target.y - attacker.y;
                        float distance = std::sqrt(dx * dx + dy * dy);

                        if (distance <= 50.0f)
                        {
                            target.hp = std::max(0, target.hp - 1);
                            break;
                        }
                    }
                }

                BroadcastSnapshot();
            }
            else if (line.rfind("CHAT ", 0) == 0)
            {
                std::string chatText = line.substr(5);

                std::string sender;
                {
                    std::lock_guard<std::mutex> lock(g_mutex);
                    sender = g_players[myId].name;
                }

                std::string msg = "CHAT " + sender + ": " + chatText + "\n";

                std::lock_guard<std::mutex> lock(g_mutex);
                for (auto &client : g_clients)
                {
                    if (client && client->is_open())
                    {
                        std::error_code ec;
                        asio::write(*client, asio::buffer(msg), ec);
                    }
                }
            
                BroadcastSnapshot();
            }
        }
    }
    catch (const std::exception&)
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_players.erase(myId);

        g_clients.erase(
            std::remove_if(
                g_clients.begin(),
                g_clients.end(),
                [&](const std::shared_ptr<tcp::socket>& s)
                {
                    return s.get() == socket.get();
                }),
            g_clients.end());
    }

    BroadcastSnapshot();
}

int main()
{
    try
    {
        asio::io_context io;
        tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 7777));

        std::cout << "Server running on port 7777...\n";

        while (true)
        {
            auto socket = std::make_shared<tcp::socket>(io);
            acceptor.accept(*socket);

            std::cout << "Client connected\n";
            std::thread(HandleClient, socket).detach();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Server error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}