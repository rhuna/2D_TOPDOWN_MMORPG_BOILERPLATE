#pragma once

#include "core/Types.h"
#include <string>

namespace gameplay
{
    class InventorySystem
    {
    public:
        bool TryAddItem(Player& player, const InventoryItem& item, std::string& outMessage) const;
        bool TryUseSelectedItem(Player& player, int selectedIndex, std::string& outMessage) const;
        bool TryEquipSelectedItem(Player& player, int selectedIndex, std::string& outMessage) const;
        void getInventory(const Player& player, std::vector<InventoryItem>& outInventory) const;
    };
}
