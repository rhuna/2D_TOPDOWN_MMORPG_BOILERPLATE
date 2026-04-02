#pragma once

// Types of packets that can travel between client and server.
enum PacketType
{
    JOIN = 0, // Reserved for later if you want explicit join packets.
    MOVE = 1, // Client sends this when its player moves.
    STATE = 2 // Server sends this to tell clients where players are.
};

// Simple fixed-size packet.
// Keeping this tiny makes it easy to send/receive with asio::read/write.
struct Packet
{
    int type; // One of PacketType
    int id;   // Player id assigned by server
    float x;  // World x position
    float y;  // World y position
};