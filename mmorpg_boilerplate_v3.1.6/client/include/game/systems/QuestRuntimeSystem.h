#pragma once

#include "core/Types.h"
#include "game/quests/QuestSystem.h"
#include "game/systems/InventorySystem.h"
#include <string>
#include <functional>

namespace gameplay
{
    class QuestRuntimeSystem
    {
    public:
        bool TryAcceptFromNpc(QuestSystem& quests, const Npc& npc, std::string& outMessage) const;
        bool NotifyNpcTalk(QuestSystem& quests, const Npc& npc) const;
        bool RewardQuestIfPossible(QuestSystem& quests, Player& player, const Npc& npc, std::string& outRewardMessage, InventoryItem* outRewardItem = nullptr) const;
        void NotifyEnemyKilled(QuestSystem& quests, const std::string& enemyType, std::string& outMessage) const;
        bool HandleNpcInteraction(QuestSystem& quests, Player& player, const Npc& npc, InventorySystem& inventory, std::function<void(const std::string&)> openChoiceUiCallback, std::string& outMessage) const;
        void NotifyItemCollected(QuestSystem& quests, const std::string& itemId, int amount) const;
    };
}
