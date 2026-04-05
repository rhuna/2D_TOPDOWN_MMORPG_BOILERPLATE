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
#include <chrono>
#include <memory>

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
bool g_running = true;

std::string BuildSnapshot()
{
    std::ostringstream out;
    out << "SNAPSHOT ";

    std::lock_guard<std::mutex> lock(g_mutex);
    for (const auto &[id, p] : g_players)
    {
        out << p.id << "," << p.x << "," << p.y << "," << p.hp << "," << p.name << ";";
    }
    out << "\n";
    return out.str();
}

void RemoveClient(const std::shared_ptr<tcp::socket> &socket, int myId)
{
    std::lock_guard<std::mutex> lock(g_mutex);

    g_players.erase(myId);

    g_clients.erase(
        std::remove_if(
            g_clients.begin(),
            g_clients.end(),
            [&](const std::shared_ptr<tcp::socket> &s)
            {
                return !s || s.get() == socket.get();
            }),
        g_clients.end());
}

void BroadcastSnapshot()
{
    const std::string msg = BuildSnapshot();

    std::lock_guard<std::mutex> lock(g_mutex);

    for (auto it = g_clients.begin(); it != g_clients.end();)
    {
        auto &client = *it;
        if (!client || !client->is_open())
        {
            it = g_clients.erase(it);
            continue;
        }

        std::error_code ec;
        asio::write(*client, asio::buffer(msg), ec);

        if (ec)
        {
            std::cout << "Broadcast write failed: " << ec.message() << "\n";
            client->close(ec);
            it = g_clients.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void BroadcastChat(const std::string &sender, const std::string &chatText)
{
    const std::string msg = "CHAT " + sender + ": " + chatText + "\n";

    std::lock_guard<std::mutex> lock(g_mutex);

    for (auto it = g_clients.begin(); it != g_clients.end();)
    {
        auto &client = *it;
        if (!client || !client->is_open())
        {
            it = g_clients.erase(it);
            continue;
        }

        std::error_code ec;
        asio::write(*client, asio::buffer(msg), ec);

        if (ec)
        {
            std::cout << "Chat write failed: " << ec.message() << "\n";
            client->close(ec);
            it = g_clients.erase(it);
        }
        else
        {
            ++it;
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
            myId, 200.0f, 200.0f, 20, "P" + std::to_string(myId)};
        g_clients.push_back(socket);
    }

    try
    {
        std::string welcome = "ID " + std::to_string(myId) + "\n";
        asio::write(*socket, asio::buffer(welcome));

        std::cout << "Client connected: P" << myId << "\n";

        asio::streambuf buffer;

        while (g_running)
        {
            std::size_t bytes = asio::read_until(*socket, buffer, '\n');
            (void)bytes;

            std::istream input(&buffer);
            std::string line;
            std::getline(input, line);

            if (line.rfind("MOVE ", 0) == 0)
            {
                std::istringstream iss(line.substr(5));
                float x = 0.0f;
                float y = 0.0f;
                iss >> x >> y;

                std::lock_guard<std::mutex> lock(g_mutex);
                auto it = g_players.find(myId);
                if (it != g_players.end())
                {
                    it->second.x = x;
                    it->second.y = y;
                }
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
            }
            else if (line.rfind("CHAT ", 0) == 0)
            {
                std::string chatText = line.substr(5);
                std::string sender = "P" + std::to_string(myId);

                {
                    std::lock_guard<std::mutex> lock(g_mutex);
                    auto it = g_players.find(myId);
                    if (it != g_players.end())
                    {
                        sender = it->second.name;
                    }
                }

                BroadcastChat(sender, chatText);
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "Client disconnected: P" << myId << " (" << e.what() << ")\n";
    }

    std::error_code ec;
    socket->close(ec);
    RemoveClient(socket, myId);
}

int main()
{
    try
    {
        asio::io_context io;
        tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 7777));

        std::cout << "Server running on port 7777...\n";

        std::thread tickThread([]()
                               {
            while (g_running)
            {
                BroadcastSnapshot();
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            } });

        while (true)
        {
            auto socket = std::make_shared<tcp::socket>(io);
            acceptor.accept(*socket);
            std::thread(HandleClient, socket).detach();
        }

        tickThread.join();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Server error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}