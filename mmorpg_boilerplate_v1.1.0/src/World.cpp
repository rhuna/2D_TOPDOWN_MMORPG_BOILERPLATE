#include "World.h"
#include "Data.h"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace {
Vector2 NormalizeOrZero(Vector2 value) {
    float length = std::sqrt(value.x * value.x + value.y * value.y);
    if (length <= 0.0001f) {
        return Vector2{0.0f, 0.0f};
    }
    return Vector2{value.x / length, value.y / length};
}
}

World::World() {
    map_ = BuildMap();
    LoadAssets();

    slimeQuest_.title = "Slime Cleanup";
    slimeQuest_.description = "Defeat 3 slimes for the village elder.";
    slimeQuest_.killsNeeded = 3;
    message_ = "Tileset loaded. Walk with WASD. E talks or loots. Space attacks.";

    for (int y = 0; y < static_cast<int>(map_.size()); ++y) {
        for (int x = 0; x < static_cast<int>(map_[y].size()); ++x) {
            Vector2 worldPos{static_cast<float>(x * tileSize_ + 2), static_cast<float>(y * tileSize_ + 2)};
            if (map_[y][x] == 'P') {
                player_.position = worldPos;
                player_.color = BLUE;
                player_.hp = 20;
                player_.maxHp = 20;
                player_.speed = 165.0f;
            } else if (map_[y][x] == 'S') {
                Enemy slime;
                slime.position = worldPos;
                slime.color = GREEN;
                slime.name = "Slime";
                slime.hp = 6;
                slime.maxHp = 6;
                slime.speed = 65.0f;
                enemies_.push_back(slime);
            } else if (map_[y][x] == 'E') {
                Npc elder;
                elder.position = worldPos;
                elder.color = GOLD;
                elder.hp = 1;
                elder.maxHp = 1;
                elder.name = "Elder Rowan";
                elder.idleText = "The road is safer than it used to be.";
                elder.questText = "Please clear out 3 slimes near the village.";
                npcs_.push_back(elder);
            }
        }
    }
}

World::~World() {
    if (tilesetLoaded_) {
        UnloadTexture(tileset_);
    }
}

void World::LoadAssets() {
    tileset_ = LoadTexture("assets/tileset.png");
    tilesetLoaded_ = tileset_.id != 0;
    if (tilesetLoaded_) {
        SetTextureFilter(tileset_, TEXTURE_FILTER_POINT);
    } else {
        message_ = "Could not load assets/tileset.png. Using fallback rectangles.";
    }
}

void World::Update(float dt) {
    playerAttackTimer_ -= dt;
    UpdatePlayer(dt);
    UpdateEnemies(dt);
    HandleCombat(dt);
    HandleInteraction();
    HandleDrops();
}

void World::Draw() const {
    DrawMap();

    for (const auto& npc : npcs_) {
        DrawActorSprite(npc, npcSprite_, npc.color);
        DrawText(npc.name.c_str(), static_cast<int>(npc.position.x) - 10, static_cast<int>(npc.position.y) - 18, 12, WHITE);
    }

    for (const auto& enemy : enemies_) {
        if (!enemy.alive) {
            continue;
        }
        DrawActorSprite(enemy, slimeSprite_, enemy.color);
        DrawRectangle(static_cast<int>(enemy.position.x), static_cast<int>(enemy.position.y) - 8, 28, 4, DARKGRAY);
        DrawRectangle(static_cast<int>(enemy.position.x), static_cast<int>(enemy.position.y) - 8, 28 * enemy.hp / enemy.maxHp, 4, RED);
    }

    for (const auto& drop : drops_) {
        if (!drop.taken) {
            DrawPickupSprite(drop.position, herbSprite_, SKYBLUE);
        }
    }

    DrawActorSprite(player_, playerSprite_, WHITE);
    DrawHud();
}

void World::UpdatePlayer(float dt) {
    Vector2 move{};

    if (IsKeyDown(KEY_W)) move.y -= 1.0f;
    if (IsKeyDown(KEY_S)) move.y += 1.0f;
    if (IsKeyDown(KEY_A)) move.x -= 1.0f;
    if (IsKeyDown(KEY_D)) move.x += 1.0f;

    move = NormalizeOrZero(move);
    Vector2 next = player_.position;
    next.x += move.x * player_.speed * dt;
    next.y += move.y * player_.speed * dt;

    if (!IsWall(Vector2{next.x, player_.position.y})) {
        player_.position.x = next.x;
    }
    if (!IsWall(Vector2{player_.position.x, next.y})) {
        player_.position.y = next.y;
    }
}

void World::UpdateEnemies(float dt) {
    for (auto& enemy : enemies_) {
        if (!enemy.alive) {
            continue;
        }

        enemy.attackTimer -= dt;
        Vector2 toPlayer{player_.position.x - enemy.position.x, player_.position.y - enemy.position.y};
        float distance = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);

        if (distance > 28.0f && distance < 220.0f) {
            Vector2 direction = NormalizeOrZero(toPlayer);
            Vector2 next = enemy.position;
            next.x += direction.x * enemy.speed * dt;
            next.y += direction.y * enemy.speed * dt;

            if (!IsWall(next)) {
                enemy.position = next;
            }
        }

        if (distance <= 34.0f && enemy.attackTimer <= 0.0f) {
            player_.hp = std::max(0, player_.hp - 1);
            message_ = enemy.name + " hits you for 1 damage.";
            enemy.attackTimer = enemy.attackCooldown;
        }
    }
}

void World::HandleCombat(float) {
    if (!IsKeyPressed(KEY_SPACE) || playerAttackTimer_ > 0.0f) {
        return;
    }

    playerAttackTimer_ = player_.weapon.cooldown;

    for (auto& enemy : enemies_) {
        if (!enemy.alive) {
            continue;
        }

        float dx = enemy.position.x - player_.position.x;
        float dy = enemy.position.y - player_.position.y;
        float distance = std::sqrt(dx * dx + dy * dy);

        if (distance <= player_.weapon.range) {
            enemy.hp -= player_.weapon.damage;
            message_ = "You hit " + enemy.name + " with " + player_.weapon.name + ".";

            if (enemy.hp <= 0) {
                enemy.alive = false;
                player_.xp += 10;
                player_.gold += 5;
                drops_.push_back(Drop{enemy.position, "Herb", 1, false});
                message_ = "You defeated a " + enemy.name + " and found an Herb.";

                if (slimeQuest_.accepted && !slimeQuest_.completed) {
                    slimeQuest_.killsDone += 1;
                    if (slimeQuest_.killsDone >= slimeQuest_.killsNeeded) {
                        slimeQuest_.completed = true;
                        message_ = "Quest complete. Return to Elder Rowan.";
                    }
                }
            }
        }
    }
}

void World::HandleInteraction() {
    if (!IsKeyPressed(KEY_E)) {
        return;
    }

    for (auto& npc : npcs_) {
        if (!IsClose(player_.position, npc.position, 52.0f)) {
            continue;
        }

        if (!slimeQuest_.accepted) {
            slimeQuest_.accepted = true;
            message_ = npc.name + ": " + npc.questText;
            return;
        }

        if (slimeQuest_.completed && !slimeQuest_.rewarded) {
            slimeQuest_.rewarded = true;
            player_.gold += 25;
            player_.weapon = Weapon{"Bronze Blade", 4, 48.0f, 0.28f};
            message_ = npc.name + ": Thank you. Take this Bronze Blade and 25 gold.";
            return;
        }

        message_ = npc.name + ": " + npc.idleText;
        return;
    }

    for (auto& drop : drops_) {
        if (!drop.taken && IsClose(player_.position, drop.position, 40.0f)) {
            drop.taken = true;
            TryPickup(drop.itemName, drop.amount);
            message_ = "Picked up " + drop.itemName + " x" + std::to_string(drop.amount) + ".";
            return;
        }
    }
}

void World::HandleDrops() {
    drops_.erase(
        std::remove_if(drops_.begin(), drops_.end(), [](const Drop& drop) { return drop.taken; }),
        drops_.end()
    );
}

void World::DrawMap() const {
    for (int y = 0; y < static_cast<int>(map_.size()); ++y) {
        for (int x = 0; x < static_cast<int>(map_[y].size()); ++x) {
            Rectangle destination{
                static_cast<float>(x * tileSize_),
                static_cast<float>(y * tileSize_),
                static_cast<float>(tileSize_),
                static_cast<float>(tileSize_)
            };

            bool isWall = map_[y][x] == '#';

            if (tilesetLoaded_) {
                DrawTexturePro(
                    tileset_,
                    isWall ? wallTile_ : grassTile_,
                    destination,
                    Vector2{0.0f, 0.0f},
                    0.0f,
                    WHITE
                );
            } else {
                DrawRectangleRec(destination, isWall ? DARKGRAY : Color{30, 80, 35, 255});
            }

            DrawRectangleLinesEx(destination, 1.0f, Fade(BLACK, 0.18f));
        }
    }
}

void World::DrawHud() const {
    DrawRectangle(0, 640, 1024, 128, Fade(BLACK, 0.75f));

    if (tilesetLoaded_) {
        Rectangle swordDestination{16.0f, 650.0f, 24.0f, 24.0f};
        DrawTexturePro(tileset_, swordSprite_, swordDestination, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
    }

    std::ostringstream stats;
    stats << "HP: " << player_.hp << "/" << player_.maxHp
          << "   XP: " << player_.xp
          << "   Gold: " << player_.gold
          << "   Weapon: " << player_.weapon.name
          << " (DMG " << player_.weapon.damage << ")";
    DrawText(stats.str().c_str(), 48, 652, 20, WHITE);

    std::ostringstream quest;
    quest << "Quest: " << slimeQuest_.title << " ["
          << slimeQuest_.killsDone << "/" << slimeQuest_.killsNeeded << "]";
    if (!slimeQuest_.accepted) {
        quest.str("");
        quest.clear();
        quest << "Quest: Talk to Elder Rowan to get your first quest.";
    }
    if (slimeQuest_.rewarded) {
        quest.str("");
        quest.clear();
        quest << "Quest: Completed.";
    }
    DrawText(quest.str().c_str(), 16, 680, 20, YELLOW);

    std::ostringstream inventory;
    inventory << "Inventory: ";
    for (std::size_t i = 0; i < player_.inventory.size(); ++i) {
        inventory << player_.inventory[i].name << " x" << player_.inventory[i].count;
        if (i + 1 < player_.inventory.size()) {
            inventory << ", ";
        }
    }
    DrawText(inventory.str().c_str(), 16, 708, 20, SKYBLUE);
    DrawText(message_.c_str(), 16, 736, 18, LIGHTGRAY);
}

void World::DrawActorSprite(const Actor& actor, Rectangle source, Color tint) const {
    Rectangle destination{actor.position.x, actor.position.y, actor.size.x, actor.size.y};
    if (tilesetLoaded_) {
        DrawTexturePro(tileset_, source, destination, Vector2{0.0f, 0.0f}, 0.0f, tint);
    } else {
        DrawRectangleRec(destination, tint);
    }
}

void World::DrawPickupSprite(Vector2 position, Rectangle source, Color tint) const {
    Rectangle destination{position.x, position.y, 24.0f, 24.0f};
    if (tilesetLoaded_) {
        DrawTexturePro(tileset_, source, destination, Vector2{0.0f, 0.0f}, 0.0f, tint);
    } else {
        DrawCircle(static_cast<int>(position.x) + 12, static_cast<int>(position.y) + 12, 8.0f, tint);
    }
}

void World::TryPickup(const std::string& itemName, int amount) {
    for (auto& item : player_.inventory) {
        if (item.name == itemName) {
            item.count += amount;
            return;
        }
    }
    player_.inventory.push_back(InventoryItem{itemName, amount});
}

bool World::IsWall(Vector2 position) const {
    int left = static_cast<int>(position.x) / tileSize_;
    int top = static_cast<int>(position.y) / tileSize_;
    int right = static_cast<int>(position.x + player_.size.x - 1) / tileSize_;
    int bottom = static_cast<int>(position.y + player_.size.y - 1) / tileSize_;

    auto blocked = [&](int x, int y) {
        if (y < 0 || y >= static_cast<int>(map_.size())) return true;
        if (x < 0 || x >= static_cast<int>(map_[y].size())) return true;
        return map_[y][x] == '#';
    };

    return blocked(left, top) || blocked(right, top) || blocked(left, bottom) || blocked(right, bottom);
}

bool World::IsClose(Vector2 a, Vector2 b, float distance) const {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy) <= distance;
}

Rectangle World::RectOf(const Actor& actor) const {
    return Rectangle{actor.position.x, actor.position.y, actor.size.x, actor.size.y};
}
