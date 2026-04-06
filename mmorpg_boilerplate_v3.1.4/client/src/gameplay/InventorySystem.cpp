#include "gameplay/InventorySystem.h"
#include <algorithm>

namespace gameplay
{
    bool InventorySystem::TryAddItem(Player &player, const InventoryItem &item, std::string &message) const
    {
        if (item.name.empty() || item.amount <= 0)
        {
            return false;
        }

        if (item.stackable)
        {
            for (auto &existing : player.inventory)
            {
                if (existing.name == item.name)
                {
                    existing.amount += item.amount;
                    message = "Picked up " + item.name + ".";
                    return true;
                }
            }
        }

        player.inventory.push_back(item);
        message = "Picked up " + item.name + ".";
        return true;
    }

    bool InventorySystem::TryUseSelectedItem(Player &player, int selectedIndex, std::string &message) const
    {
        if (selectedIndex < 0 || selectedIndex >= static_cast<int>(player.inventory.size()))
        {
            return false;
        }

        InventoryItem &item = player.inventory[selectedIndex];

        if (item.category != ItemCategory::Consumable)
        {
            message = "That item cannot be used.";
            return false;
        }

        player.hp = std::min(player.maxHp, player.hp + item.healAmount);
        item.amount -= 1;
        message = "Used " + item.name + ".";

        if (item.amount <= 0)
        {
            player.inventory.erase(player.inventory.begin() + selectedIndex);
        }

        return true;
    }

    bool InventorySystem::TryEquipSelectedItem(Player &player, int selectedIndex, std::string &message) const
    {
        if (selectedIndex < 0 || selectedIndex >= static_cast<int>(player.inventory.size()))
        {
            return false;
        }

        const InventoryItem &item = player.inventory[selectedIndex];

        if (item.category == ItemCategory::Weapon)
        {
            player.weapon.name = item.name;
            player.weapon.damage = item.attackBonus;
            message = "Equipped " + item.name + ".";
            return true;
        }

        if (item.category == ItemCategory::Armor)
        {
            player.armor.name = item.name;
            player.armor.defense = item.defenseBonus;
            message = "Equipped " + item.name + ".";
            return true;
        }

        message = "That item cannot be equipped.";
        return false;
    }
    
    void InventorySystem::getInventory(const Player &player, std::vector<InventoryItem> &outInventory) const
    {
        outInventory = player.inventory;
    }
}