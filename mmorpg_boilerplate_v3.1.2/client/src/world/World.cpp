#include "world/World.h" // for World class definition
#include "world/Data.h" // for BuildMap()
#include "net/NetworkClient.h" // for RemotePlayer
#include "game/QuestSystem.h" //
#include <algorithm>
#include <cmath>
#include <sstream>
#include "raylib.h" // for Vector2, Rectangle, Color, and drawing functions

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

const char *ItemCategoryLabel(ItemCategory category)
{
    switch (category)
    {
    case ItemCategory::Consumable:
        return "Consumable";
    case ItemCategory::Weapon:
        return "Weapon";
    case ItemCategory::Armor:
        return "Armor";
    case ItemCategory::Material:
        return "Material";
    case ItemCategory::Quest:
        return "Quest";
    default:
        return "Unknown";
    }
}

Color ItemCategoryColor(ItemCategory category)
{
    switch (category)
    {
    case ItemCategory::Consumable:
        return GREEN;
    case ItemCategory::Weapon:
        return ORANGE;
    case ItemCategory::Armor:
        return SKYBLUE;
    case ItemCategory::Material:
        return YELLOW;
    case ItemCategory::Quest:
        return PINK;
    default:
        return LIGHTGRAY;
    }
}

Weapon MakeWeaponForItemName(const std::string &name)
{
    if (name == "Bronze Blade")
        return Weapon{"Bronze Blade", 4, 48.0f, 0.28f};

    if (name == "Iron Sword")
        return Weapon{"Iron Sword", 6, 52.0f, 0.25f};

    return Weapon{"Rusty Sword", 2, 42.0f, 0.35f};
}

Armor MakeArmorForItemName(const std::string &name)
{
    if (name == "Leather Armor")
        return Armor{"Leather Armor", 3};

    if (name == "Chain Vest")
        return Armor{"Chain Vest", 5};

    return Armor{"Traveler Clothes", 0};
}

InventoryItem MakeInventoryItemByName(const std::string &itemName, int amount)
{
    if (itemName == "Potion")
    {
        return InventoryItem{
            "Potion",
            amount,
            ItemCategory::Consumable,
            "Restores a strong amount of health.",
            0,
            0,
            10,
            10,
            false,
            true};
    }

    if (itemName == "Herb")
    {
        return InventoryItem{
            "Herb",
            amount,
            ItemCategory::Consumable,
            "Restores a little health.",
            0,
            0,
            4,
            4,
            false,
            true};
    }

    if (itemName == "Iron Sword")
    {
        return InventoryItem{
            "Iron Sword",
            amount,
            ItemCategory::Weapon,
            "A stronger sword sold by merchants.",
            6,
            0,
            0,
            30,
            true,
            false};
    }

    if (itemName == "Bronze Blade")
    {
        return InventoryItem{
            "Bronze Blade",
            amount,
            ItemCategory::Weapon,
            "A quest reward weapon.",
            4,
            0,
            0,
            0,
            true,
            false};
    }

    if (itemName == "Leather Armor")
    {
        return InventoryItem{
            "Leather Armor",
            amount,
            ItemCategory::Armor,
            "Light armor that adds defense.",
            0,
            3,
            0,
            24,
            true,
            false};
    }

    if (itemName == "Chain Vest")
    {
        return InventoryItem{
            "Chain Vest",
            amount,
            ItemCategory::Armor,
            "Heavier armor with better protection.",
            0,
            5,
            0,
            40,
            true,
            false};
    }

    return InventoryItem{
        itemName,
        amount,
        ItemCategory::Material,
        "A generic crafting material.",
        0,
        0,
        0,
        1,
        false,
        true};
}
}

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
                    npc.shopStock = {
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
void World::Update(float dt)
{
    screenWidth_ = GetScreenWidth();
    screenHeight_ = GetScreenHeight();
    camera_.offset = Vector2{screenWidth_ * 0.5f, screenHeight_ * 0.5f};

    if (IsKeyPressed(KEY_TAB) && !IsBlockingUiOpen())
    {
        showBigMap_ = !showBigMap_;
    }

    const float wheel = GetMouseWheelMove();
    if (wheel != 0.0f && !showBigMap_ && !IsBlockingUiOpen())
    {
        targetZoom_ = std::clamp(targetZoom_ + wheel * 0.20f, 1.0f, 3.5f);
    }

    // -------------------------------------------------------------------------
    // Global popup hotkeys.
    // These keys toggle their own popup on/off.
    // Only one overlay menu is shown at a time.
    // -------------------------------------------------------------------------
    if (!choiceUi_.visible && !shopUi_.IsOpen())
    {
        if (IsKeyPressed(KEY_I))
        {
            ToggleInventoryUi();
        }

        if (IsKeyPressed(KEY_C))
        {
            ToggleEquipmentUi();
        }

        if (IsKeyPressed(KEY_Q))
        {
            ToggleQuestLogUi();
        }
        }
    }

    // Modal / blocking UIs take over the update loop.
    if (choiceUi_.visible)
    {
        UpdateChoiceUi();
        UpdateCamera(dt);
        return;
    }

    if (shopUi_.IsOpen())
    {
        UpdateShopUi();
        UpdateCamera(dt);
        return;
    }


    if (inventoryUi_.IsOpen())
    {
        UpdateInventoryUi();
        UpdateCamera(dt);
        return;
    }

    if (equipmentUi_.IsOpen())
    {
        UpdateEquipmentUi();
        UpdateCamera(dt);
        return;
    }

    if (questLogUi_.IsOpen())
    {
        UpdateQuestLogUi();
        UpdateCamera(dt);
        return;
    }

    playerAttackTimer_ -= dt;
    UpdatePlayer(dt);
    HandleBuildingTransitions();
    UpdateEnemies(dt);
    HandleCombat(dt);
    HandleInteraction();
    HandleDrops();
    UpdateCamera(dt);
}
// Draw the complete frame: world, actors, HUD, minimap, then optional world map.
void World::Draw() const
{
    BeginMode2D(camera_);
    DrawMap();

    for (const auto &npc : npcs_)
    {
        DrawNpc(npc);
    }

    for (const auto &enemy : enemies_)
    {
        if (enemy.alive)
        {
            DrawEnemy(enemy);
        }
    }

    for (const auto &drop : drops_)
    {
        if (!drop.taken)
        {
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

    if (showBigMap_)
    {
        DrawWorldMapOverlay();
    }

    if (choiceUi_.visible)
    {
        DrawChoiceUi();
    }

    if (shopUi_.IsOpen())
    {
        DrawShopUi();
    }

    if (inventoryUi_.IsOpen())
    {
        DrawInventoryUi();
    }

    if (equipmentUi_.IsOpen())
    {
        DrawEquipmentUi();
    }

    if (questLogUi_.IsOpen())
    {
        DrawQuestLogUi();
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

                questSystem_.NotifyEvent({QuestEventType::EnemyKilled, "slime", 1});

                if (questSystem_.IsQuestCompleted("main_001"))
                {
                    message_ = "Quest complete. Return to Elder Rowan.";
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

        if (!IsClose(player_.position, npc.position, 52.0f))
        {
            continue;
        }
        if (npc.isMerchant)
        {
            for (int i = 0; i < static_cast<int>(npcs_.size()); ++i)
            {
                if (&npcs_[i] == &npc)
                {
                    OpenShopUi(i);
                    message_ = npc.name + ": What would you like to buy?";
                    return;
                }
            }
        }
        if (npc.name == "Elder Rowan")
        {
            if (questSystem_.CanAcceptQuest("main_001"))
            {
                questSystem_.AcceptQuest("main_001");
                questSystem_.NotifyEvent({QuestEventType::NpcTalkedTo, "elder_rowan", 1});
                message_ = npc.name + ": Please clear out 3 slimes near the village.";
                return;
            }
            if (questSystem_.IsQuestActive("main_001"))
            {
                questSystem_.NotifyEvent({QuestEventType::NpcTalkedTo, "elder_rowan", 1});
            }
            if (questSystem_.IsAwaitingChoice("main_001"))
            {
                OpenChoiceUi("main_001");
                message_ = npc.name + ": Choose your next path.";
                return;
            }
            int rewardGold = 0;
            int rewardXp = 0;
            int rewardItemCount = 0;
            std::string rewardItemId;
            if (questSystem_.RewardQuest("main_001", rewardGold, rewardXp, rewardItemId, rewardItemCount))
            {
                player_.gold += rewardGold;
                player_.xp += rewardXp;
                if (rewardItemId == "bronze_blade")
                {
                    // Add the reward item to inventory and equip it immediately.
                    TryPickup("Bronze Blade", 1);
                    player_.weapon = MakeWeaponForItemName("Bronze Blade");
                }
                message_ = npc.name + ": You have earned your reward.";
                return;
            }
            if (questSystem_.CanAcceptQuest("main_002") && questSystem_.HasFlag("path_crossroads"))
            {
                questSystem_.AcceptQuest("main_002");
                questSystem_.NotifyEvent({QuestEventType::NpcTalkedTo, "elder_rowan", 1});
                message_ = npc.name + ": Head to the crossroads and find the Wanderer.";
                return;
            }
            if (questSystem_.CanAcceptQuest("side_001") && questSystem_.HasFlag("path_healer"))
            {
                questSystem_.AcceptQuest("side_001");
                questSystem_.NotifyEvent({QuestEventType::NpcTalkedTo, "elder_rowan", 1});
                message_ = npc.name + ": Gather 3 herbs for the healer.";
                return;
            }
            if (questSystem_.IsQuestActive("side_001"))
            {
                questSystem_.NotifyEvent({QuestEventType::NpcTalkedTo, "elder_rowan", 1});
                if (questSystem_.RewardQuest("side_001", rewardGold, rewardXp, rewardItemId, rewardItemCount))
                {
                    player_.gold += rewardGold;
                    player_.xp += rewardXp;
                    message_ = npc.name + ": Thank you for helping the healer.";
                    return;
                }
            }
            message_ = npc.name + " (" + npc.regionName + "): " + npc.idleText;
            return;
        }
        if (npc.name == "Wanderer")
        {
            if (questSystem_.IsQuestActive("main_002"))
            {
                questSystem_.NotifyEvent({QuestEventType::NpcTalkedTo, "wanderer", 1});
            }
            int rewardGold = 0;
            int rewardXp = 0;
            int rewardItemCount = 0;
            std::string rewardItemId;
            if (questSystem_.RewardQuest("main_002", rewardGold, rewardXp, rewardItemId, rewardItemCount))
            {
                player_.gold += rewardGold;
                player_.xp += rewardXp;
                message_ = npc.name + ": Welcome to the crossroads.";
                return;
            }
            message_ = npc.name + " (" + npc.regionName + "): " + npc.idleText;
            return;
        }
    }

    for (auto& drop : drops_) {
        if (!drop.taken && IsClose(player_.position, drop.position, 40.0f)) {
            drop.taken = true;
            TryPickup(drop.itemName, drop.amount);
            if (drop.itemName == "Herb")
            {
                questSystem_.NotifyEvent({QuestEventType::ItemCollected, "herb", drop.amount});
            }
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
            const bool isDoor = (tile == 'B' || tile == 'D');

            if (!tilesetLoaded_) {
                DrawRectangleRec(destination, isWall ? DARKGRAY : Color{30, 80, 35, 255});
                DrawRectangleLinesEx(destination, 1.0f, Fade(BLACK, 0.18f));
                continue;
            }

            Rectangle source{};
            if (isWall)
            {
                bool isTopEdge = (y == 0 || map_[y - 1][x] != '#');
                const std::vector<Rectangle> &palette = isTopEdge ? tiles_.wallTop : tiles_.wallFace;
                source = palette[TileVariantIndex(x, y, static_cast<int>(palette.size()))];
            }
            else if (isDoor)
            {
                source = tiles_.path[0];
            }
            else if (tile == 'p')
            {
                source = tiles_.path[TileVariantIndex(x, y, static_cast<int>(tiles_.path.size()))];
            }
            else
            {
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
void World::DrawHud() const
{
    const int hudHeight = 128;
    DrawRectangle(0, screenHeight_ - hudHeight, screenWidth_, hudHeight, Fade(BLACK, 0.75f));

    if (tilesetLoaded_)
    {
        Rectangle swordDestination{16.0f, static_cast<float>(screenHeight_ - 118), 24.0f, 24.0f};
        DrawTexturePro(tileset_, tiles_.sword, swordDestination, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
    }

    std::ostringstream stats;
    stats << "HP: " << player_.hp << "/" << player_.maxHp
          << "   XP: " << player_.xp
          << "   Gold: " << player_.gold
          << "   Weapon: " << player_.weapon.name
          << " (DMG " << player_.weapon.damage << ")"
          << "   Armor: " << player_.armor.name
          << " (DEF " << player_.armor.defense << ")";
    DrawText(stats.str().c_str(), 48, screenHeight_ - 116, 20, WHITE);

    std::ostringstream quest;
    auto activeQuests = questSystem_.GetActiveQuests();

    if (!activeQuests.empty())
    {
        const QuestState *state = activeQuests.front();
        const QuestDefinition *def = questSystem_.GetDefinition(state->questId);

        if (def)
        {
            quest << "Quest: " << def->title;

            if (state->status == QuestStatus::AwaitingChoice)
            {
                quest << " - Choose [1] or [2]";
            }
            else if (state->currentStageIndex >= 0 &&
                     state->currentStageIndex < static_cast<int>(def->stages.size()))
            {
                const auto &stageDef = def->stages[state->currentStageIndex];
                const auto &stageState = state->stages[state->currentStageIndex];

                quest << " - " << stageDef.description;

                if (!stageDef.objectives.empty() && !stageState.objectives.empty())
                {
                    quest << " ["
                          << stageState.objectives[0].currentCount
                          << "/"
                          << stageDef.objectives[0].requiredCount
                          << "]";
                }
            }
        }
    }
    else
    {
        quest << "Quest: No active quests.";
    }

    DrawText(quest.str().c_str(), 16, screenHeight_ - 88, 20, YELLOW);

    std::ostringstream inventory;
    inventory << "Inventory: ";
    for (std::size_t i = 0; i < player_.inventory.size(); ++i)
    {
        inventory << player_.inventory[i].name << " x" << player_.inventory[i].count;
        if (i + 1 < player_.inventory.size())
        {
            inventory << ", ";
        }
    }

    DrawText(inventory.str().c_str(), 16, screenHeight_ - 60, 20, SKYBLUE);

    const char *controls = showBigMap_
                               ? "TAB closes map | I inventory | C equipment | Q quests"
                               : "Mouse wheel zooms | TAB map | I inventory | C equipment | Q quests";

    DrawText(controls, screenWidth_ - 470, screenHeight_ - 32, 18, LIGHTGRAY);
    DrawText(message_.c_str(), 16, screenHeight_ - 32, 18, LIGHTGRAY);
}

void World::OpenChoiceUi(const std::string &questId)
{
    const QuestDefinition *def = questSystem_.GetDefinition(questId);
    if (!def || def->branchChoices.empty())
        return;

    choiceUi_.visible = true;
    choiceUi_.questId = questId;
    choiceUi_.choiceIds.clear();
    choiceUi_.choiceTexts.clear();
    choiceUi_.selectedIndex = 0;

    for (const auto &choice : def->branchChoices)
    {
        choiceUi_.choiceIds.push_back(choice.id);
        choiceUi_.choiceTexts.push_back(choice.text);
    }
}

void World::CloseChoiceUi()
{
    choiceUi_.visible = false;
    choiceUi_.questId.clear();
    choiceUi_.choiceIds.clear();
    choiceUi_.choiceTexts.clear();
    choiceUi_.selectedIndex = 0;
}

void World::UpdateChoiceUi()
{
    if (!choiceUi_.visible)
        return;

    if (IsKeyPressed(KEY_ESCAPE))
    {
        CloseChoiceUi();
        message_ = "Choice cancelled.";
        return;
    }

    if (IsKeyPressed(KEY_UP))
    {
        if (!choiceUi_.choiceTexts.empty())
        {
            choiceUi_.selectedIndex--;
            if (choiceUi_.selectedIndex < 0)
                choiceUi_.selectedIndex = static_cast<int>(choiceUi_.choiceTexts.size()) - 1;
        }
    }

    if (IsKeyPressed(KEY_DOWN))
    {
        if (!choiceUi_.choiceTexts.empty())
        {
            choiceUi_.selectedIndex++;
            if (choiceUi_.selectedIndex >= static_cast<int>(choiceUi_.choiceTexts.size()))
                choiceUi_.selectedIndex = 0;
        }
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        if (choiceUi_.selectedIndex >= 0 &&
            choiceUi_.selectedIndex < static_cast<int>(choiceUi_.choiceIds.size()))
        {
            const std::string chosenId = choiceUi_.choiceIds[choiceUi_.selectedIndex];

            if (questSystem_.ChooseBranch(choiceUi_.questId, chosenId))
            {
                const std::string chosenText = choiceUi_.choiceTexts[choiceUi_.selectedIndex];
                message_ = "Choice made: " + chosenText;
            }
            else
            {
                message_ = "Could not apply choice.";
            }

            CloseChoiceUi();
        }
    }
}

void World::DrawChoiceUi() const
{
    if (!choiceUi_.visible)
        return;

    DrawRectangle(0, 0, screenWidth_, screenHeight_, Fade(BLACK, 0.55f));

    const int boxW = 620;
    const int boxH = 240;
    const int boxX = (screenWidth_ - boxW) / 2;
    const int boxY = (screenHeight_ - boxH) / 2;

    DrawRectangle(boxX, boxY, boxW, boxH, Fade(BLACK, 0.90f));
    DrawRectangleLines(boxX, boxY, boxW, boxH, WHITE);

    DrawText("Choose Your Path", boxX + 20, boxY + 18, 28, YELLOW);
    DrawText("Use Up/Down to select, Enter to confirm, Esc to cancel",
             boxX + 20, boxY + 52, 18, LIGHTGRAY);

    int y = boxY + 95;
    for (int i = 0; i < static_cast<int>(choiceUi_.choiceTexts.size()); ++i)
    {
        const bool selected = (i == choiceUi_.selectedIndex);

        if (selected)
        {
            DrawRectangle(boxX + 16, y - 4, boxW - 32, 30, Fade(SKYBLUE, 0.25f));
            DrawRectangleLines(boxX + 16, y - 4, boxW - 32, 30, SKYBLUE);
        }

        std::string line = std::to_string(i + 1) + ". " + choiceUi_.choiceTexts[i];
        DrawText(line.c_str(), boxX + 28, y, 22, selected ? WHITE : LIGHTGRAY);
        y += 38;
    }
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
void World::TryPickup(const std::string &itemName, int amount)
{
    InventoryItem newItem = MakeInventoryItemByName(itemName, amount);

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

// Check for player overlap with building door triggers and move them inside/outside.
void World::HandleBuildingTransitions()
{
    Rectangle playerRect{
        player_.position.x,
        player_.position.y,
        player_.size.x,
        player_.size.y};

    for (const auto &door : buildingDoors_)
    {
        if (CheckCollisionRecs(playerRect, door.trigger) && IsKeyPressed(KEY_E))
        {
            player_.position = door.insideSpawn;
            message_ = "You entered the building.";
            return;
        }
    }

    for (const auto &exitDoor : exitDoors_)
    {
        if (CheckCollisionRecs(playerRect, exitDoor.trigger) && IsKeyPressed(KEY_E))
        {
            player_.position = exitDoor.outsideSpawn;
            message_ = "You left the building.";
            return;
        }
    }
}


/////////////////////////////////////////////////////////////
/////////////// uI: Inventory, shop, inventory, choices   //
////////////////////////////////////////////////////////////

// shop UI management: open/close, input handling, and drawing.
void World::OpenShopUi(int merchantIndex)
{
    CloseAllOverlayUi();
    shopUi_.OpenForMerchant(merchantIndex);
}

void World::CloseShopUi()
{
    shopUi_.Close();
}
void World::UpdateShopUi()
{
    if (!shopUi_.IsOpen())
        return;

    if (IsKeyPressed(KEY_ESCAPE))
    {
        shopUi_.Close();
        return;
    }

    // Replace 3 with actual merchant item count later
    constexpr int itemCount = 3;

    if (IsKeyPressed(KEY_UP))
    {
        shopUi_.MoveSelection(-1, itemCount);
    }

    if (IsKeyPressed(KEY_DOWN))
    {
        shopUi_.MoveSelection(1, itemCount);
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        message_ = "Shop purchase logic not fully implemented yet.";
    }
}
void World::DrawShopUi() const
{
    if (!shopUi_.IsOpen())
        return;

    const int boxW = 420;
    const int boxH = 260;
    const int boxX = (screenWidth_ - boxW) / 2;
    const int boxY = (screenHeight_ - boxH) / 2;

    DrawRectangle(boxX, boxY, boxW, boxH, Fade(BLACK, 0.90f));
    DrawRectangleLines(boxX, boxY, boxW, boxH, WHITE);

    DrawText("Shop", boxX + 20, boxY + 18, 28, YELLOW);
    DrawText("Enter = buy, Esc = close", boxX + 20, boxY + 52, 18, LIGHTGRAY);

    const char *items[] = {"Potion - 10g", "Herb - 5g", "Bronze Blade - 25g"};

    int y = boxY + 95;
    for (int i = 0; i < 3; ++i)
    {
        const bool selected = (i == shopUi_.SelectedIndex());

        if (selected)
        {
            DrawRectangle(boxX + 16, y - 4, boxW - 32, 30, Fade(SKYBLUE, 0.25f));
            DrawRectangleLines(boxX + 16, y - 4, boxW - 32, 30, SKYBLUE);
        }

        DrawText(items[i], boxX + 28, y, 22, selected ? WHITE : LIGHTGRAY);
        y += 38;
    }
}




// inventory UI management: open/close, input handling, and drawing.
void World::UpdateInventoryUi()
{
    if (!inventoryUi_.IsOpen())
        return;

    if (IsKeyPressed(KEY_ESCAPE))
    {
        inventoryUi_.Close();
        return;
    }

    const int itemCount = static_cast<int>(player_.inventory.size());

    if (IsKeyPressed(KEY_UP))
    {
        inventoryUi_.MoveSelection(-1, itemCount);
    }

    if (IsKeyPressed(KEY_DOWN))
    {
        inventoryUi_.MoveSelection(1, itemCount);
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        if (itemCount > 0)
        {
            message_ = "Use/drop inventory item logic goes here.";
        }
    }
}

void World::DrawInventoryUi() const
{
{
    if (!inventoryUi_.IsOpen())
        return;

    const int boxW = 460;
    const int boxH = 300;
    const int boxX = (screenWidth_ - boxW) / 2;
    const int boxY = (screenHeight_ - boxH) / 2;

    DrawRectangle(boxX, boxY, boxW, boxH, Fade(BLACK, 0.90f));
    DrawRectangleLines(boxX, boxY, boxW, boxH, WHITE);

    DrawText("Inventory", boxX + 20, boxY + 18, 28, YELLOW);
    DrawText("I / Esc = close, Enter = use", boxX + 20, boxY + 52, 18, LIGHTGRAY);

    int y = boxY + 95;

    if (player_.inventory.empty())
    {
        DrawText("Inventory is empty.", boxX + 28, y, 22, LIGHTGRAY);
        return;
    }

    for (int i = 0; i < static_cast<int>(player_.inventory.size()); ++i)
    {
        const bool selected = (i == inventoryUi_.SelectedIndex());

        if (selected)
        {
            DrawRectangle(boxX + 16, y - 4, boxW - 32, 30, Fade(SKYBLUE, 0.25f));
            DrawRectangleLines(boxX + 16, y - 4, boxW - 32, 30, SKYBLUE);
        }

        const auto &item = player_.inventory[i];
        DrawText(item.name.c_str(), boxX + 28, y, 22, selected ? WHITE : LIGHTGRAY);
        y += 38;
    }
}


// equipment UI management: open/close, input handling, and drawing.

void World::UpdateEquipmentUi()
{
    if (!equipmentUi_.IsOpen())
        return;

    if (IsKeyPressed(KEY_ESCAPE))
    {
        equipmentUi_.Close();
        return;
    }

    constexpr int slotCount = 2; // weapon, armor

    if (IsKeyPressed(KEY_UP))
    {
        equipmentUi_.MoveSelection(-1, slotCount);
    }

    if (IsKeyPressed(KEY_DOWN))
    {
        equipmentUi_.MoveSelection(1, slotCount);
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        message_ = "Equipment management logic goes here.";
    }
}

void World::DrawEquipmentUi() const
{
    if (!equipmentUi_.IsOpen())
    {
        return;
    }

    const int x = 20;
    const int y = 80;
    const int w = 260;
    const int h = 170;

    DrawRectangle(x, y, w, h, Fade(BLACK, 0.80f));
    DrawRectangleLines(x, y, w, h, WHITE);

    DrawText("Equipment", x + 12, y + 10, 24, YELLOW);

    DrawText("Weapon:", x + 12, y + 55, 20, LIGHTGRAY);
    DrawText("weapon", x + 110, y + 55, 20, WHITE);

    DrawText("Armor:", x + 12, y + 90, 20, LIGHTGRAY);
    DrawText("armor", x + 110, y + 90, 20, WHITE);

    DrawText("Press C to close", x + 12, y + 135, 18, GRAY);
}

// quest log UI management: open/close, input handling, and drawing.

void World::UpdateQuestLogUi()
{
    if (!questLogUi_.IsOpen())
        return;

    if (IsKeyPressed(KEY_ESCAPE))
    {
        questLogUi_.Close();
        return;
    }

    auto activeQuests = questSystem_.GetActiveQuests();
    const int questCount = static_cast<int>(activeQuests.size());

    if (IsKeyPressed(KEY_UP))
    {
        questLogUi_.MoveSelection(-1, questCount);
    }

    if (IsKeyPressed(KEY_DOWN))
    {
        questLogUi_.MoveSelection(1, questCount);
    }
}

void World::DrawQuestLogUi() const
{
    if (!questLogUi_.IsOpen())
        return;

    const int boxW = 540;
    const int boxH = 320;
    const int boxX = (screenWidth_ - boxW) / 2;
    const int boxY = (screenHeight_ - boxH) / 2;

    DrawRectangle(boxX, boxY, boxW, boxH, Fade(BLACK, 0.92f));
    DrawRectangleLines(boxX, boxY, boxW, boxH, WHITE);

    DrawText("Quest Log", boxX + 20, boxY + 18, 28, YELLOW);
    DrawText("Q / Esc = close", boxX + 20, boxY + 52, 18, LIGHTGRAY);

    auto activeQuests = questSystem_.GetActiveQuests();

    int y = boxY + 95;

    if (activeQuests.empty())
    {
        DrawText("No active quests.", boxX + 28, y, 22, LIGHTGRAY);
        return;
    }

    for (int i = 0; i < static_cast<int>(activeQuests.size()); ++i)
    {
        const bool selected = (i == questLogUi_.SelectedIndex());
        const QuestState *state = activeQuests[i];
        const QuestDefinition *def = questSystem_.GetDefinition(state->questId);

        if (!def)
            continue;

        if (selected)
        {
            DrawRectangle(boxX + 16, y - 4, boxW - 32, 30, Fade(SKYBLUE, 0.25f));
            DrawRectangleLines(boxX + 16, y - 4, boxW - 32, 30, SKYBLUE);
        }

        DrawText(def->title.c_str(), boxX + 28, y, 22, selected ? WHITE : LIGHTGRAY);
        y += 38;
    }

    const int selected = questLogUi_.SelectedIndex();
    if (selected >= 0 && selected < static_cast<int>(activeQuests.size()))
    {
        const QuestState *state = activeQuests[selected];
        const QuestDefinition *def = questSystem_.GetDefinition(state->questId);

        if (def)
        {
            DrawText(def->description.c_str(), boxX + 20, boxY + boxH - 70, 18, LIGHTGRAY);
        }
    }
}

bool World::IsBlockingUiOpen() const
{
    return dialogueUi_.IsOpen() ||
           choiceUi_.visible ||
           shopUi_.IsOpen() ||
           inventoryUi_.IsOpen() ||
           equipmentUi_.IsOpen() ||
           questLogUi_.IsOpen();
}
void World::CloseAllOverlayUi()
{
    inventoryUi_.Close();
    questLogUi_.Close();
    equipmentUi_.Close();
    shopUi_.Close();
    CloseChoiceUi();
    dialogueUi_.Close();
}




////////////////////////////////////////////////////////////
/// Toggle functions for each UI that close all other
/// UIs when opening to ensure only one is open at a time.
////////////////////////////////////////////////////////////
void World::ToggleInventoryUi()
{
    const bool shouldOpen = !inventoryUi_.IsOpen();
    CloseAllOverlayUi();

    if (shouldOpen)
    {
        inventoryUi_.Open();
    }
}

void World::ToggleEquipmentUi()
{
    const bool shouldOpen = !equipmentUi_.IsOpen();
    CloseAllOverlayUi();

    if (shouldOpen)
    {
        equipmentUi_.Open();
    }
}

void World::ToggleQuestLogUi()
{
    const bool shouldOpen = !questLogUi_.IsOpen();
    CloseAllOverlayUi();

    if (shouldOpen)
    {
        questLogUi_.Open();
    }
}