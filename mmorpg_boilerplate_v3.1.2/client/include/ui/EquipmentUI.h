#pragma once

class EquipmentUI
{
public:
    void Open()
    {
        visible_ = true;
        selectedIndex_ = 0;
    }

    void Close()
    {
        visible_ = false;
    }

    bool IsOpen() const
    {
        return visible_;
    }

    void ResetSelection()
    {
        selectedIndex_ = 0;
    }

    void MoveSelection(int delta, int slotCount)
    {
        if (slotCount <= 0)
        {
            selectedIndex_ = 0;
            return;
        }

        selectedIndex_ += delta;

        if (selectedIndex_ < 0)
            selectedIndex_ = slotCount - 1;
        if (selectedIndex_ >= slotCount)
            selectedIndex_ = 0;
    }

    int SelectedIndex() const
    {
        return selectedIndex_;
    }

private:
    bool visible_ = false;
    int selectedIndex_ = 0;
};