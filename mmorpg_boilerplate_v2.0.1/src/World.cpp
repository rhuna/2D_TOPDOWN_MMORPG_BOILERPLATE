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

bool HasMovement(const Actor& actor) {
    return std::fabs(actor.moveIntent.x) > 0.01f || std::fabs(actor.moveIntent.y) > 0.01f;
}
}

World::World() {
    map_ = BuildMap();
    LoadAssets();

    slimeQuest_.title = "Slime Cleanup";
    slimeQuest_.description = "Defeat 3 slimes for the village elder.";
    slimeQuest_.killsNeeded = 3;
    message_ = "Atlas build loaded. WASD moves, E interacts, Space attacks.";

    for (int y = 0; y < static_cast<int>(map_.size()); ++y) {
        for (int x = 0; x < static_cast<int>(map_[y].size()); ++x) {
            Vector2 worldPos{static_cast<float>(x * tileSize_ + 2), static_cast<float>(y * tileSize_ + 2)};

            if (map_[y][x] == 'P') {
                player_.position = worldPos;
                player_.color = WHITE;
                player_.hp = 20;
                player_.maxHp = 20;
                player_.speed = 165.0f;
                player_.facing = Direction::Down;
            } else if (map_[y][x] == 'S') {
                Enemy slime;
                slime.position = worldPos;
                slime.color = WHITE;
                slime.name = "Slime";
                slime.hp = 6;
                slime.maxHp = 6;
                slime.speed = 65.0f;
                slime.facing = Direction::Left;
                enemies_.push_back(slime);
            } else if (map_[y][x] == 'E') {
                Npc elder;
                elder.position = worldPos;
                elder.color = WHITE;
                elder.hp = 1;
                elder.maxHp = 1;
                elder.name = "Elder Rowan";
                elder.idleText = "The road is safer than it used to be.";
                elder.questText = "Please clear out 3 slimes near the village.";
                elder.facing = Direction::Down;
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

    if (!tilesetLoaded_) {
        message_ = "Could not load assets/tileset.png. Using fallback rectangles.";
        return;
    }

    SetTextureFilter(tileset_, TEXTURE_FILTER_POINT);
    SetupAtlas();
}

void World::SetupAtlas() {
    tiles_.grass = {Cell(0, 27), Cell(1, 27), Cell(2, 27), Cell(3, 27), Cell(4, 27)};
    tiles_.path = {Cell(0, 8), Cell(1, 8), Cell(2, 8), Cell(3, 8), Cell(4, 8)};
    tiles_.dirt = {Cell(0, 19), Cell(1, 19), Cell(2, 19), Cell(3, 19)};
    tiles_.wallTop = {Cell(0, 24), Cell(1, 24), Cell(2, 24), Cell(3, 24)};
    tiles_.wallFace = {Cell(0, 26), Cell(1, 26), Cell(2, 26), Cell(3, 26)};
    tiles_.herb = Cell(16, 17);
    tiles_.sword = Cell(18, 17);

    auto setClip = [&](DirectionalSprite& spriteSet, Direction direction, bool walking, std::initializer_list<AtlasFrame> frames, float frameDuration = 0.20f) {
        AnimationClip clip;
        clip.frameDuration = frameDuration;
        clip.frames = frames;
        if (walking) {
            spriteSet.walk[DirectionIndex(direction)] = clip;
        } else {
            spriteSet.idle[DirectionIndex(direction)] = clip;
        }
    };

    setClip(playerAtlas_, Direction::Down, false, {{Cell(0, 108), false, 0.0f}});
    setClip(playerAtlas_, Direction::Down, true,  {{Cell(0, 108), false, 0.0f}, {Cell(1, 108), false, -1.0f}}, 0.16f);
    setClip(playerAtlas_, Direction::Up, false,   {{Cell(6, 108), false, 0.0f}});
    setClip(playerAtlas_, Direction::Up, true,    {{Cell(6, 108), false, 0.0f}, {Cell(7, 108), false, -1.0f}}, 0.16f);
    setClip(playerAtlas_, Direction::Left, false, {{Cell(10, 108), false, 0.0f}});
    setClip(playerAtlas_, Direction::Left, true,  {{Cell(10, 108), false, 0.0f}, {Cell(11, 108), false, -1.0f}}, 0.16f);
    setClip(playerAtlas_, Direction::Right, false, {{Cell(10, 108), true, 0.0f}});
    setClip(playerAtlas_, Direction::Right, true,  {{Cell(10, 108), true, 0.0f}, {Cell(11, 108), true, -1.0f}}, 0.16f);

    setClip(npcAtlas_, Direction::Down, false, {{Cell(12, 108), false, 0.0f}});
    setClip(npcAtlas_, Direction::Down, true,  {{Cell(12, 108), false, 0.0f}, {Cell(13, 108), false, -1.0f}}, 0.18f);
    setClip(npcAtlas_, Direction::Up, false,   {{Cell(18, 108), false, 0.0f}});
    setClip(npcAtlas_, Direction::Up, true,    {{Cell(18, 108), false, 0.0f}, {Cell(19, 108), false, -1.0f}}, 0.18f);
    setClip(npcAtlas_, Direction::Left, false, {{Cell(14, 108), false, 0.0f}});
    setClip(npcAtlas_, Direction::Left, true,  {{Cell(14, 108), false, 0.0f}, {Cell(15, 108), false, -1.0f}}, 0.18f);
    setClip(npcAtlas_, Direction::Right, false, {{Cell(14, 108), true, 0.0f}});
    setClip(npcAtlas_, Direction::Right, true,  {{Cell(14, 108), true, 0.0f}, {Cell(15, 108), true, -1.0f}}, 0.18f);

    setClip(slimeAtlas_, Direction::Down, false, {{Cell(7, 105), false, 0.0f}});
    setClip(slimeAtlas_, Direction::Down, true,  {{Cell(7, 105), false, 0.0f}, {Cell(8, 105), false, -1.0f}, {Cell(9, 105), false, 0.0f}}, 0.12f);
    setClip(slimeAtlas_, Direction::Up, false,   {{Cell(7, 105), false, 0.0f}});
    setClip(slimeAtlas_, Direction::Up, true,    {{Cell(7, 105), false, 0.0f}, {Cell(8, 105), false, -1.0f}, {Cell(9, 105), false, 0.0f}}, 0.12f);
    setClip(slimeAtlas_, Direction::Left, false, {{Cell(7, 105), false, 0.0f}});
    setClip(slimeAtlas_, Direction::Left, true,  {{Cell(7, 105), false, 0.0f}, {Cell(8, 105), false, -1.0f}, {Cell(9, 105), false, 0.0f}}, 0.12f);
    setClip(slimeAtlas_, Direction::Right, false, {{Cell(7, 105), true, 0.0f}});
    setClip(slimeAtlas_, Direction::Right, true,  {{Cell(7, 105), true, 0.0f}, {Cell(8, 105), true, -1.0f}, {Cell(9, 105), true, 0.0f}}, 0.12f);
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
        DrawNpc(npc);
    }

    for (const auto& enemy : enemies_) {
        if (enemy.alive) {
            DrawEnemy(enemy);
        }
    }

    for (const auto& drop : drops_) {
        if (!drop.taken) {
            DrawPickupSprite(drop.position, tiles_.herb, WHITE);
        }
    }

    DrawPlayer();
    DrawHud();
}

void World::UpdatePlayer(float dt) {
    Vector2 move{};

    if (IsKeyDown(KEY_W)) move.y -= 1.0f;
    if (IsKeyDown(KEY_S)) move.y += 1.0f;
    if (IsKeyDown(KEY_A)) move.x -= 1.0f;
    if (IsKeyDown(KEY_D)) move.x += 1.0f;

    move = NormalizeOrZero(move);
    player_.moveIntent = move;

    if (move.x != 0.0f || move.y != 0.0f) {
        player_.facing = DirectionFromVector(move);
        player_.animClock += dt;
    } else {
        player_.animClock = 0.0f;
    }

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
        enemy.moveIntent = Vector2{0.0f, 0.0f};

        Vector2 toPlayer{player_.position.x - enemy.position.x, player_.position.y - enemy.position.y};
        float distance = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);

        if (distance > 28.0f && distance < 220.0f) {
            Vector2 direction = NormalizeOrZero(toPlayer);
            Vector2 next = enemy.position;
            next.x += direction.x * enemy.speed * dt;
            next.y += direction.y * enemy.speed * dt;

            enemy.moveIntent = direction;
            enemy.facing = DirectionFromVector(direction);
            enemy.animClock += dt;

            if (!IsWall(next)) {
                enemy.position = next;
            }
        } else {
            enemy.animClock = 0.0f;
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

            char tile = map_[y][x];
            bool isWall = tile == '#';

            if (!tilesetLoaded_) {
                DrawRectangleRec(destination, isWall ? DARKGRAY : Color{30, 80, 35, 255});
                DrawRectangleLinesEx(destination, 1.0f, Fade(BLACK, 0.18f));
                continue;
            }

            Rectangle source{};
            if (isWall) {
                bool isTopEdge = (y == 0 || map_[y - 1][x] != '#');
                const std::vector<Rectangle>& palette = isTopEdge ? tiles_.wallTop : tiles_.wallFace;
                source = palette[TileVariantIndex(x, y, static_cast<int>(palette.size()))];
            } else if (tile == 'p') {
                source = tiles_.path[TileVariantIndex(x, y, static_cast<int>(tiles_.path.size()))];
            } else {
                source = tiles_.grass[TileVariantIndex(x, y, static_cast<int>(tiles_.grass.size()))];
            }

            DrawTexturePro(tileset_, source, destination, Vector2{0.0f, 0.0f}, 0.0f, WHITE);

            if (!isWall && tile != 'p' && TileVariantIndex(x + 13, y + 7, 8) == 0) {
                Rectangle dirtDest{
                    destination.x + 8.0f,
                    destination.y + 8.0f,
                    16.0f,
                    16.0f
                };
                DrawTexturePro(
                    tileset_,
                    tiles_.dirt[TileVariantIndex(x + 5, y + 11, static_cast<int>(tiles_.dirt.size()))],
                    dirtDest,
                    Vector2{0.0f, 0.0f},
                    0.0f,
                    Fade(WHITE, 0.75f)
                );
            }

            DrawRectangleLinesEx(destination, 1.0f, Fade(BLACK, 0.12f));
        }
    }
}

void World::DrawHud() const {
    DrawRectangle(0, 640, 1024, 128, Fade(BLACK, 0.75f));

    if (tilesetLoaded_) {
        Rectangle swordDestination{16.0f, 650.0f, 24.0f, 24.0f};
        DrawTexturePro(tileset_, tiles_.sword, swordDestination, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
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

void World::DrawPlayer() const {
    DrawAnimatedActor(player_, playerAtlas_, WHITE);
}

void World::DrawNpc(const Npc& npc) const {
    DrawAnimatedActor(npc, npcAtlas_, Color{245, 235, 200, 255});
    DrawText(npc.name.c_str(), static_cast<int>(npc.position.x) - 12, static_cast<int>(npc.position.y) - 18, 12, WHITE);
}

void World::DrawEnemy(const Enemy& enemy) const {
    DrawAnimatedActor(enemy, slimeAtlas_, WHITE);
    DrawRectangle(static_cast<int>(enemy.position.x), static_cast<int>(enemy.position.y) - 8, 28, 4, DARKGRAY);
    DrawRectangle(static_cast<int>(enemy.position.x), static_cast<int>(enemy.position.y) - 8, 28 * enemy.hp / enemy.maxHp, 4, RED);
}

void World::DrawAnimatedActor(const Actor& actor, const DirectionalSprite& spriteSet, Color tint) const {
    bool walking = HasMovement(actor);
    const AnimationClip& clip = SelectClip(spriteSet, actor.facing, walking);
    const AtlasFrame& frame = ResolveFrame(clip, actor.animClock);

    Rectangle destination{
        actor.position.x,
        actor.position.y + frame.yBob,
        actor.size.x,
        actor.size.y
    };

    if (tilesetLoaded_) {
        Rectangle source = frame.source;
        if (frame.flipX) {
            source.x += source.width;
            source.width *= -1.0f;
        }
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

Rectangle World::Cell(int col, int row) const {
    return Rectangle{
        static_cast<float>(col * atlasCellSize_),
        static_cast<float>(row * atlasCellSize_),
        static_cast<float>(atlasCellSize_),
        static_cast<float>(atlasCellSize_)
    };
}

int World::DirectionIndex(Direction direction) const {
    return static_cast<int>(direction);
}

Direction World::DirectionFromVector(Vector2 direction) const {
    if (std::fabs(direction.x) > std::fabs(direction.y)) {
        return direction.x >= 0.0f ? Direction::Right : Direction::Left;
    }
    return direction.y >= 0.0f ? Direction::Down : Direction::Up;
}

const AnimationClip& World::SelectClip(const DirectionalSprite& spriteSet, Direction facing, bool walking) const {
    int index = DirectionIndex(facing);
    const AnimationClip& clip = walking ? spriteSet.walk[index] : spriteSet.idle[index];
    if (!clip.frames.empty()) {
        return clip;
    }
    return spriteSet.idle[index];
}

const AtlasFrame& World::ResolveFrame(const AnimationClip& clip, float animClock) const {
    static AtlasFrame fallback{};
    if (clip.frames.empty()) {
        return fallback;
    }

    if (clip.frames.size() == 1 || clip.frameDuration <= 0.0f) {
        return clip.frames.front();
    }

    int frameIndex = static_cast<int>(animClock / clip.frameDuration) % static_cast<int>(clip.frames.size());
    return clip.frames[frameIndex];
}

int World::TileVariantIndex(int x, int y, int count) const {
    if (count <= 0) {
        return 0;
    }

    int seed = x * 73428767 ^ y * 912931;
    seed ^= (seed >> 13);
    seed *= 1274126177;
    if (seed < 0) {
        seed = -seed;
    }
    return seed % count;
}
