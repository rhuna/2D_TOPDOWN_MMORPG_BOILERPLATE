#pragma once

#include "Types.h"
#include <string>
#include <vector>

class World {
public:
    World();
    ~World();

    void Update(float dt);
    void Draw() const;

private:
    void LoadAssets();
    void UpdatePlayer(float dt);
    void UpdateEnemies(float dt);
    void HandleCombat(float dt);
    void HandleInteraction();
    void HandleDrops();
    void DrawMap() const;
    void DrawHud() const;
    void DrawActorSprite(const Actor& actor, Rectangle source, Color tint = WHITE) const;
    void DrawPickupSprite(Vector2 position, Rectangle source, Color tint = WHITE) const;
    void TryPickup(const std::string& itemName, int amount);
    bool IsWall(Vector2 position) const;
    bool IsClose(Vector2 a, Vector2 b, float distance) const;
    Rectangle RectOf(const Actor& actor) const;

    std::vector<std::string> map_;
    int tileSize_ = 32;
    Player player_;
    std::vector<Enemy> enemies_;
    std::vector<Npc> npcs_;
    std::vector<Drop> drops_;
    Quest slimeQuest_;
    std::string message_;
    float playerAttackTimer_ = 0.0f;

    Texture2D tileset_{};
    bool tilesetLoaded_ = false;
    Rectangle grassTile_{288.0f, 80.0f, 16.0f, 16.0f};
    Rectangle wallTile_{208.0f, 384.0f, 16.0f, 16.0f};
    Rectangle playerSprite_{384.0f, 1744.0f, 16.0f, 16.0f};
    Rectangle npcSprite_{400.0f, 1744.0f, 16.0f, 16.0f};
    Rectangle slimeSprite_{208.0f, 1600.0f, 16.0f, 16.0f};
    Rectangle herbSprite_{240.0f, 272.0f, 16.0f, 16.0f};
    Rectangle swordSprite_{736.0f, 992.0f, 16.0f, 16.0f};
};
