#include "game/world/World.h"

void World::HandleInteraction()
{
    if (!IsKeyPressed(KEY_E))
    {
        return;
    }

    for (int i = 0; i < static_cast<int>(npcs_.size()); ++i)
    {
        const Npc& npc = npcs_[i];
        if (!IsClose(player_.position, npc.position, 52.0f))
        {
            continue;
        }

        if (npc.isMerchant)
        {
            OpenShopUi(i);
            message_ = npc.name + ": What would you like to buy?";
            return;
        }

        if (services_.questRuntime.HandleNpcInteraction(
                questSystem_,
                player_,
                npc,
                services_.inventory,
                [&](const std::string& questId)
                { OpenChoiceUi(questId); },
                message_))
        {
            return;
        }
    }

    for (auto& drop : drops_)
    {
        if (!drop.taken && IsClose(player_.position, drop.position, 40.0f))
        {
            drop.taken = true;
            TryPickup(drop.itemName, drop.amount);
            if (drop.itemName == "Herb")
            {
                services_.questRuntime.NotifyItemCollected(*services_.quests, "herb", drop.amount);
            }
            message_ = "Picked up " + drop.itemName + " x" + std::to_string(drop.amount) + ".";
            return;
        }
    }
}
