#include "gameplay/ShopSystem.h"

#include "gameplay/ItemFactory.h"

bool ShopSystem::TryBuy(Player& player,
                        const Npc& merchant,
                        int selectedIndex,
                        const InventorySystem& inventorySystem,
                        std::string& outMessage) const
{
    if (!merchant.isMerchant)
    {
        outMessage = merchant.name + " is not a merchant.";
        return false;
    }

    if (selectedIndex < 0 || selectedIndex >= static_cast<int>(merchant.shopInventory.size()))
    {
        outMessage = "Nothing is selected.";
        return false;
    }

    const ShopItem& item = merchant.shopInventory[selectedIndex];
    if (player.gold < item.price)
    {
        outMessage = "Not enough gold for " + item.name + ".";
        return false;
    }

    player.gold -= item.price;
    inventorySystem.AddItem(player, gameplay::MakeInventoryItemByName(item.name, item.amount));
    outMessage = "Bought " + item.name + " for " + std::to_string(item.price) + " gold.";
    return true;
}
