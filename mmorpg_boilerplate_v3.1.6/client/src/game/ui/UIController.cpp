#include "game/ui/UIController.h"
#include "raylib.h"
namespace gameplay {
void UIController::HandleToggleInput(UIState& ui){
    if(IsKeyPressed(KEY_I)) ui.inventoryOpen=!ui.inventoryOpen;
    if(IsKeyPressed(KEY_C)) ui.equipmentOpen=!ui.equipmentOpen;
    if(IsKeyPressed(KEY_Q)) ui.questLogOpen=!ui.questLogOpen;
    if(IsKeyPressed(KEY_ESCAPE)) CloseAll(ui);
}
void UIController::CloseAll(UIState& ui) const{
    ui.inventoryOpen=false;
    ui.equipmentOpen=false;
    ui.questLogOpen=false;
    ui.shopOpen=false;
    ui.dialogueOpen=false;
    ui.choiceOpen=false;
}
bool UIController::AnyPanelOpen(const UIState& ui) const{
    return ui.inventoryOpen||ui.equipmentOpen||ui.questLogOpen||
           ui.shopOpen||ui.dialogueOpen||ui.choiceOpen;
}
}
