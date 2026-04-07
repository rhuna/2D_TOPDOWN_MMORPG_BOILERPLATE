#include "game/world/World.h"
#include "game/world/Data.h"

#include <algorithm>
#include <cmath>
#include <sstream>

namespace
{
InventoryItem MakeInventoryItemByName(const std::string &name, int amount)
{
    InventoryItem item{};
    item.name = name;
    item.amount = amount;

    if (name == "Potion")
    {
        item.price = 10;
        item.category = ItemCategory::Consumable;
        item.description = "Restores a strong amount of health.";
        item.healAmount = 10;
        item.stackable = true;
        return item;
    }

    if (name == "Herb")
    {
        item.price = 4;
        item.category = ItemCategory::Consumable;
        item.description = "Restores a little health.";
        item.healAmount = 4;
        item.stackable = true;
        return item;
    }

    if (name == "Iron Sword")
    {
        item.price = 30;
        item.category = ItemCategory::Weapon;
        item.description = "A stronger sword than your starter blade.";
        item.attackBonus = 6;
        item.stackable = false;
        return item;
    }

    if (name == "Leather Armor")
    {
        item.price = 24;
        item.category = ItemCategory::Armor;
        item.description = "Light armor for new adventurers.";
        item.defenseBonus = 3;
        item.stackable = false;
        return item;
    }

    if (name == "Health Potion Recipe")
    {
        item.price = 15;
        item.category = ItemCategory::Quest;
        item.description = "A recipe for a health potion. Maybe a local alchemist would be interested in this?";
        item.stackable = false;
        return item;
    }

    if (name == "Slime Goo")
    {
        item.price = 2;
        item.category = ItemCategory::Material;
        item.description = "A glob of slime goo. Could be useful for crafting.";
        item.stackable = false;
        return item;
    }

    if (name == "Magic Mushroom")
    {
        item.price = 3;
        item.category = ItemCategory::Material;
        item.description = "A mushroom with magical properties. Could be useful for crafting.";
        item.stackable = false;
        return item;
    }

    item.category = ItemCategory::Material;
    item.description = "Unknown item.";
    item.stackable = false;
    return item;
}
} // namespace

World::World()
{
    map_ = BuildMap();
    LoadAssets();

    questSystem_.LoadFromJson("data/quests.json");
    services_.BindQuestSystem(questSystem_);

    camera_.offset = Vector2{screenWidth_ * 0.5f, screenHeight_ * 0.5f};
    camera_.zoom = 2.0f;
    targetZoom_ = 2.0f;
    message_ = "Huge world loaded. Mouse wheel zooms, TAB opens world map, WASD moves, E interacts, Space attacks.";

    for (int y = 0; y < static_cast<int>(map_.size()); ++y)
    {
        for (int x = 0; x < static_cast<int>(map_[y].size()); ++x)
        {
            Vector2 worldPos{static_cast<float>(x * tileSize_ + 2), static_cast<float>(y * tileSize_ + 2)};

            if (map_[y][x] == 'P')
            {
                player_.position = worldPos;
                player_.color = WHITE;
                player_.hp = 20;
                player_.maxHp = 20;
                player_.speed = 165.0f;
                player_.facing = Direction::Down;
            }
            else if (map_[y][x] == 'S')
            {
                Enemy slime;
                slime.position = worldPos;
                slime.color = WHITE;
                slime.name = "Slime";
                slime.hp = 6;
                slime.maxHp = 6;
                slime.speed = 65.0f;
                slime.facing = Direction::Left;
                enemies_.push_back(slime);
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
                        {"Leather Armor", 24, 1, ItemCategory::Armor, "Light armor that adds defense.", 0, 3, 0},
                        {"Chain Vest", 40, 1, ItemCategory::Armor, "Heavier armor with better protection.", 0, 5, 0},
                        {"Bronze Blade", 0, 1, ItemCategory::Weapon, "A quest reward weapon. It's a bit better than your starter blade.", 4, 0, 0},
                        {"Health Potion Recipe", 15, 1, ItemCategory::Quest, "A recipe for a health potion. Maybe a local alchemist would be interested in this?", 0, 0, 0},
                        {"Slime Goo", 2, 1, ItemCategory::Material, "A glob of slime goo. Could be useful for crafting.", 0, 0, 0},
                        {"Magic Mushroom", 3, 1, ItemCategory::Material, "A mushroom with magical properties. Could be useful for crafting.", 0, 0, 0}};
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

    BuildingDoor door;
    door.trigger = Rectangle{35.0f * tileSize_, 23.0f * tileSize_, static_cast<float>(tileSize_), static_cast<float>(tileSize_)};
    door.insideSpawn = Vector2{306.0f * tileSize_ + 2.0f, 27.0f * tileSize_ + 2.0f};
    door.outsideSpawn = Vector2{35.0f * tileSize_ + 2.0f, 24.0f * tileSize_ + 2.0f};
    door.buildingId = "starter_shop";
    buildingDoors_.push_back(door);

    ExitDoor exitDoor;
    exitDoor.trigger = Rectangle{306.0f * tileSize_, 28.0f * tileSize_, static_cast<float>(tileSize_), static_cast<float>(tileSize_)};
    exitDoor.outsideSpawn = Vector2{35.0f * tileSize_ + 2.0f, 24.0f * tileSize_ + 2.0f};
    exitDoor.buildingId = "starter_shop";
    exitDoors_.push_back(exitDoor);
}

World::~World()
{
    if (tilesetLoaded_)
    {
        UnloadTexture(tileset_);
    }
}

void World::LoadAssets()
{
    tileset_ = LoadTexture("assets/tileset.png");
    tilesetLoaded_ = tileset_.id != 0;

    if (!tilesetLoaded_)
    {
        message_ = "Could not load assets/tileset.png. Using fallback rectangles.";
        return;
    }

    SetTextureFilter(tileset_, TEXTURE_FILTER_POINT);
    SetupAtlas();
}

void World::SetupAtlas()
{
    tiles_.grass = {Cell(0, 27), Cell(1, 27), Cell(2, 27), Cell(3, 27), Cell(4, 27)};
    tiles_.path = {Cell(0, 8), Cell(1, 8), Cell(2, 8), Cell(3, 8), Cell(4, 8)};
    tiles_.dirt = {Cell(0, 19), Cell(1, 19), Cell(2, 19), Cell(3, 19)};
    tiles_.wallTop = {Cell(0, 24), Cell(1, 24), Cell(2, 24), Cell(3, 24)};
    tiles_.wallFace = {Cell(0, 26), Cell(1, 26), Cell(2, 26), Cell(3, 26)};
    tiles_.herb = Cell(16, 17);
    tiles_.sword = Cell(18, 17);

    auto setClip = [&](DirectionalSprite &spriteSet, Direction direction, bool walking, std::initializer_list<AtlasFrame> frames, float frameDuration = 0.18f)
    {
        AnimationClip clip;
        clip.frames = frames;
        clip.frameDuration = frameDuration;
        if (walking)
        {
            spriteSet.walk[DirectionIndex(direction)] = clip;
        }
        else
        {
            spriteSet.idle[DirectionIndex(direction)] = clip;
        }
    };

    setClip(playerAtlas_, Direction::Down, false, {{Cell(0, 108), false, 0.0f}});
    setClip(playerAtlas_, Direction::Down, true, {{Cell(0, 108), false, 0.0f}, {Cell(1, 108), false, -1.0f}}, 0.16f);
    setClip(playerAtlas_, Direction::Up, false, {{Cell(6, 108), false, 0.0f}});
    setClip(playerAtlas_, Direction::Up, true, {{Cell(6, 108), false, 0.0f}, {Cell(7, 108), false, -1.0f}}, 0.16f);
    setClip(playerAtlas_, Direction::Left, false, {{Cell(10, 108), false, 0.0f}});
    setClip(playerAtlas_, Direction::Left, true, {{Cell(10, 108), false, 0.0f}, {Cell(11, 108), false, -1.0f}}, 0.16f);
    setClip(playerAtlas_, Direction::Right, false, {{Cell(10, 108), true, 0.0f}});
    setClip(playerAtlas_, Direction::Right, true, {{Cell(10, 108), true, 0.0f}, {Cell(11, 108), true, -1.0f}}, 0.16f);

    setClip(npcAtlas_, Direction::Down, false, {{Cell(12, 108), false, 0.0f}});
    setClip(npcAtlas_, Direction::Down, true, {{Cell(12, 108), false, 0.0f}, {Cell(13, 108), false, -1.0f}}, 0.18f);
    setClip(npcAtlas_, Direction::Up, false, {{Cell(18, 108), false, 0.0f}});
    setClip(npcAtlas_, Direction::Up, true, {{Cell(18, 108), false, 0.0f}, {Cell(19, 108), false, -1.0f}}, 0.18f);
    setClip(npcAtlas_, Direction::Left, false, {{Cell(14, 108), false, 0.0f}});
    setClip(npcAtlas_, Direction::Left, true, {{Cell(14, 108), false, 0.0f}, {Cell(15, 108), false, -1.0f}}, 0.18f);
    setClip(npcAtlas_, Direction::Right, false, {{Cell(14, 108), true, 0.0f}});
    setClip(npcAtlas_, Direction::Right, true, {{Cell(14, 108), true, 0.0f}, {Cell(15, 108), true, -1.0f}}, 0.18f);

    setClip(slimeAtlas_, Direction::Down, false, {{Cell(7, 105), false, 0.0f}});
    setClip(slimeAtlas_, Direction::Down, true, {{Cell(7, 105), false, 0.0f}, {Cell(8, 105), false, -1.0f}, {Cell(9, 105), false, 0.0f}}, 0.12f);
    setClip(slimeAtlas_, Direction::Up, false, {{Cell(7, 105), false, 0.0f}});
    setClip(slimeAtlas_, Direction::Up, true, {{Cell(7, 105), false, 0.0f}, {Cell(8, 105), false, -1.0f}, {Cell(9, 105), false, 0.0f}}, 0.12f);
    setClip(slimeAtlas_, Direction::Left, false, {{Cell(7, 105), false, 0.0f}});
    setClip(slimeAtlas_, Direction::Left, true, {{Cell(7, 105), false, 0.0f}, {Cell(8, 105), false, -1.0f}, {Cell(9, 105), false, 0.0f}}, 0.12f);
    setClip(slimeAtlas_, Direction::Right, false, {{Cell(7, 105), true, 0.0f}});
    setClip(slimeAtlas_, Direction::Right, true, {{Cell(7, 105), true, 0.0f}, {Cell(8, 105), true, -1.0f}, {Cell(9, 105), true, 0.0f}}, 0.12f);
}

Vector2 World::GetPlayerPosition() const
{
    return player_.position;
}

Vector2 World::GetWorldPixelSize() const
{
    const int width = map_.empty() ? 0 : static_cast<int>(map_[0].size()) * tileSize_;
    const int height = static_cast<int>(map_.size()) * tileSize_;
    return Vector2{static_cast<float>(width), static_cast<float>(height)};
}

void World::UpdateCamera(float dt)
{
    camera_.target.x += (player_.position.x + player_.size.x * 0.5f - camera_.target.x) * std::min(1.0f, dt * 8.0f);
    camera_.target.y += (player_.position.y + player_.size.y * 0.5f - camera_.target.y) * std::min(1.0f, dt * 8.0f);
    camera_.zoom += (targetZoom_ - camera_.zoom) * std::min(1.0f, dt * 10.0f);

    const Vector2 worldSize = GetWorldPixelSize();
    const float halfWidth = screenWidth_ * 0.5f / camera_.zoom;
    const float halfHeight = screenHeight_ * 0.5f / camera_.zoom;

    camera_.target.x = std::clamp(camera_.target.x, halfWidth, std::max(halfWidth, worldSize.x - halfWidth));
    camera_.target.y = std::clamp(camera_.target.y, halfHeight, std::max(halfHeight, worldSize.y - halfHeight));
}

void World::TryPickup(const std::string &itemName, int amount)
{
    InventoryItem newItem = MakeInventoryItemByName(itemName, amount);

    for (auto &item : player_.inventory)
    {
        if (item.name == newItem.name && item.stackable)
        {
            item.amount += amount;
            return;
        }
    }

    player_.inventory.push_back(newItem);
}

bool World::IsWall(Vector2 position) const
{
    int left = static_cast<int>(position.x) / tileSize_;
    int top = static_cast<int>(position.y) / tileSize_;
    int right = static_cast<int>(position.x + player_.size.x - 1) / tileSize_;
    int bottom = static_cast<int>(position.y + player_.size.y - 1) / tileSize_;

    auto blocked = [&](int x, int y)
    {
        if (y < 0 || y >= static_cast<int>(map_.size())) return true;
        if (x < 0 || x >= static_cast<int>(map_[y].size())) return true;
        return map_[y][x] == '#';
    };

    return blocked(left, top) || blocked(right, top) || blocked(left, bottom) || blocked(right, bottom);
}

bool World::IsClose(Vector2 a, Vector2 b, float distance) const
{
    const float dx = a.x - b.x;
    const float dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy) <= distance;
}

Rectangle World::Cell(int col, int row) const
{
    return Rectangle{
        static_cast<float>(col * atlasCellSize_),
        static_cast<float>(row * atlasCellSize_),
        static_cast<float>(atlasCellSize_),
        static_cast<float>(atlasCellSize_)};
}

int World::DirectionIndex(Direction direction) const
{
    return static_cast<int>(direction);
}

Direction World::DirectionFromVector(Vector2 direction) const
{
    if (std::fabs(direction.x) > std::fabs(direction.y))
    {
        return direction.x >= 0.0f ? Direction::Right : Direction::Left;
    }
    return direction.y >= 0.0f ? Direction::Down : Direction::Up;
}

const AnimationClip &World::SelectClip(const DirectionalSprite &spriteSet, Direction facing, bool walking) const
{
    const int index = DirectionIndex(facing);
    const AnimationClip &clip = walking ? spriteSet.walk[index] : spriteSet.idle[index];
    if (!clip.frames.empty())
    {
        return clip;
    }
    return spriteSet.idle[index];
}

const AtlasFrame &World::ResolveFrame(const AnimationClip &clip, float animClock) const
{
    static AtlasFrame fallback{};
    if (clip.frames.empty())
    {
        return fallback;
    }

    if (clip.frames.size() == 1 || clip.frameDuration <= 0.0f)
    {
        return clip.frames.front();
    }

    const int frameIndex = static_cast<int>(animClock / clip.frameDuration) % static_cast<int>(clip.frames.size());
    return clip.frames[frameIndex];
}

int World::TileVariantIndex(int x, int y, int count) const
{
    if (count <= 0)
    {
        return 0;
    }

    int seed = x * 73428767 ^ y * 912931;
    seed ^= (seed >> 13);
    seed *= 1274126177;
    if (seed < 0)
    {
        seed = -seed;
    }
    return seed % count;
}
