#include "gameplay/QuestRuntimeSystem.h"

#include "gameplay/ItemFactory.h"

#include <algorithm>

bool QuestRuntimeSystem::HandleNpcInteraction(QuestSystem& questSystem,
                                              Player& player,
                                              const Npc& npc,
                                              const InventorySystem& inventorySystem,
                                              std::function<void(const std::string&)> openChoiceUi,
                                              std::string& outMessage) const
{
    if (npc.name == "Elder Rowan")
    {
        if (questSystem.CanAcceptQuest("main_001"))
        {
            questSystem.AcceptQuest("main_001");
            questSystem.NotifyEvent({QuestEventType::NpcTalkedTo, "elder_rowan", 1});
            outMessage = npc.name + ": Please clear out 3 slimes near the village.";
            return true;
        }

        if (questSystem.IsQuestActive("main_001"))
        {
            questSystem.NotifyEvent({QuestEventType::NpcTalkedTo, "elder_rowan", 1});
        }

        if (questSystem.IsAwaitingChoice("main_001"))
        {
            openChoiceUi("main_001");
            outMessage = npc.name + ": Choose your next path.";
            return true;
        }

        int rewardGold = 0;
        int rewardXp = 0;
        int rewardItemCount = 0;
        std::string rewardItemId;
        if (questSystem.RewardQuest("main_001", rewardGold, rewardXp, rewardItemId, rewardItemCount))
        {
            player.gold += rewardGold;
            player.xp += rewardXp;
            if (rewardItemId == "bronze_blade")
            {
                inventorySystem.AddNamedItem(player, "Bronze Blade", std::max(1, rewardItemCount));
                player.weapon = gameplay::MakeWeaponForItemName("Bronze Blade");
            }
            outMessage = npc.name + ": You have earned your reward.";
            return true;
        }

        if (questSystem.CanAcceptQuest("main_002") && questSystem.HasFlag("path_crossroads"))
        {
            questSystem.AcceptQuest("main_002");
            questSystem.NotifyEvent({QuestEventType::NpcTalkedTo, "elder_rowan", 1});
            outMessage = npc.name + ": Head to the crossroads and find the Wanderer.";
            return true;
        }

        if (questSystem.CanAcceptQuest("side_001") && questSystem.HasFlag("path_healer"))
        {
            questSystem.AcceptQuest("side_001");
            questSystem.NotifyEvent({QuestEventType::NpcTalkedTo, "elder_rowan", 1});
            outMessage = npc.name + ": Gather 3 herbs for the healer.";
            return true;
        }

        if (questSystem.IsQuestActive("side_001"))
        {
            questSystem.NotifyEvent({QuestEventType::NpcTalkedTo, "elder_rowan", 1});
            if (questSystem.RewardQuest("side_001", rewardGold, rewardXp, rewardItemId, rewardItemCount))
            {
                player.gold += rewardGold;
                player.xp += rewardXp;
                outMessage = npc.name + ": Thank you for helping the healer.";
                return true;
            }
        }

        outMessage = npc.name + " (" + npc.regionName + "): " + npc.idleText;
        return true;
    }

    if (npc.name == "Wanderer")
    {
        if (questSystem.IsQuestActive("main_002"))
        {
            questSystem.NotifyEvent({QuestEventType::NpcTalkedTo, "wanderer", 1});
        }

        int rewardGold = 0;
        int rewardXp = 0;
        int rewardItemCount = 0;
        std::string rewardItemId;
        if (questSystem.RewardQuest("main_002", rewardGold, rewardXp, rewardItemId, rewardItemCount))
        {
            player.gold += rewardGold;
            player.xp += rewardXp;
            outMessage = npc.name + ": Welcome to the crossroads.";
            return true;
        }

        outMessage = npc.name + " (" + npc.regionName + "): " + npc.idleText;
        return true;
    }

    outMessage = npc.name + " (" + npc.regionName + "): " + npc.idleText;
    return true;
}

void QuestRuntimeSystem::NotifyEnemyKilled(QuestSystem& questSystem,
                                           const std::string& enemyId,
                                           std::string& ioMessage) const
{
    questSystem.NotifyEvent({QuestEventType::EnemyKilled, enemyId, 1});
    if (questSystem.IsQuestCompleted("main_001"))
    {
        ioMessage = "Quest complete. Return to Elder Rowan.";
    }
}

void QuestRuntimeSystem::NotifyItemCollected(QuestSystem& questSystem,
                                             const std::string& itemId,
                                             int amount) const
{
    questSystem.NotifyEvent({QuestEventType::ItemCollected, itemId, amount});
}
