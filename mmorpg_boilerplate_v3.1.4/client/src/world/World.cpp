#include "world/World.h"

#include "world/Data.h"
#include "gameplay/ItemFactory.h"

#include <algorithm>
#include <cmath>

namespace
{
Vector2 NormalizeOrZero(Vector2 value)
{
    const float length = std::sqrt(value.x * value.x + value.y * value.y);
    if (length <= 0.0001f)
    {
        return Vector2{0.0f, 0.0f};
    }
    return Vector2{value.x / length, value.y / length};
}

} // namespace

// Construct the world, load the map, set up the atlas, and spawn all actors.
World::World() {
    map_ = BuildMap();
    LoadAssets();

    questSystem_.LoadFromJson("data/quests.json");
    // The quest system will also check this field to prevent over-tracking kills after the quest is complete.
    camera_.offset = Vector2{screenWidth_ * 0.5f, screenHeight_ * 0.5f};
    // Start with a zoomed-out view so players can see the world and their position in it right away.
    camera_.zoom = 2.0f;
    // The target zoom is what the camera will smoothly interpolate towards, so we set it to the same value here.
    targetZoom_ = 2.0f;
    // A message to display in the HUD. In a real game, you'd have a more robust system for showing tutorial messages and quest updates.
    message_ = "Huge world loaded. Mouse wheel zooms, TAB opens world map, WASD moves, E interacts, Space attacks.";

    // Spawn actors based on map symbols. In a real game, you'd likely have separate spawn data and more complex logic for different regions.
    for (int y = 0; y < static_cast<int>(map_.size()); ++y) {
        // We add 2 pixels of padding here so actors aren't perfectly flush with the tile edges. This looks better, especially when zoomed in.
        for (int x = 0; x < static_cast<int>(map_[y].size()); ++x) {
            // Skip empty tiles and walls since they don't spawn actors.
            Vector2 worldPos{static_cast<float>(x * tileSize_ + 2), static_cast<float>(y * tileSize_ + 2)};
            // The player is spawned on the map tile marked with 'P', slimes are marked with 'S', and NPCs are marked with 'E' or 'N' depending on their type.
            if (map_[y][x] == 'P') {
                // Set up the player with some starting stats. In a real game, you'd likely have a character creation process and load saved player data instead.
                player_.position = worldPos;
                player_.color = WHITE;
                player_.hp = 20;
                player_.maxHp = 20;
                player_.speed = 165.0f;
                player_.facing = Direction::Down;
                // The player starts with a basic weapon and some inventory items to demonstrate those systems, but in a real game you'd have item pickups and shops to acquire these instead.
            } else if (map_[y][x] == 'S') {
                // Set up a slime enemy. In a real game, you'd likely have different enemy types with different stats and behaviors, and you'd load these from data files instead of hardcoding them.
                Enemy slime;
                slime.position = worldPos;
                slime.color = WHITE;
                slime.name = "Slime";
                slime.hp = 6;
                slime.maxHp = 6;
                slime.speed = 65.0f;
                slime.facing = Direction::Left;
                enemies_.push_back(slime);
                // The slime quest system will check this field when slimes are killed to track quest progress.
            }
            else if (map_[y][x] == 'E' || map_[y][x] == 'N' || map_[y][x] == 'M')
            {
                Npc npc;
                npc.position = worldPos;
                npc.color = WHITE;
                npc.hp = 1;
                npc.maxHp = 1;
                npc.facing = Direction::Down;

                if (map_[y][x] == 'E')
                {
                    npc.name = "Elder Rowan";
                    npc.idleText = "The road is safer than it used to be.";
                    npc.questText = "Please clear out 3 slimes near the village.";
                    npc.regionName = "Starter Village";
                }
                else if (map_[y][x] == 'M')
                {
                    npc.name = "Shopkeeper Mira";
                    npc.idleText = "Take a look at my wares.";
                    npc.regionName = "Village Shop";
                    npc.isMerchant = true;
                    npc.shopInventory = {
                        {"Potion", 10, 1, ItemCategory::Consumable, "Restores a strong amount of health.", 0, 0, 10},
                        {"Herb", 4, 1, ItemCategory::Consumable, "Restores a little health.", 0, 0, 4},
                        {"Iron Sword", 30, 1, ItemCategory::Weapon, "A stronger sword than your starter blade.", 6, 0, 0},
                        {"Leather Armor", 24, 1, ItemCategory::Armor, "Light armor for new adventurers.", 0, 3, 0}};
                }
                else
                {
                    npc.name = "Wanderer";
                    npc.idleText = "You've entered another travel region. Keep following the roads.";
                    npc.questText = "Please clear out 3 slimes near the village.";
                    npc.regionName = "Crossroads Camp";
                }

                npcs_.push_back(npc);
            }
        }
    }

    // Overworld building door -> shop interior
    buildingDoors_.push_back(BuildingDoor{
        Rectangle{
            35.0f * tileSize_,
            23.0f * tileSize_,
            static_cast<float>(tileSize_),
            static_cast<float>(tileSize_)},
        Vector2{306.0f * tileSize_ + 2.0f, 27.0f * tileSize_ + 2.0f}, // inside spawn
        Vector2{35.0f * tileSize_ + 2.0f, 24.0f * tileSize_ + 2.0f},  // outside spawn
        "starter_shop"});

    // Shop interior exit -> overworld
    exitDoors_.push_back(ExitDoor{
        Rectangle{
            306.0f * tileSize_,
            28.0f * tileSize_,
            static_cast<float>(tileSize_),
            static_cast<float>(tileSize_)},
        Vector2{35.0f * tileSize_ + 2.0f, 24.0f * tileSize_ + 2.0f},
        "starter_shop"});
}

// Release GPU texture memory when the world shuts down.
World::~World() {
    if (tilesetLoaded_) {
        UnloadTexture(tileset_);
    }
}

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

Vector2 World::GetPlayerPosition() const {
    return player_.position;
}

Vector2 World::GetWorldPixelSize() const {
    if (map_.empty()) {
        return Vector2{0.0f, 0.0f};
    }
    return Vector2{static_cast<float>(map_[0].size() * tileSize_), static_cast<float>(map_.size() * tileSize_)};
}

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

void World::TryPickup(const std::string &itemName, int amount)
{
    InventoryItem newItem = gameplay::MakeInventoryItemByName(itemName, amount);

    for (auto &item : player_.inventory)
    {
        // Stack only items that are allowed to stack.
        if (item.name == newItem.name && item.stackable)
        {
            item.count += amount;
            return;
        }
    }

    player_.inventory.push_back(newItem);
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

