#include "game/ui/UIController.h"
#include "raylib.h"

namespace gameplay
{
    void UIController::HandleGlobalToggleInput(UIState& ui) const
    {
        if (AnyBlockingPanelOpen(ui) && IsKeyPressed(KEY_ESCAPE))
        {
            CloseAll(ui);
            return;
        }

        if (ui.choiceOpen || ui.shopOpen || ui.dialogueOpen)
        {
            return;
        }

        if (IsKeyPressed(KEY_I))
        {
            const bool next = !ui.inventoryOpen;
            CloseAll(ui);
            ui.inventoryOpen = next;
        }

        if (IsKeyPressed(KEY_C))
        {
            const bool next = !ui.equipmentOpen;
            CloseAll(ui);
            ui.equipmentOpen = next;
        }

        if (IsKeyPressed(KEY_Q))
        {
            const bool next = !ui.questLogOpen;
            CloseAll(ui);
            ui.questLogOpen = next;
        }
    }

    void UIController::CloseAll(UIState& ui) const
    {
        ui.inventoryOpen = false;
        ui.equipmentOpen = false;
        ui.questLogOpen = false;
        ui.shopOpen = false;
        ui.dialogueOpen = false;
        ui.choiceOpen = false;
        ui.merchantIndex = -1;
    }

    bool UIController::AnyBlockingPanelOpen(const UIState& ui) const
    {
        return ui.inventoryOpen ||
               ui.equipmentOpen ||
               ui.questLogOpen ||
               ui.shopOpen ||
               ui.dialogueOpen ||
               ui.choiceOpen;
    }
}
