#include "world/World.h"

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

void World::HandleInteraction() {
    if (!IsKeyPressed(KEY_E)) {
        return;
    }

    for (auto& npc : npcs_) {
        if (!IsClose(player_.position, npc.position, 52.0f)) {
            continue;
        }

        if (!IsClose(player_.position, npc.position, 52.0f))
        {
            continue;
        }
        if (npc.isMerchant)
        {
            for (int i = 0; i < static_cast<int>(npcs_.size()); ++i)
            {
                if (&npcs_[i] == &npc)
                {
                    OpenShopUi(i);
                    message_ = npc.name + ": What would you like to buy?";
                    return;
                }
            }
        }
        if (npc.name == "Elder Rowan")
        {
            if (questSystem_.CanAcceptQuest("main_001"))
            {
                questSystem_.AcceptQuest("main_001");
                questSystem_.NotifyEvent({QuestEventType::NpcTalkedTo, "elder_rowan", 1});
                message_ = npc.name + ": Please clear out 3 slimes near the village.";
                return;
            }
            if (questSystem_.IsQuestActive("main_001"))
            {
                questSystem_.NotifyEvent({QuestEventType::NpcTalkedTo, "elder_rowan", 1});
            }
            if (questSystem_.IsAwaitingChoice("main_001"))
            {
                OpenChoiceUi("main_001");
                message_ = npc.name + ": Choose your next path.";
                return;
            }
            int rewardGold = 0;
            int rewardXp = 0;
            int rewardItemCount = 0;
            std::string rewardItemId;
            if (questSystem_.RewardQuest("main_001", rewardGold, rewardXp, rewardItemId, rewardItemCount))
            {
                player_.gold += rewardGold;
                player_.xp += rewardXp;
                if (rewardItemId == "bronze_blade")
                {
                    // Add the reward item to inventory and equip it immediately.
                    TryPickup("Bronze Blade", 1);
                    player_.weapon = MakeWeaponForItemName("Bronze Blade");
                }
                message_ = npc.name + ": You have earned your reward.";
                return;
            }
            if (questSystem_.CanAcceptQuest("main_002") && questSystem_.HasFlag("path_crossroads"))
            {
                questSystem_.AcceptQuest("main_002");
                questSystem_.NotifyEvent({QuestEventType::NpcTalkedTo, "elder_rowan", 1});
                message_ = npc.name + ": Head to the crossroads and find the Wanderer.";
                return;
            }
            if (questSystem_.CanAcceptQuest("side_001") && questSystem_.HasFlag("path_healer"))
            {
                questSystem_.AcceptQuest("side_001");
                questSystem_.NotifyEvent({QuestEventType::NpcTalkedTo, "elder_rowan", 1});
                message_ = npc.name + ": Gather 3 herbs for the healer.";
                return;
            }
            if (questSystem_.IsQuestActive("side_001"))
            {
                questSystem_.NotifyEvent({QuestEventType::NpcTalkedTo, "elder_rowan", 1});
                if (questSystem_.RewardQuest("side_001", rewardGold, rewardXp, rewardItemId, rewardItemCount))
                {
                    player_.gold += rewardGold;
                    player_.xp += rewardXp;
                    message_ = npc.name + ": Thank you for helping the healer.";
                    return;
                }
            }
            message_ = npc.name + " (" + npc.regionName + "): " + npc.idleText;
            return;
        }
        if (npc.name == "Wanderer")
        {
            if (questSystem_.IsQuestActive("main_002"))
            {
                questSystem_.NotifyEvent({QuestEventType::NpcTalkedTo, "wanderer", 1});
            }
            int rewardGold = 0;
            int rewardXp = 0;
            int rewardItemCount = 0;
            std::string rewardItemId;
            if (questSystem_.RewardQuest("main_002", rewardGold, rewardXp, rewardItemId, rewardItemCount))
            {
                player_.gold += rewardGold;
                player_.xp += rewardXp;
                message_ = npc.name + ": Welcome to the crossroads.";
                return;
            }
            message_ = npc.name + " (" + npc.regionName + "): " + npc.idleText;
            return;
        }
    }

    for (auto& drop : drops_) {
        if (!drop.taken && IsClose(player_.position, drop.position, 40.0f)) {
            drop.taken = true;
            TryPickup(drop.itemName, drop.amount);
            if (drop.itemName == "Herb")
            {
                questSystem_.NotifyEvent({QuestEventType::ItemCollected, "herb", drop.amount});
            }
            message_ = "Picked up " + drop.itemName + " x" + std::to_string(drop.amount) + ".";
            return;
        }
    }
}

