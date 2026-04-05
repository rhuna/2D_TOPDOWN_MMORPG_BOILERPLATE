#pragma once

#include "core/Types.h"
#include "game/QuestSystem.h"
#include "gameplay/InventorySystem.h"
#include <functional>
#include <string>

class QuestRuntimeSystem
{
public:
    bool HandleNpcInteraction(QuestSystem& questSystem,
                              Player& player,
                              const Npc& npc,
                              const InventorySystem& inventorySystem,
                              std::function<void(const std::string&)> openChoiceUi,
                              std::string& outMessage) const;

    void NotifyEnemyKilled(QuestSystem& questSystem,
                           const std::string& enemyId,
                           std::string& ioMessage) const;

    void NotifyItemCollected(QuestSystem& questSystem,
                             const std::string& itemId,
                             int amount) const;
};
