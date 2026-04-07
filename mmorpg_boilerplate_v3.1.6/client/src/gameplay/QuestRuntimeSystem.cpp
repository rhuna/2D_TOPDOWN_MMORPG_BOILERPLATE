#include "gameplay/QuestRuntimeSystem.h"
#include "gameplay/InventorySystem.h"
#include <functional>

namespace gameplay
{
    bool QuestRuntimeSystem::TryAcceptFromNpc(QuestSystem& quests, const Npc& npc, std::string& outMessage) const
    {
        // Placeholder hook for future npc->quest mapping.
        QuestEvent event;
        event.type = QuestEventType::NpcTalkedTo;
        event.targetId = npc.name;
        event.amount = 1;
        quests.NotifyEvent(event);
        outMessage = npc.idleText;
        return true;
    }

    bool QuestRuntimeSystem::NotifyNpcTalk(QuestSystem& quests, const Npc& npc) const
    {
        QuestEvent event;
        event.type = QuestEventType::NpcTalkedTo;
        event.targetId = npc.name;
        event.amount = 1;
        quests.NotifyEvent(event);
        return true;
    }

    bool QuestRuntimeSystem::RewardQuestIfPossible(QuestSystem& quests, Player& player, const Npc& npc, std::string& outRewardMessage, InventoryItem* outRewardItem) const
    {
        (void)player;
        (void)npc;
        (void)outRewardItem;
        int gold = 0;
        int xp = 0;
        std::string itemId;
        int itemCount = 0;
        // Placeholder: real quest turn-in routing should map NPCs to quest IDs.
        (void)quests;
        (void)gold; (void)xp; (void)itemId; (void)itemCount;
        outRewardMessage.clear();
        return false;
    }

    void QuestRuntimeSystem::NotifyEnemyKilled(QuestSystem& quests, const std::string& enemyType, std::string& outMessage) const
    {
        QuestEvent event;
        event.type = QuestEventType::EnemyKilled;
        event.targetId = enemyType;
        event.amount = 1;
        quests.NotifyEvent(event);
        outMessage = "Killed an enemy: " + enemyType;
    }

    bool QuestRuntimeSystem::HandleNpcInteraction(
        QuestSystem &quests,
        Player &player,
        const Npc &npc,
        gameplay::InventorySystem &inventory,
        std::function<void(const std::string &)> openChoiceUiCallback,
        std::string &outMessage) const
    {
        // This is a placeholder implementation. A real implementation would have more complex logic to determine whether to open a choice UI, what choices to present, etc.
        if (quests.IsAwaitingChoice(npc.name))
        {
            openChoiceUiCallback(npc.name);
            outMessage = npc.questText;
            return true;
        }
        else
        {
            // Just trigger a normal talk event for now.
            return NotifyNpcTalk(quests, npc);
        }
    }

    void QuestRuntimeSystem::NotifyItemCollected(QuestSystem& quests, const std::string& itemId, int amount) const
    {
        QuestEvent event;
        event.type = QuestEventType::ItemCollected;
        event.targetId = itemId;
        event.amount = amount;
        quests.NotifyEvent(event);
        // outMessage is not used here since this is a more passive event, but you could extend the system to support it if desired.
    }
}
