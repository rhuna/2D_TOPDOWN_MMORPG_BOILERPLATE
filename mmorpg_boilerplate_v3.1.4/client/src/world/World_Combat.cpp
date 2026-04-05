#include "world/World.h"

#include <algorithm>
#include <cmath>

namespace
{
Weapon MakeWeaponForItemName(const std::string& name)
{
    if (name == "Bronze Blade")
    {
        return Weapon{"Bronze Blade", 4, 48.0f, 0.28f};
    }
    if (name == "Iron Sword")
    {
        return Weapon{"Iron Sword", 6, 52.0f, 0.25f};
    }
    return Weapon{"Rusty Sword", 2, 42.0f, 0.35f};
}
} // namespace

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

