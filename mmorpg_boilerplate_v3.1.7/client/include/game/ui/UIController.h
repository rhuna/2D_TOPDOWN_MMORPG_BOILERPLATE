#pragma once

#include "game/ui/UIState.h"

namespace gameplay
{
    class UIController
    {
    public:
        void HandleGlobalToggleInput(UIState& ui) const;
        void CloseAll(UIState& ui) const;
        bool AnyBlockingPanelOpen(const UIState& ui) const;
    };
}
