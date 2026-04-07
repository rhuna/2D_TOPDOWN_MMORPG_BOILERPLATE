
#include "game/world/World.h"
#include "raylib.h"
#include <algorithm>

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

    if (!choiceUi_.visible && !shopUi_.IsOpen() && !dialogueUi_.IsOpen())
    {
        if (IsKeyPressed(KEY_I)) ToggleInventoryUi();
        if (IsKeyPressed(KEY_C)) ToggleEquipmentUi();
        if (IsKeyPressed(KEY_Q)) ToggleQuestLogUi();
    }

    if (choiceUi_.visible) { UpdateChoiceUi(); UpdateCamera(dt); return; }
    if (shopUi_.IsOpen()) { UpdateShopUi(); UpdateCamera(dt); return; }
    if (inventoryUi_.IsOpen()) { UpdateInventoryUi(); UpdateCamera(dt); return; }
    if (equipmentUi_.IsOpen()) { UpdateEquipmentUi(); UpdateCamera(dt); return; }
    if (questLogUi_.IsOpen()) { UpdateQuestLogUi(); UpdateCamera(dt); return; }

    playerAttackTimer_ -= dt;
    UpdatePlayer(dt);
    HandleBuildingTransitions();
    UpdateEnemies(dt);
    HandleCombat(dt);
    HandleInteraction();
    HandleDrops();
    UpdateCamera(dt);
}
