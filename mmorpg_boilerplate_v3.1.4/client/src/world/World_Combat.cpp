#include "world/World.h"

#include <algorithm>
#include <cmath>

void World::HandleCombat(float)
{
    if (!IsKeyPressed(KEY_SPACE) || playerAttackTimer_ > 0.0f)
    {
        return;
    }

    playerAttackTimer_ = player_.weapon.cooldown;

    for (auto& enemy : enemies_)
    {
        if (!enemy.alive)
        {
            continue;
        }

        const float dx = enemy.position.x - player_.position.x;
        const float dy = enemy.position.y - player_.position.y;
        const float distance = std::sqrt(dx * dx + dy * dy);

        if (distance <= player_.weapon.range)
        {
            enemy.hp -= player_.weapon.damage;
            message_ = "You hit " + enemy.name + " with " + player_.weapon.name + ".";

            if (enemy.hp <= 0)
            {
                enemy.alive = false;
                player_.xp += 10;
                player_.gold += 5;
                drops_.push_back(Drop{enemy.position, "Herb", 1, false});
                message_ = "You defeated a " + enemy.name + " and found an Herb.";
                questRuntimeSystem_.NotifyEnemyKilled(questSystem_, "slime", message_);
            }
        }
    }
}
