#pragma once

#include "core/Types.h"
#include <string>

namespace gameplay
{
    InventoryItem MakeInventoryItemByName(const std::string& itemName, int amount = 1);
    Weapon MakeWeaponForItemName(const std::string& name);
    Armor MakeArmorForItemName(const std::string& name);

    InventoryItem ItemConverter(const Weapon &weapon);
    InventoryItem ItemConverter(const Armor &armor);
}
