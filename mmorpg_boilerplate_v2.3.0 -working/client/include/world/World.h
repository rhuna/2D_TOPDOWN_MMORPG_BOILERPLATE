#pragma once

#include "core/Types.h"
#include <array>
#include <string>
#include <vector>

// One drawable sprite frame pulled from a tileset atlas.
struct AtlasFrame {
    Rectangle source{}; // Source rectangle inside the atlas texture
    bool flipX = false; // Mirror frame horizontally when drawing
    float yBob = 0.0f;  // Small vertical offset used for a walk bob
};

// One animation clip, such as "walk left" or "idle down".
struct AnimationClip {
    std::vector<AtlasFrame> frames;
    float frameDuration = 0.18f;
};

// A full directional animation set for an actor type.
struct DirectionalSprite {
    std::array<AnimationClip, 4> idle;
    std::array<AnimationClip, 4> walk;
};

// A collection of commonly-used map and pickup tiles from the atlas.
struct TileAtlas {
    std::vector<Rectangle> grass;
    std::vector<Rectangle> path;
    std::vector<Rectangle> dirt;
    std::vector<Rectangle> wallTop;
    std::vector<Rectangle> wallFace;
    Rectangle herb{};
    Rectangle sword{};
};

// World owns the main gameplay simulation.
//
// Responsibilities:
// - map generation data usage
// - player/enemy/NPC state
// - combat and quest progression
// - camera update
// - minimap/world map drawing
// - sprite atlas lookup and animation
class World {
public:
    World();
    ~World();

    // Advance one frame of simulation.
    void Update(float dt);

    // Draw one complete frame.
    void Draw() const;

    // Helper accessors used by camera and future systems.
    Vector2 GetPlayerPosition() const;
    Vector2 GetWorldPixelSize() const;

private:
    // Asset and atlas setup.
    void LoadAssets();
    void SetupAtlas();

    // Core update phases.
    void UpdatePlayer(float dt);
    void UpdateEnemies(float dt);
    void HandleCombat(float dt);
    void HandleInteraction();
    void HandleDrops();

    // Camera and rendering helpers.
    void UpdateCamera(float dt);
    void DrawMap() const;
    void DrawHud() const;
    void DrawMinimap() const;
    void DrawWorldMapOverlay() const;
    void DrawPlayer() const;
    void DrawNpc(const Npc& npc) const;
    void DrawEnemy(const Enemy& enemy) const;
    void DrawAnimatedActor(const Actor& actor, const DirectionalSprite& spriteSet, Color tint) const;
    void DrawPickupSprite(Vector2 position, Rectangle source, Color tint = WHITE) const;

    // Inventory helper.
    void TryPickup(const std::string& itemName, int amount);

    // Utility helpers.
    bool IsWall(Vector2 position) const;
    bool IsClose(Vector2 a, Vector2 b, float distance) const;

    Rectangle Cell(int col, int row) const;
    int DirectionIndex(Direction direction) const;
    Direction DirectionFromVector(Vector2 direction) const;
    const AnimationClip& SelectClip(const DirectionalSprite& spriteSet, Direction facing, bool walking) const;
    const AtlasFrame& ResolveFrame(const AnimationClip& clip, float animClock) const;
    int TileVariantIndex(int x, int y, int count) const;

private:
    // Raw tile map using character symbols from BuildMap().
    std::vector<std::string> map_;

    // Camera is mutable so const drawing helpers can still adjust read-only camera state if needed.
    mutable Camera2D camera_{};

    // Cached screen and tile metrics.
    int screenWidth_ = 1024;
    int screenHeight_ = 768;
    int tileSize_ = 32;
    int atlasCellSize_ = 16;

    // UI state.
    bool showBigMap_ = false;
    float targetZoom_ = 2.0f;

    // Gameplay state.
    Player player_;
    std::vector<Enemy> enemies_;
    std::vector<Npc> npcs_;
    std::vector<Drop> drops_;
    Quest slimeQuest_;
    std::string message_;
    float playerAttackTimer_ = 0.0f;

    // Atlas texture state.
    Texture2D tileset_{};
    bool tilesetLoaded_ = false;

    // Tile palettes and sprite sets.
    TileAtlas tiles_;
    DirectionalSprite playerAtlas_;
    DirectionalSprite npcAtlas_;
    DirectionalSprite slimeAtlas_;
};
