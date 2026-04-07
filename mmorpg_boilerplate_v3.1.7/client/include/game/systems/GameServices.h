#pragma once

#include "game/quests/QuestSystem.h"
#include "game/systems/InventorySystem.h"
#include "game/systems/QuestRuntimeSystem.h"
#include "game/systems/ShopSystem.h"

namespace gameplay
{
    struct GameServices
    {
        InventorySystem inventory;
        ShopSystem shop;
        QuestRuntimeSystem questRuntime;
        QuestSystem* quests = nullptr;

        void BindQuestSystem(QuestSystem& questSystem)
        {
            quests = &questSystem;
        }

        bool HasQuestSystem() const
        {
            return quests != nullptr;
        }

        QuestSystem& QuestSystemRef()
        {
            return *quests;
        }

        const QuestSystem& QuestSystemRef() const
        {
            return *quests;
        }
    };
}
