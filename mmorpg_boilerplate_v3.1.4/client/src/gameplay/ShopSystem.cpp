
#include "gameplay/ShopSystem.h"

namespace gameplay
{
    bool ShopSystem::TryBuy(Player &player, const Npc &merchant, int selectedIndex, std::string &message) const
    {
        if (!merchant.isMerchant)
        {
            message = "That NPC is not a merchant.";
            return false;
        }

        if (selectedIndex < 0 || selectedIndex >= static_cast<int>(merchant.shopInventory.size()))
        {
            message = "Invalid shop item.";
            return false;
        }

        const ShopItem &shopItem = merchant.shopInventory[selectedIndex];

        if (player.gold < shopItem.price)
        {
            message = "Not enough gold.";
            return false;
        }

        InventoryItem item{};
        item.name = shopItem.name;
        item.price = shopItem.price;
        item.amount = shopItem.amount;
        item.category = shopItem.category;
        item.description = shopItem.description;
        item.attackBonus = shopItem.attackBonus;
        item.defenseBonus = shopItem.defenseBonus;
        item.healAmount = shopItem.healAmount;
        item.stackable = (shopItem.category == ItemCategory::Consumable || shopItem.category == ItemCategory::Material);

        player.gold -= shopItem.price;
        player.inventory.push_back(item);

        message = "Bought " + item.name + ".";
        return true;
    }
}
