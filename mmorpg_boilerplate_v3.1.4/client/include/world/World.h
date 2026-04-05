#pragma once

#include "core/Types.h"
#include "game/QuestSystem.h"
#include "gameplay/InventorySystem.h"
#include "gameplay/QuestRuntimeSystem.h"
#include "gameplay/ShopSystem.h"
#include "net/NetworkClient.h"
#include "raylib.h"
#include "ui/DialogueUI.h"
#include "ui/EquipmentUI.h"
#include "ui/InventoryUI.h"
#include "ui/QuestUI.h"
#include "ui/ShopUI.h"

#include <array>
#include <string>
#include <unordered_map>
#include <vector>

struct AtlasFrame
{
    Rectangle source{};
    bool flipX = false;
    float yBob = 0.0f;
};

struct AnimationClip
{
    std::vector<AtlasFrame> frames;
    float frameDuration = 0.18f;
};

struct DirectionalSprite
{
    std::array<AnimationClip, 4> idle;
    std::array<AnimationClip, 4> walk;
};

struct TileAtlas
{
    std::vector<Rectangle> grass;
    std::vector<Rectangle> path;
    std::vector<Rectangle> dirt;
    std::vector<Rectangle> wallTop;
    std::vector<Rectangle> wallFace;
    Rectangle herb{};
    Rectangle sword{};
};

struct ChoiceUiState
{
    bool visible = false;
    std::string questId;
    std::vector<std::string> choiceIds;
    std::vector<std::string> choiceTexts;
    int selectedIndex = 0;
};

struct RemoteSnapshot;

class World
{
public:
    World();
    ~World();

    void Update(float dt);
    void Draw() const;

    void UpdateRemotePlayers(const std::unordered_map<int, RemotePlayer>& snapshots, int localId, float dt);
    Vector2 GetPlayerPosition() const;
    Vector2 GetWorldPixelSize() const;
    bool IsBlockingUiOpen() const;

    void OpenShopUi(int merchantIndex);
    void CloseShopUi();
    void UpdateShopUi();
    void DrawShopUi() const;

    void UpdateInventoryUi();
    void DrawInventoryUi() const;

    void UpdateEquipmentUi();
    void DrawEquipmentUi() const;

    void UpdateQuestLogUi();
    void DrawQuestLogUi() const;

    void UpdateChoiceUi();
    void OpenChoiceUi(const std::string& questId);
    void CloseChoiceUi();
    void DrawChoiceUi() const;

    void HandleBuildingTransitions();

private:
    void LoadAssets();
    void SetupAtlas();

    void UpdatePlayer(float dt);
    void UpdateEnemies(float dt);
    void HandleCombat(float dt);
    void HandleInteraction();
    void HandleDrops();
    void UpdateCamera(float dt);

    void DrawMap() const;
    void DrawHud() const;
    void DrawMinimap() const;
    void DrawWorldMapOverlay() const;
    void DrawPlayer() const;
    void DrawNpc(const Npc& npc) const;
    void DrawEnemy(const Enemy& enemy) const;
    void DrawRemotePlayer(const RemoteActor& remote) const;
    void DrawAnimatedActor(const Actor& actor, const DirectionalSprite& spriteSet, Color tint) const;
    void DrawPickupSprite(Vector2 position, Rectangle source, Color tint = WHITE) const;

    void TryPickup(const std::string& itemName, int amount);
    bool IsWall(Vector2 position) const;
    bool IsClose(Vector2 a, Vector2 b, float distance) const;

    Rectangle Cell(int col, int row) const;
    int DirectionIndex(Direction direction) const;
    Direction DirectionFromVector(Vector2 direction) const;
    const AnimationClip& SelectClip(const DirectionalSprite& spriteSet, Direction facing, bool walking) const;
    const AtlasFrame& ResolveFrame(const AnimationClip& clip, float animClock) const;
    int TileVariantIndex(int x, int y, int count) const;

    void CloseAllOverlayUi();
    void ToggleInventoryUi();
    void ToggleEquipmentUi();
    void ToggleQuestLogUi();

private:
    std::vector<std::string> map_;
    mutable Camera2D camera_{};
    int screenWidth_ = 1024;
    int screenHeight_ = 768;
    int tileSize_ = 32;
    int atlasCellSize_ = 16;
    bool showBigMap_ = false;
    float targetZoom_ = 2.0f;

    Player player_;
    std::vector<Enemy> enemies_;
    std::vector<Npc> npcs_;
    std::vector<Drop> drops_;
    std::vector<RemoteActor> remotePlayers_;

    QuestSystem questSystem_;
    InventorySystem inventorySystem_;
    ShopSystem shopSystem_;
    QuestRuntimeSystem questRuntimeSystem_;
    std::string message_;
    float playerAttackTimer_ = 0.0f;

    Texture2D tileset_{};
    bool tilesetLoaded_ = false;
    TileAtlas tiles_;
    DirectionalSprite playerAtlas_;
    DirectionalSprite npcAtlas_;
    DirectionalSprite slimeAtlas_;

    ChoiceUiState choiceUi_;
    std::vector<BuildingDoor> buildingDoors_;
    std::vector<ExitDoor> exitDoors_;

    DialogueUI dialogueUi_;
    InventoryUI inventoryUi_;
    EquipmentUI equipmentUi_;
    QuestUI questLogUi_;
    ShopUI shopUi_;
};
