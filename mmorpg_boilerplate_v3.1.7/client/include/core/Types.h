#pragma once

#include <string>
#include <vector>
#include "raylib.h"

// -------------------- CORE --------------------
enum class Direction
{
    Up,
    Down,
    Left,
    Right
};

// -------------------- ITEMS --------------------
enum class ItemCategory
{
    Consumable,
    Weapon,
    Armor,
    Material,
    Quest
};

struct Weapon
{
    std::string name = "Fists";
    int damage = 1;
    float cooldown = 0.5f;
    float range = 30.0f;
    int price = 0;
    int attackBonus = 0;
    std::string description = "";
    
};

struct Armor
{
    std::string name = "Clothes";
    int defense = 0;
    int price = 0;
    int defenseBonus = 0;
    std::string description = "";
};

struct InventoryItem
{
    std::string name = "";
    int price = 0;
    int amount = 1;
    ItemCategory category = ItemCategory::Consumable;
    std::string description = "";
    int attackBonus = 0;
    int defenseBonus = 0;
    int healAmount = 0;
    bool stackable = true;
};

struct ShopItem
{
    std::string name;
    int price;
    int amount;
    ItemCategory category;
    std::string description;
    int attackBonus;
    int defenseBonus;
    int healAmount;
};

// -------------------- ACTOR --------------------
struct Actor
{
    Vector2 position{};
    Vector2 moveIntent{};
    Vector2 size{32, 32};

    float speed = 100.0f;
    float animClock = 0.0f;

    Direction facing = Direction::Down;
    Color color = WHITE;
};

// -------------------- PLAYER --------------------
struct Player : Actor
{
    int hp = 20;
    int maxHp = 20;
    int xp = 0;
    int gold = 0;

    Weapon weapon;
    Armor armor;

    std::vector<InventoryItem> inventory;
};

// -------------------- ENEMY --------------------
struct Enemy : Actor
{
    std::string name = "Enemy";

    int hp = 5;
    int maxHp = 5;
    bool alive = true;

    float attackTimer = 0.0f;
    float attackCooldown = 1.0f;
};

// -------------------- NPC --------------------
struct Npc : Actor
{
    std::string name;
    std::string idleText;
    std::string questText;
    std::string regionName = "World";

    int hp = 1;
    int maxHp = 1;

    bool isMerchant = false;
    std::vector<ShopItem> shopInventory;
};

// -------------------- NETWORK --------------------
struct RemoteActor : Actor
{
    int id = -1;
    std::string name;
    Vector2 targetPosition{};
    int hp = 0;
};

// -------------------- WORLD --------------------
struct Drop
{
    Vector2 position{};
    std::string itemName;
    int amount = 1;
    bool taken = false;
};

struct BuildingDoor
{
    Rectangle trigger{};    // world-space trigger near the door
    Vector2 insideSpawn{};  // where the player appears inside
    Vector2 outsideSpawn{}; // where the player appears when exiting
    std::string buildingId;
};
struct ExitDoor
{
    Rectangle trigger{}; // inside exit trigger
    Vector2 outsideSpawn{};
    std::string buildingId;
};