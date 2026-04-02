#pragma once

// Simple fixed-size network packet used by the optional prototype server.
//
// Notes:
// - This is intentionally tiny and easy to debug.
// - In a real MMO you would add sequence numbers, message sizes, validation,
//   entity snapshots, chat packets, inventory packets, and more.
enum class PacketType {
    Join  = 0, // Client connected or requested entry to the world.
    Move  = 1, // Client sent a position update.
    State = 2  // Server broadcasted player state to clients.
};

struct Packet {
    PacketType type = PacketType::Join;
    int id = 0;       // The server-assigned player id.
    float x = 0.0f;   // World-space x coordinate.
    float y = 0.0f;   // World-space y coordinate.
};
