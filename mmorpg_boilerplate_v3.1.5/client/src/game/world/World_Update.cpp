#include "game/world/World.h" // for World class definition

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
}


void World::Update(float dt)
{
    screenWidth_ = GetScreenWidth();
    screenHeight_ = GetScreenHeight();
    camera_.offset = Vector2{
        screenWidth_ * 0.5f,
        screenHeight_ * 0.5f};

    if (IsKeyPressed(KEY_TAB) && !IsBlockingUiOpen())
    {
        showBigMap_ = !showBigMap_;
    }

    const float wheel = GetMouseWheelMove();
    if (wheel != 0.0f && !showBigMap_ && !IsBlockingUiOpen())
    {
        targetZoom_ = std::clamp(targetZoom_ + wheel * 0.20f, 1.0f, 3.5f);
    }

    if (!choiceUi_.visible && !shopUi_.IsOpen() && !IsBlockingUiOpen())
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

// Update player input, movement, facing, animation clock, and wall collision.
void World::UpdatePlayer(float dt)
{
    Vector2 move{};

    if (IsKeyDown(KEY_W))
        move.y -= 1.0f;
    if (IsKeyDown(KEY_S))
        move.y += 1.0f;
    if (IsKeyDown(KEY_A))
        move.x -= 1.0f;
    if (IsKeyDown(KEY_D))
        move.x += 1.0f;

    move = NormalizeOrZero(move);
    player_.moveIntent = move;

    if (move.x != 0.0f || move.y != 0.0f)
    {
        player_.facing = DirectionFromVector(move);
        player_.animClock += dt;
    }
    else
    {
        player_.animClock = 0.0f;
    }

    Vector2 next = player_.position;
    next.x += move.x * player_.speed * dt;
    next.y += move.y * player_.speed * dt;

    if (!IsWall(Vector2{next.x, player_.position.y}))
    {
        player_.position.x = next.x;
    }
    if (!IsWall(Vector2{player_.position.x, next.y}))
    {
        player_.position.y = next.y;
    }
}

// Simple enemy AI: chase the player when nearby and attack in melee range.
void World::UpdateEnemies(float dt)
{
    for (auto &enemy : enemies_)
    {
        if (!enemy.alive)
        {
            continue;
        }

        enemy.attackTimer -= dt;
        enemy.moveIntent = Vector2{0.0f, 0.0f};

        Vector2 toPlayer{player_.position.x - enemy.position.x, player_.position.y - enemy.position.y};
        float distance = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);

        if (distance > 28.0f && distance < 220.0f)
        {
            Vector2 direction = NormalizeOrZero(toPlayer);
            Vector2 next = enemy.position;
            next.x += direction.x * enemy.speed * dt;
            next.y += direction.y * enemy.speed * dt;

            enemy.moveIntent = direction;
            enemy.facing = DirectionFromVector(direction);
            enemy.animClock += dt;

            if (!IsWall(next))
            {
                enemy.position = next;
            }
        }
        else
        {
            enemy.animClock = 0.0f;
        }

        if (distance <= 34.0f && enemy.attackTimer <= 0.0f)
        {
            player_.hp = std::max(0, player_.hp - 1);
            message_ = enemy.name + " hits you for 1 damage.";
            enemy.attackTimer = enemy.attackCooldown;
        }
    }
}
