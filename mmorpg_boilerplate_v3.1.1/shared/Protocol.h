#pragma once

// Simple fixed-size packet used by the prototype network layer.
enum class PacketType {
    Join  = 0, // Server tells the client its assigned player id.
    Move  = 1, // Client sends a position update.
    State = 2  // Server broadcasts player state to all clients.
};

struct Packet {
    PacketType type = PacketType::Join;
    int id = 0;
    float x = 0.0f;
    float y = 0.0f;
};
