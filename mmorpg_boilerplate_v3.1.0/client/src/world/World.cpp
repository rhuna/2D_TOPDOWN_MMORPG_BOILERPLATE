#include "world/World.h"
#include "world/Data.h"
#include "net/NetworkClient.h"
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

// Construct the world, load the map, set up the atlas, and spawn all actors.
World::World() {
    map_ = BuildMap();
    LoadAssets();

    slimeQuest_.title = "Slime Cleanup";
    slimeQuest_.description = "Defeat 3 slimes for the village elder.";
    slimeQuest_.killsNeeded = 3;
    camera_.offset = Vector2{screenWidth_ * 0.5f, screenHeight_ * 0.5f};
    camera_.zoom = 2.0f;
    targetZoom_ = 2.0f;
    message_ = "Huge world loaded. Mouse wheel zooms, TAB opens world map, WASD moves, E interacts, Space attacks.";

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
            } else if (map_[y][x] == 'E' || map_[y][x] == 'N') {
                Npc elder;
                elder.position = worldPos;
                elder.color = WHITE;
                elder.hp = 1;
                elder.maxHp = 1;
                elder.name = map_[y][x] == 'E' ? "Elder Rowan" : "Wanderer";
                elder.idleText = map_[y][x] == 'E' ? "The road is safer than it used to be." : "You've entered another travel region. Keep following the roads.";
                elder.questText = "Please clear out 3 slimes near the village.";
                elder.regionName = map_[y][x] == 'E' ? "Starter Village" : "Crossroads Camp";
                elder.facing = Direction::Down;
                npcs_.push_back(elder);
            }
        }
    }
}

// Release GPU texture memory when the world shuts down.
World::~World() {
    if (tilesetLoaded_) {
        UnloadTexture(tileset_);
    }
}


// Update client-side remote-player visuals from the latest network snapshots.
//
// The server only sends positions in this prototype.
// The client infers facing direction and walking animation from the change in position.
void World::UpdateRemotePlayers(const std::unordered_map<int, RemotePlayer> &snapshots, int localId, float dt)
{
    remotePlayers_.erase(
        std::remove_if(remotePlayers_.begin(), remotePlayers_.end(),
            [&](const RemoteActor& remote) {
                return snapshots.find(remote.id) == snapshots.end() || remote.id == localId;
            }),
        remotePlayers_.end()
    );

    for (const auto& [id, snapshot] : snapshots) {
        if (id == localId) {
            continue;
        }

        auto it = std::find_if(remotePlayers_.begin(), remotePlayers_.end(),
            [&](const RemoteActor& remote) {
                return remote.id == id;
            });

        if (it == remotePlayers_.end()) {
            RemoteActor remote;
            remote.id = id;
            remote.position = {snapshot.x, snapshot.y};
            remote.targetPosition = {snapshot.x, snapshot.y};
            remote.size = player_.size;
            remote.facing = Direction::Down;
            remote.color = Color{170, 210, 255, 255};
            remote.hp = snapshot.hp;
            remote.name = snapshot.name;
            remotePlayers_.push_back(remote);
            continue;
        }

        it->targetPosition = {snapshot.x, snapshot.y};
        it->hp = snapshot.hp;
        it->name = snapshot.name;

        Vector2 toTarget{
            it->targetPosition.x - it->position.x,
            it->targetPosition.y - it->position.y
        };

        const float distance = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);
        if (distance > 0.25f) {
            it->moveIntent = NormalizeOrZero(toTarget);
            it->facing = DirectionFromVector(it->moveIntent);
            it->animClock += dt;

            const float smoothing = std::min(1.0f, dt * 10.0f);
            it->position.x += toTarget.x * smoothing;
            it->position.y += toTarget.y * smoothing;
        } else {
            it->position = it->targetPosition;
            it->moveIntent = {0.0f, 0.0f};
        }
    }
}

// Load the tileset texture and prepare atlas rectangles.
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

// Define all tile and sprite source rectangles inside the tileset atlas.
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

// Per-frame update entry point.
// Handles input toggles, zoom, combat cooldowns, actors, drops, and camera.
void World::Update(float dt) {
    screenWidth_ = GetScreenWidth();
    screenHeight_ = GetScreenHeight();
    camera_.offset = Vector2{screenWidth_ * 0.5f, screenHeight_ * 0.5f};

    if (IsKeyPressed(KEY_TAB)) {
        showBigMap_ = !showBigMap_;
    }

    const float wheel = GetMouseWheelMove();
    if (wheel != 0.0f && !showBigMap_) {
        targetZoom_ = std::clamp(targetZoom_ + wheel * 0.20f, 1.0f, 3.5f);
    }

    playerAttackTimer_ -= dt;
    UpdatePlayer(dt);
    UpdateEnemies(dt);
    HandleCombat(dt);
    HandleInteraction();
    HandleDrops();
    UpdateCamera(dt);
}

// Draw the complete frame: world, actors, HUD, minimap, then optional world map.
void World::Draw() const {
    BeginMode2D(camera_);
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
    for (const auto &remote : remotePlayers_)
    {
        DrawRemotePlayer(remote);
    }
    EndMode2D();
    DrawHud();
    DrawMinimap();
    if (showBigMap_) {
        DrawWorldMapOverlay();
    }
}

// Update player input, movement, facing, animation clock, and wall collision.
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

// Simple enemy AI: chase the player when nearby and attack in melee range.
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

// Handle player melee attacks when Space is pressed.
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

// Talk to nearby NPCs or pick up nearby drops when E is pressed.
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

        message_ = npc.name + " (" + npc.regionName + "): " + npc.idleText;
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

// Remove drops that were already picked up.
void World::HandleDrops() {
    drops_.erase(
        std::remove_if(drops_.begin(), drops_.end(), [](const Drop& drop) { return drop.taken; }),
        drops_.end()
    );
}


Vector2 World::GetPlayerPosition() const {
    return player_.position;
}

Vector2 World::GetWorldPixelSize() const {
    if (map_.empty()) {
        return Vector2{0.0f, 0.0f};
    }
    return Vector2{static_cast<float>(map_[0].size() * tileSize_), static_cast<float>(map_.size() * tileSize_)};
}

// Smoothly update camera follow and clamp it to world bounds.
void World::UpdateCamera(float dt) {
    const float activeZoom = showBigMap_ ? 0.42f : targetZoom_;
    camera_.zoom += (activeZoom - camera_.zoom) * std::min(1.0f, dt * 10.0f);

    Vector2 worldSize = GetWorldPixelSize();
    const float halfVisibleWidth = screenWidth_ * 0.5f / camera_.zoom;
    const float halfVisibleHeight = screenHeight_ * 0.5f / camera_.zoom;

    camera_.target.x = std::clamp(player_.position.x + player_.size.x * 0.5f, halfVisibleWidth, worldSize.x - halfVisibleWidth);
    camera_.target.y = std::clamp(player_.position.y + player_.size.y * 0.5f, halfVisibleHeight, worldSize.y - halfVisibleHeight);

    if (worldSize.x <= halfVisibleWidth * 2.0f) {
        camera_.target.x = worldSize.x * 0.5f;
    }
    if (worldSize.y <= halfVisibleHeight * 2.0f) {
        camera_.target.y = worldSize.y * 0.5f;
    }
}

// Draw only the visible region of the map for performance.
void World::DrawMap() const {
    const int mapHeight = static_cast<int>(map_.size());
    const int mapWidth = mapHeight > 0 ? static_cast<int>(map_[0].size()) : 0;
    const float halfVisibleWidth = screenWidth_ * 0.5f / camera_.zoom;
    const float halfVisibleHeight = screenHeight_ * 0.5f / camera_.zoom;

    const int startX = std::max(0, static_cast<int>((camera_.target.x - halfVisibleWidth) / tileSize_) - 2);
    const int endX = std::min(mapWidth - 1, static_cast<int>((camera_.target.x + halfVisibleWidth) / tileSize_) + 2);
    const int startY = std::max(0, static_cast<int>((camera_.target.y - halfVisibleHeight) / tileSize_) - 2);
    const int endY = std::min(mapHeight - 1, static_cast<int>((camera_.target.y + halfVisibleHeight) / tileSize_) + 2);

    for (int y = startY; y <= endY; ++y) {
        for (int x = startX; x <= endX; ++x) {
            Rectangle destination{
                static_cast<float>(x * tileSize_),
                static_cast<float>(y * tileSize_),
                static_cast<float>(tileSize_),
                static_cast<float>(tileSize_)
            };

            char tile = map_[y][x];
            const bool isWall = tile == '#';

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
        }
    }
}

// Draw bottom HUD bar with player stats, quest text, inventory, and status message.
void World::DrawHud() const {
    const int hudHeight = 128;
    DrawRectangle(0, screenHeight_ - hudHeight, screenWidth_, hudHeight, Fade(BLACK, 0.75f));

    if (tilesetLoaded_) {
        Rectangle swordDestination{16.0f, static_cast<float>(screenHeight_ - 118), 24.0f, 24.0f};
        DrawTexturePro(tileset_, tiles_.sword, swordDestination, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
    }

    std::ostringstream stats;
    stats << "HP: " << player_.hp << "/" << player_.maxHp
          << "   XP: " << player_.xp
          << "   Gold: " << player_.gold
          << "   Weapon: " << player_.weapon.name
          << " (DMG " << player_.weapon.damage << ")";
    DrawText(stats.str().c_str(), 48, screenHeight_ - 116, 20, WHITE);

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
    DrawText(quest.str().c_str(), 16, screenHeight_ - 88, 20, YELLOW);

    std::ostringstream inventory;
    inventory << "Inventory: ";
    for (std::size_t i = 0; i < player_.inventory.size(); ++i) {
        inventory << player_.inventory[i].name << " x" << player_.inventory[i].count;
        if (i + 1 < player_.inventory.size()) {
            inventory << ", ";
        }
    }
    DrawText(inventory.str().c_str(), 16, screenHeight_ - 60, 20, SKYBLUE);
    const char* controls = showBigMap_ ? "TAB closes world map" : "Mouse wheel zooms, TAB opens world map";
    DrawText(controls, screenWidth_ - 320, screenHeight_ - 32, 18, LIGHTGRAY);
    DrawText(message_.c_str(), 16, screenHeight_ - 32, 18, LIGHTGRAY);
}


// Draw a compact minimap in the top-right corner.
void World::DrawMinimap() const {
    const int miniW = 220;
    const int miniH = 220;
    const int pad = 16;
    const int originX = screenWidth_ - miniW - pad;
    const int originY = 16;
    const int mapHeight = static_cast<int>(map_.size());
    const int mapWidth = mapHeight > 0 ? static_cast<int>(map_[0].size()) : 0;

    DrawRectangle(originX - 4, originY - 4, miniW + 8, miniH + 8, Fade(BLACK, 0.65f));
    DrawRectangleLines(originX - 4, originY - 4, miniW + 8, miniH + 8, Fade(WHITE, 0.35f));

    const float scaleX = static_cast<float>(miniW) / static_cast<float>(mapWidth);
    const float scaleY = static_cast<float>(miniH) / static_cast<float>(mapHeight);

    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            Color c = DARKGREEN;
            if (map_[y][x] == '#') c = GRAY;
            else if (map_[y][x] == 'p') c = Color{166, 124, 82, 255};
            DrawRectangle(
                originX + static_cast<int>(x * scaleX),
                originY + static_cast<int>(y * scaleY),
                std::max(1, static_cast<int>(std::ceil(scaleX))),
                std::max(1, static_cast<int>(std::ceil(scaleY))),
                c
            );
        }
    }

    for (const auto& enemy : enemies_) {
        if (!enemy.alive) continue;
        DrawCircle(
            originX + static_cast<int>((enemy.position.x / tileSize_) * scaleX),
            originY + static_cast<int>((enemy.position.y / tileSize_) * scaleY),
            2.0f,
            RED
        );
    }

    for (const auto& npc : npcs_) {
        DrawCircle(
            originX + static_cast<int>((npc.position.x / tileSize_) * scaleX),
            originY + static_cast<int>((npc.position.y / tileSize_) * scaleY),
            2.0f,
            YELLOW
        );
    }

    DrawCircle(
        originX + static_cast<int>((player_.position.x / tileSize_) * scaleX),
        originY + static_cast<int>((player_.position.y / tileSize_) * scaleY),
        3.0f,
        SKYBLUE
    );

    const float halfVisibleWidth = screenWidth_ * 0.5f / camera_.zoom;
    const float halfVisibleHeight = screenHeight_ * 0.5f / camera_.zoom;
    Rectangle visible{
        originX + ((camera_.target.x - halfVisibleWidth) / tileSize_) * scaleX,
        originY + ((camera_.target.y - halfVisibleHeight) / tileSize_) * scaleY,
        (screenWidth_ / camera_.zoom / tileSize_) * scaleX,
        (screenHeight_ / camera_.zoom / tileSize_) * scaleY
    };
    DrawRectangleLinesEx(visible, 1.0f, WHITE);
    DrawText("Mini Map", originX + 6, originY + miniH + 6, 18, WHITE);
}

// Draw a large fullscreen world map overlay when TAB is enabled.
void World::DrawWorldMapOverlay() const {
    const int pad = 42;
    const int originX = pad;
    const int originY = pad;
    const int overlayW = screenWidth_ - pad * 2;
    const int overlayH = screenHeight_ - pad * 2;
    const int mapHeight = static_cast<int>(map_.size());
    const int mapWidth = mapHeight > 0 ? static_cast<int>(map_[0].size()) : 0;
    const float scaleX = static_cast<float>(overlayW) / static_cast<float>(mapWidth);
    const float scaleY = static_cast<float>(overlayH) / static_cast<float>(mapHeight);

    DrawRectangle(0, 0, screenWidth_, screenHeight_, Fade(BLACK, 0.72f));
    DrawRectangle(originX - 6, originY - 6, overlayW + 12, overlayH + 12, Fade(BLACK, 0.85f));
    DrawRectangleLines(originX - 6, originY - 6, overlayW + 12, overlayH + 12, Fade(WHITE, 0.55f));

    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            Color c = Color{44, 96, 48, 255};
            if (map_[y][x] == '#') c = Color{76, 82, 96, 255};
            else if (map_[y][x] == 'p') c = Color{176, 132, 84, 255};
            DrawRectangle(
                originX + static_cast<int>(x * scaleX),
                originY + static_cast<int>(y * scaleY),
                std::max(1, static_cast<int>(std::ceil(scaleX))),
                std::max(1, static_cast<int>(std::ceil(scaleY))),
                c
            );
        }
    }

    for (const auto& npc : npcs_) {
        DrawCircle(
            originX + static_cast<int>((npc.position.x / tileSize_) * scaleX),
            originY + static_cast<int>((npc.position.y / tileSize_) * scaleY),
            3.5f,
            GOLD
        );
    }

    for (const auto& enemy : enemies_) {
        if (!enemy.alive) continue;
        DrawCircle(
            originX + static_cast<int>((enemy.position.x / tileSize_) * scaleX),
            originY + static_cast<int>((enemy.position.y / tileSize_) * scaleY),
            2.0f,
            RED
        );
    }

    DrawCircle(
        originX + static_cast<int>((player_.position.x / tileSize_) * scaleX),
        originY + static_cast<int>((player_.position.y / tileSize_) * scaleY),
        4.0f,
        SKYBLUE
    );

    const float halfVisibleWidth = screenWidth_ * 0.5f / camera_.zoom;
    const float halfVisibleHeight = screenHeight_ * 0.5f / camera_.zoom;
    Rectangle visible{
        originX + ((camera_.target.x - halfVisibleWidth) / tileSize_) * scaleX,
        originY + ((camera_.target.y - halfVisibleHeight) / tileSize_) * scaleY,
        (screenWidth_ / camera_.zoom / tileSize_) * scaleX,
        (screenHeight_ / camera_.zoom / tileSize_) * scaleY
    };
    DrawRectangleLinesEx(visible, 2.0f, WHITE);

    DrawText("World Map", originX, 10, 28, WHITE);
    DrawText("Starter Village", originX + 18, originY + 18, 20, YELLOW);
    DrawText("North Marsh", originX + static_cast<int>(mapWidth * 0.35f * scaleX), originY + 28, 20, YELLOW);
    DrawText("Central Road", originX + static_cast<int>(mapWidth * 0.45f * scaleX), originY + static_cast<int>(mapHeight * 0.43f * scaleY), 20, YELLOW);
    DrawText("East Frontier", originX + static_cast<int>(mapWidth * 0.73f * scaleX), originY + static_cast<int>(mapHeight * 0.72f * scaleY), 20, YELLOW);
    DrawText("TAB to close", screenWidth_ - 190, 14, 22, LIGHTGRAY);
}

// Draw the player using the directional player atlas.
void World::DrawPlayer() const {
    DrawAnimatedActor(player_, playerAtlas_, WHITE);
}

// Draw one NPC and label them with their name.
void World::DrawNpc(const Npc& npc) const {
    DrawAnimatedActor(npc, npcAtlas_, Color{245, 235, 200, 255});
    DrawText(npc.name.c_str(), static_cast<int>(npc.position.x) - 12, static_cast<int>(npc.position.y) - 18, 12, WHITE);
}

// Draw one enemy and a small HP bar.
void World::DrawEnemy(const Enemy& enemy) const {
    DrawAnimatedActor(enemy, slimeAtlas_, WHITE);
    DrawRectangle(static_cast<int>(enemy.position.x), static_cast<int>(enemy.position.y) - 8, 28, 4, DARKGRAY);
    DrawRectangle(static_cast<int>(enemy.position.x), static_cast<int>(enemy.position.y) - 8, 28 * enemy.hp / enemy.maxHp, 4, RED);
}


// Draw a remote networked player using the same player atlas as the local player.
void World::DrawRemotePlayer(const RemoteActor &remote) const
{
    DrawAnimatedActor(remote, playerAtlas_, Color{170, 210, 255, 255});

    DrawText(remote.name.c_str(),
             static_cast<int>(remote.position.x - 6.0f),
             static_cast<int>(remote.position.y - 12.0f),
             10,
             WHITE);

    DrawRectangle(static_cast<int>(remote.position.x),
                  static_cast<int>(remote.position.y) - 18,
                  30, 4, DARKGRAY);

    DrawRectangle(static_cast<int>(remote.position.x),
                  static_cast<int>(remote.position.y) - 18,
                  static_cast<int>(30 * remote.hp / 20.0f),
                  4, RED);
}

// Draw any actor using the correct idle/walk directional animation clip.
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

// Draw an item drop sprite at a world position.
void World::DrawPickupSprite(Vector2 position, Rectangle source, Color tint) const {
    Rectangle destination{position.x, position.y, 24.0f, 24.0f};
    if (tilesetLoaded_) {
        DrawTexturePro(tileset_, source, destination, Vector2{0.0f, 0.0f}, 0.0f, tint);
    } else {
        DrawCircle(static_cast<int>(position.x) + 12, static_cast<int>(position.y) + 12, 8.0f, tint);
    }
}

// Add a picked-up item into the player's inventory, stacking when possible.
void World::TryPickup(const std::string& itemName, int amount) {
    for (auto& item : player_.inventory) {
        if (item.name == itemName) {
            item.count += amount;
            return;
        }
    }
    player_.inventory.push_back(InventoryItem{itemName, amount});
}

// Test the four corners of an actor-sized box against wall tiles.
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

// Distance helper for interactions and pickups.
bool World::IsClose(Vector2 a, Vector2 b, float distance) const {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy) <= distance;
}

// Convert atlas grid coordinates into a source rectangle.
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

// Choose the dominant cardinal direction from a movement vector.
Direction World::DirectionFromVector(Vector2 direction) const {
    if (std::fabs(direction.x) > std::fabs(direction.y)) {
        return direction.x >= 0.0f ? Direction::Right : Direction::Left;
    }
    return direction.y >= 0.0f ? Direction::Down : Direction::Up;
}

// Pick the correct clip for facing + movement state.
const AnimationClip& World::SelectClip(const DirectionalSprite& spriteSet, Direction facing, bool walking) const {
    int index = DirectionIndex(facing);
    const AnimationClip& clip = walking ? spriteSet.walk[index] : spriteSet.idle[index];
    if (!clip.frames.empty()) {
        return clip;
    }
    return spriteSet.idle[index];
}

// Convert animation time into a specific frame.
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

// Stable pseudo-random tile picker so the ground looks less repetitive.
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
