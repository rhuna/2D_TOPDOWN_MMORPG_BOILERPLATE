#include "gameplay/ItemFactory.h"

namespace gameplay
{
InventoryItem MakeInventoryItemByName(const std::string& itemName, int amount)
{
    if (itemName == "Potion")
    {
        return InventoryItem{"Potion", amount, ItemCategory::Consumable, "Restores a strong amount of health.", 0, 0, 10, 10, false, true};
    }
    if (itemName == "Herb")
    {
        return InventoryItem{"Herb", amount, ItemCategory::Consumable, "Restores a little health.", 0, 0, 4, 4, false, true};
    }
    if (itemName == "Iron Sword")
    {
        return InventoryItem{"Iron Sword", amount, ItemCategory::Weapon, "A stronger sword sold by merchants.", 6, 0, 0, 30, true, false};
    }
    if (itemName == "Bronze Blade")
    {
        return InventoryItem{"Bronze Blade", amount, ItemCategory::Weapon, "A quest reward weapon.", 4, 0, 0, 0, true, false};
    }
    if (itemName == "Leather Armor")
    {
        return InventoryItem{"Leather Armor", amount, ItemCategory::Armor, "Light armor that adds defense.", 0, 3, 0, 24, true, false};
    }
    if (itemName == "Chain Vest")
    {
        return InventoryItem{"Chain Vest", amount, ItemCategory::Armor, "Heavier armor with better protection.", 0, 5, 0, 40, true, false};
    }
    return InventoryItem{itemName, amount, ItemCategory::Material, "A generic crafting material.", 0, 0, 0, 1, false, true};
}

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

Armor MakeArmorForItemName(const std::string& name)
{
    if (name == "Leather Armor")
    {
        return Armor{"Leather Armor", 3};
    }
    if (name == "Chain Vest")
    {
        return Armor{"Chain Vest", 5};
    }
    return Armor{"Traveler Clothes", 0};
}

Weapon MakeWeaponFromItem(const InventoryItem& item)
{
    return MakeWeaponForItemName(item.name);
}

Armor MakeArmorFromItem(const InventoryItem& item)
{
    return MakeArmorForItemName(item.name);
}
} // namespace gameplay
