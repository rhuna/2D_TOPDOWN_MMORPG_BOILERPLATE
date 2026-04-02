#pragma once

#include <raylib.h>
#include <string>
#include <vector>

// Cardinal facing directions used by actors and animation selection.
enum class Direction {
    Down = 0,
    Left = 1,
    Right = 2,
    Up = 3
};

// Basic weapon data for the player.
struct Weapon {
    std::string name;
    int damage = 1;
    float range = 40.0f;
    float cooldown = 0.35f;
};

// Very simple inventory stack entry.
struct InventoryItem {
    std::string name;
    int count = 0;
};

// One simple kill quest.
struct Quest {
    std::string title;
    std::string description;
    int killsNeeded = 0;
    int killsDone = 0;
    bool accepted = false;
    bool completed = false;
    bool rewarded = false;
};

// Base actor shared by player, enemies, and NPCs.
struct Actor {
    Vector2 position{};             // Top-left world position in pixels
    Vector2 size{28.0f, 28.0f};     // Draw/collision size
    float speed = 140.0f;           // Movement speed in pixels per second
    int hp = 10;
    int maxHp = 10;
    Color color = WHITE;
    Direction facing = Direction::Down;
    Vector2 moveIntent{};           // Current desired movement vector
    float animClock = 0.0f;         // Time accumulator used for animation frames
};

// The controllable player.
struct Player : Actor {
    int xp = 0;
    int gold = 0;
    Weapon weapon{"Rusty Sword", 2, 42.0f, 0.35f};
    std::vector<InventoryItem> inventory{{"Herb", 0}, {"Potion", 1}};
};

// Enemy type used in the prototype.
struct Enemy : Actor {
    std::string name = "Slime";
    bool alive = true;
    float attackCooldown = 0.9f;
    float attackTimer = 0.0f;
};

// Non-player character.
struct Npc : Actor {
    std::string name;
    std::string idleText;
    std::string questText;
    std::string regionName = "Roadside";
};

// World item drop.
struct Drop {
    Vector2 position{};
    std::string itemName;
    int amount = 1;
    bool taken = false;
};


// Client-side visual representation of a remote player.
struct RemoteActor : Actor {
    int id = 0;
    Vector2 targetPosition{};
};
