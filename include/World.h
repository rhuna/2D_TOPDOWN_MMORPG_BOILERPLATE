#pragma once

#include "Types.h"
#include <string>
#include <vector>

class World {
public:
    World();

    void Update(float dt);
    void Draw() const;

private:
    void UpdatePlayer(float dt);
    void UpdateEnemies(float dt);
    void HandleCombat(float dt);
    void HandleInteraction();
    void HandleDrops();
    void DrawMap() const;
    void DrawHud() const;
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
};
