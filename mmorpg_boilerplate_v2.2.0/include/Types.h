#pragma once

#include <raylib.h>
#include <string>
#include <vector>

enum class Direction {
    Down = 0,
    Left = 1,
    Right = 2,
    Up = 3
};

struct Weapon {
    std::string name;
    int damage = 1;
    float range = 40.0f;
    float cooldown = 0.35f;
};

struct InventoryItem {
    std::string name;
    int count = 0;
};

struct Quest {
    std::string title;
    std::string description;
    int killsNeeded = 0;
    int killsDone = 0;
    bool accepted = false;
    bool completed = false;
    bool rewarded = false;
};

struct Actor {
    Vector2 position{};
    Vector2 size{28.0f, 28.0f};
    float speed = 140.0f;
    int hp = 10;
    int maxHp = 10;
    Color color = WHITE;
    Direction facing = Direction::Down;
    Vector2 moveIntent{};
    float animClock = 0.0f;
};

struct Player : Actor {
    int xp = 0;
    int gold = 0;
    Weapon weapon{"Rusty Sword", 2, 42.0f, 0.35f};
    std::vector<InventoryItem> inventory{{"Herb", 0}, {"Potion", 1}};
};

struct Enemy : Actor {
    std::string name = "Slime";
    bool alive = true;
    float attackCooldown = 0.9f;
    float attackTimer = 0.0f;
};

struct Npc : Actor {
    std::string name;
    std::string idleText;
    std::string questText;
    std::string regionName = "Roadside";
};

struct Drop {
    Vector2 position{};
    std::string itemName;
    int amount = 1;
    bool taken = false;
};
