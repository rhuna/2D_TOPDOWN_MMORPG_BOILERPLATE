#pragma once
#include "game/ui/UIState.h"
namespace gameplay {
class UIController {
public:
    void HandleToggleInput(UIState& ui);
    void CloseAll(UIState& ui) const;
    bool AnyPanelOpen(const UIState& ui) const;
};
}
