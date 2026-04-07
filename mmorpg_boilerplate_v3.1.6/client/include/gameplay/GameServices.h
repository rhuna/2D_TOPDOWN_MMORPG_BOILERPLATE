#pragma once

#include "game/QuestSystem.h"
#include "gameplay/InventorySystem.h"
#include "gameplay/QuestRuntimeSystem.h"
#include "gameplay/ShopSystem.h"

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
