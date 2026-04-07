#pragma once
#include <string>
namespace gameplay {
struct UIState {
    bool inventoryOpen=false;
    bool equipmentOpen=false;
    bool questLogOpen=false;
    bool shopOpen=false;
    bool dialogueOpen=false;
    bool choiceOpen=false;
    std::string dialogueText;
    std::string choiceText;
    std::string statusMessage;
    int selectedInventoryIndex=0;
    int selectedShopIndex=0;
    int selectedQuestIndex=0;
};
}
