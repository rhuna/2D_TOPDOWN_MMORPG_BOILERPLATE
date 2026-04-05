#pragma once

#include "core/Types.h"
#include "gameplay/InventorySystem.h"
#include <string>

class ShopSystem
{
public:
    bool TryBuy(Player& player,
                const Npc& merchant,
                int selectedIndex,
                const InventorySystem& inventorySystem,
                std::string& outMessage) const;
};
