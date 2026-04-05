#include "world/World.h"
#include <algorithm>
#include <cmath>

namespace
{
    Vector2 NormalizeOrZero(Vector2 value)
    {
        float length = std::sqrt(value.x * value.x + value.y * value.y);
        if (length <= 0.0001f)
        {
            return Vector2{0.0f, 0.0f};
        }
        return Vector2{value.x / length, value.y / length};
    }
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

                questSystem_.NotifyEvent({QuestEventType::EnemyKilled, "slime", 1});

                if (questSystem_.IsQuestCompleted("main_001"))
                {
                    message_ = "Quest complete. Return to Elder Rowan.";
                }
            }
        }
    }
}

void World::HandleDrops() {
    drops_.erase(
        std::remove_if(drops_.begin(), drops_.end(), [](const Drop& drop) { return drop.taken; }),
        drops_.end()
    );
}
