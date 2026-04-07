#pragma once

#include "core/Types.h"
#include "game/QuestSystem.h"
#include <string>
#include <functional>

namespace gameplay
{
    class InventorySystem;
    class QuestRuntimeSystem
    {
    public:
        bool TryAcceptFromNpc(QuestSystem& quests, const Npc& npc, std::string& outMessage) const;
        bool NotifyNpcTalk(QuestSystem& quests, const Npc& npc) const;
        bool RewardQuestIfPossible(QuestSystem& quests, Player& player, const Npc& npc, std::string& outRewardMessage, InventoryItem* outRewardItem = nullptr) const;
        void NotifyEnemyKilled(QuestSystem& quests, const std::string& enemyType, std::string& outMessage) const;
        bool HandleNpcInteraction(
            QuestSystem &quests,
            Player &player,
            const Npc &npc,
            gameplay::InventorySystem &inventory,
            std::function<void(const std::string &)> openChoiceUiCallback,
            std::string &outMessage) const;
        void NotifyItemCollected(QuestSystem& quests, const std::string& itemId, int amount) const;
    };
}
