#include "game/systems/ItemFactory.h"

namespace gameplay
{
    InventoryItem MakeInventoryItemByName(const std::string &itemName, int amount)
    {
        InventoryItem item{};
        item.name = itemName;
        item.amount = amount;

        if (itemName == "Potion")
        {
            item.category = ItemCategory::Consumable;
            item.description = "Restores a strong amount of health.";
            item.attackBonus = 0;
            item.defenseBonus = 0;
            item.healAmount = 10;
            item.price = 10;
            item.stackable = true;
            return item;
        }

        if (itemName == "Herb")
        {
            item.category = ItemCategory::Consumable;
            item.description = "Restores a little health.";
            item.attackBonus = 0;
            item.defenseBonus = 0;
            item.healAmount = 4;
            item.price = 4;
            item.stackable = true;
            return item;
        }

        if (itemName == "Iron Sword")
        {
            item.category = ItemCategory::Weapon;
            item.description = "A stronger sword than your starter blade.";
            item.attackBonus = 6;
            item.defenseBonus = 0;
            item.healAmount = 0;
            item.price = 30;
            item.stackable = false;
            return item;
        }

        if (itemName == "Leather Armor")
        {
            item.category = ItemCategory::Armor;
            item.description = "Light armor for new adventurers.";
            item.attackBonus = 0;
            item.defenseBonus = 3;
            item.healAmount = 0;
            item.price = 24;
            item.stackable = false;
            return item;
        }

        item.category = ItemCategory::Material;
        item.description = "A gathered material.";
        item.attackBonus = 0;
        item.defenseBonus = 0;
        item.healAmount = 0;
        item.price = 1;
        item.stackable = true;
        return item;
    }

    InventoryItem ItemConverter(const Weapon &weapon)
    {
        InventoryItem item{};
        item.name = weapon.name;
        item.category = ItemCategory::Weapon;
        item.description = weapon.description;
        item.attackBonus = weapon.attackBonus;
        item.defenseBonus = 0;
        item.healAmount = 0;
        item.price = weapon.price;
        item.stackable = false;
        return item;
    }

    InventoryItem ItemConverter(const Armor &armor)
    {
        InventoryItem item{};
        item.name = armor.name;
        item.category = ItemCategory::Armor;
        item.description = armor.description;
        item.attackBonus = 0;
        item.defenseBonus = armor.defenseBonus;
        item.healAmount = 0;
        item.price = armor.price;
        item.stackable = false;
        return item;
    }
}