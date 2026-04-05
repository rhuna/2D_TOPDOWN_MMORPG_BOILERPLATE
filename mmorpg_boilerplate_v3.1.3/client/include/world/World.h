#pragma once

#include "net/NetworkClient.h" // for RemotePlayer
// Forward declare UI classes to avoid circular dependencies.
#include "core/Types.h" // for Actor, Npc, Enemy, Vector2, Color, Direction
#include "raylib.h" // for Rectangle
#include "game/QuestSystem.h" // for QuestSystem
#include "ui/EquipmentUI.h" // for EquipmentUI
#include "ui/InventoryUI.h" // for InventoryUI
#include "ui/QuestUI.h" // for QuestUI
#include "ui/ShopUI.h"  // for ShopUI
#include "ui/DialogueUI.h"  // for DialogueUI

#include <vector>
#include <array>
#include <string>
#include <unordered_map>
#include <vector>



struct AtlasFrame {
    Rectangle source{};
    bool flipX = false;
    float yBob = 0.0f;
};

struct AnimationClip {
    std::vector<AtlasFrame> frames;
    float frameDuration = 0.18f;
};

struct DirectionalSprite {
    std::array<AnimationClip, 4> idle;
    std::array<AnimationClip, 4> walk;
};

struct TileAtlas {
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

class World {
public:
    World();
    ~World();
    void Update(float dt);
    void Draw() const;
    void UpdateRemotePlayers(const std::unordered_map<int, RemotePlayer> &snapshots, int localId, float dt);
    Vector2 GetPlayerPosition() const;
    Vector2 GetWorldPixelSize() const;
    // Lets Game ask whether a modal / blocking world UI is open.
    // This is needed so Enter won't also open chat while a menu is active.
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
    void OpenChoiceUi(const std::string &questId);
    void CloseChoiceUi();
    void DrawChoiceUi() const;
    void HandleBuildingTransitions();
    
    private:

    // Core game loop methods:
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


    // Helper methods for animation and tile variation:
    Rectangle Cell(int col, int row) const;
    int DirectionIndex(Direction direction) const;
    Direction DirectionFromVector(Vector2 direction) const;
    const AnimationClip& SelectClip(const DirectionalSprite& spriteSet, Direction facing, bool walking) const;
    const AtlasFrame& ResolveFrame(const AnimationClip& clip, float animClock) const;
    int TileVariantIndex(int x, int y, int count) const;


    //bool IsBlockingUiOpen() const;
    void CloseAllOverlayUi();

     // UI toggle methods - these are called by Game when the corresponding key is pressed.
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
    std::string message_;
    float playerAttackTimer_ = 0.0f;
    Texture2D tileset_{};
    bool tilesetLoaded_ = false;
    TileAtlas tiles_;
    DirectionalSprite playerAtlas_;
    DirectionalSprite npcAtlas_;
    DirectionalSprite slimeAtlas_;
    // UI states:
    ChoiceUiState choiceUi_;
    std::vector<BuildingDoor> buildingDoors_;
    std::vector<ExitDoor> exitDoors_;

    DialogueUI dialogueUi_;
    InventoryUI inventoryUi_;
    EquipmentUI equipmentUi_;
    QuestUI questLogUi_;
    ShopUI shopUi_;
};
