#include "net/NetworkClient.h"
#include <asio.hpp>
#include <sstream>
#include <iostream>

using asio::ip::tcp;

NetworkClient::NetworkClient()
    : socket_(ioContext_)
{
}

NetworkClient::~NetworkClient()
{
    Disconnect();
}

bool NetworkClient::Connect(const std::string &host, unsigned short port)
{
    try
    {
        tcp::resolver resolver(ioContext_);
        auto endpoints = resolver.resolve(host, std::to_string(port));
        asio::connect(socket_, endpoints);

        connected_ = true;
        readThread_ = std::thread(&NetworkClient::ReadLoop, this);
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Connect failed: " << e.what() << "\n";
        return false;
    }
}

void NetworkClient::Disconnect()
{
    connected_ = false;

    if (socket_.is_open())
    {
        std::error_code ec;
        socket_.close(ec);
    }

    if (readThread_.joinable())
    {
        readThread_.join();
    }
}
bool NetworkClient::IsConnected() const
{
    return connected_;
}
void NetworkClient::SendMove(float x, float y)
{
    if (!connected_)
        return;

    std::ostringstream out;
    out << "MOVE " << x << " " << y << "\n";

    std::error_code ec;
    asio::write(socket_, asio::buffer(out.str()), ec);
}

void NetworkClient::SendAttack()
{
    if (!connected_)
        return;

    std::string msg = "ATTACK\n";
    std::error_code ec;
    asio::write(socket_, asio::buffer(msg), ec);
}

void NetworkClient::SendChat(const std::string &text)
{
    if (!connected_ || text.empty())
        return;

    std::string msg = "CHAT " + text + "\n";
    std::error_code ec;
    asio::write(socket_, asio::buffer(msg), ec);
}
int NetworkClient::GetLocalId() const
{
    return localId_;
}

std::unordered_map<int, RemotePlayer> NetworkClient::GetRemotePlayers() const
{
    std::lock_guard<std::mutex> lock(playersMutex_);
    return remotePlayers_;
}

std::vector<std::string> NetworkClient::GetChatMessages() const
{
    std::lock_guard<std::mutex> lock(chatMutex_);
    return chatMessages_;
}

void NetworkClient::ReadLoop()
{
    try
    {
        asio::streambuf buffer;

        while (connected_)
        {
            asio::read_until(socket_, buffer, '\n');

            std::istream input(&buffer);
            std::string line;
            std::getline(input, line);

            HandleLine(line);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Read loop ended: " << e.what() << "\n";
        connected_ = false;
    }
}


void NetworkClient::HandleLine(const std::string &line)
{
    if (line.rfind("ID ", 0) == 0)
    {
        localId_ = std::stoi(line.substr(3));
        std::cout << "Assigned player id: " << localId_ << "\n";
        return;
    }

    if (line.rfind("CHAT ", 0) == 0)
    {
        std::lock_guard<std::mutex> lock(chatMutex_);
        chatMessages_.push_back(line.substr(5));

        if (chatMessages_.size() > 8)
            chatMessages_.erase(chatMessages_.begin());

        return;
    }

    if (line.rfind("SNAPSHOT ", 0) == 0)
    {
        std::unordered_map<int, RemotePlayer> nextPlayers;

        std::string payload = line.substr(9);
        std::stringstream ss(payload);
        std::string token;

        while (std::getline(ss, token, ';'))
        {
            if (token.empty())
                continue;

            std::stringstream item(token);
            std::string part;

            std::vector<std::string> parts;
            while (std::getline(item, part, ','))
            {
                parts.push_back(part);
            }

            if (parts.size() >= 5)
            {
                RemotePlayer p;
                p.id = std::stoi(parts[0]);
                p.x = std::stof(parts[1]);
                p.y = std::stof(parts[2]);
                p.hp = std::stoi(parts[3]);
                p.name = parts[4];
                nextPlayers[p.id] = p;
            }
        }

        std::lock_guard<std::mutex> lock(playersMutex_);
        remotePlayers_ = std::move(nextPlayers);
    }
}





