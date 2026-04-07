#pragma once

#include <string>
#include <vector>

namespace gameplay
{
    struct UIState
    {
        bool inventoryOpen = false;
        bool equipmentOpen = false;
        bool questLogOpen = false;
        bool shopOpen = false;
        bool dialogueOpen = false;
        bool choiceOpen = false;

        int selectedInventoryIndex = 0;
        int selectedEquipmentIndex = 0;
        int selectedQuestIndex = 0;
        int selectedShopIndex = 0;
        int merchantIndex = -1;

        std::string statusMessage;
        std::string dialogueText;
        std::string choiceQuestId;
        std::vector<std::string> choiceTexts;
        int selectedChoiceIndex = 0;
    };
}
