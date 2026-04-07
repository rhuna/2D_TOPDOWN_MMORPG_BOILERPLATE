#pragma once

#include "core/Types.h"
#include "game/quests/QuestSystem.h"
#include <string>
#include <vector>

namespace gameplay
{
    class UIRenderer
    {
    public:
        void DrawInventoryPanel(
            int screenWidth,
            int screenHeight,
            const std::vector<InventoryItem>& inventory,
            int selectedIndex) const;

        void DrawEquipmentPanel(
            int screenWidth,
            int screenHeight,
            const InventoryItem& weapon,
            const InventoryItem& armor,
            int selectedIndex) const;

        void DrawQuestLogPanel(
            int screenWidth,
            int screenHeight,
            const std::vector<const QuestState*>& activeQuests,
            const QuestSystem& questSystem,
            int selectedIndex) const;

        void DrawShopPanel(
            int screenWidth,
            int screenHeight,
            const Npc& merchant,
            int selectedIndex) const;

        void DrawChoicePanel(
            int screenWidth,
            int screenHeight,
            const std::vector<std::string>& choices,
            int selectedIndex) const;
    };
}
