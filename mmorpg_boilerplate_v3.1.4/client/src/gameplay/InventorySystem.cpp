#include "gameplay/InventorySystem.h"

#include "gameplay/ItemFactory.h"

namespace
{
void RemoveOrDecrement(Player& player, int index)
{
    if (index < 0 || index >= static_cast<int>(player.inventory.size()))
    {
        return;
    }

    if (player.inventory[index].stackable)
    {
        player.inventory[index].count -= 1;
    }
    else
    {
        player.inventory[index].count = 0;
    }

    if (player.inventory[index].count <= 0)
    {
        player.inventory.erase(player.inventory.begin() + index);
    }
}
} // namespace

bool InventorySystem::AddItem(Player& player, const InventoryItem& item) const
{
    if (item.stackable)
    {
        for (auto& existing : player.inventory)
        {
            if (existing.name == item.name && existing.stackable)
            {
                existing.count += item.count;
                return true;
            }
        }
    }

    player.inventory.push_back(item);
    return true;
}

bool InventorySystem::AddNamedItem(Player& player, const std::string& itemName, int amount) const
{
    return AddItem(player, gameplay::MakeInventoryItemByName(itemName, amount));
}

bool InventorySystem::UseSelectedItem(Player& player, int selectedIndex, std::string& outMessage) const
{
    if (selectedIndex < 0 || selectedIndex >= static_cast<int>(player.inventory.size()))
    {
        return false;
    }

    const InventoryItem& item = player.inventory[selectedIndex];
    if (item.category != ItemCategory::Consumable || item.healAmount <= 0)
    {
        return false;
    }

    const int before = player.hp;
    player.hp = std::min(player.maxHp, player.hp + item.healAmount);
    const int healed = player.hp - before;

    RemoveOrDecrement(player, selectedIndex);
    outMessage = "Used " + item.name + " and restored " + std::to_string(healed) + " HP.";
    return true;
}

bool InventorySystem::EquipSelectedItem(Player& player, int selectedIndex, std::string& outMessage) const
{
    if (selectedIndex < 0 || selectedIndex >= static_cast<int>(player.inventory.size()))
    {
        return false;
    }

    const InventoryItem& item = player.inventory[selectedIndex];
    if (!item.equippable)
    {
        return false;
    }

    if (item.category == ItemCategory::Weapon)
    {
        player.weapon = gameplay::MakeWeaponFromItem(item);
        outMessage = "Equipped " + item.name + ".";
        return true;
    }

    if (item.category == ItemCategory::Armor)
    {
        player.armor = gameplay::MakeArmorFromItem(item);
        outMessage = "Equipped " + item.name + ".";
        return true;
    }

    return false;
}
