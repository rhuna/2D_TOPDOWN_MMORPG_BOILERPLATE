#pragma once

#include "core/Types.h"
#include <string>

class InventorySystem
{
public:
    bool AddItem(Player& player, const InventoryItem& item) const;
    bool AddNamedItem(Player& player, const std::string& itemName, int amount) const;
    bool UseSelectedItem(Player& player, int selectedIndex, std::string& outMessage) const;
    bool EquipSelectedItem(Player& player, int selectedIndex, std::string& outMessage) const;
};
