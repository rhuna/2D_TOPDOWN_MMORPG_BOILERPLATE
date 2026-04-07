#pragma once

#include "core/Types.h"
#include <string>

namespace gameplay
{
    class ShopSystem
    {
    public:
        bool TryBuy(Player& player, const Npc& merchant, int selectedIndex, std::string& outMessage) const;
    };
}
