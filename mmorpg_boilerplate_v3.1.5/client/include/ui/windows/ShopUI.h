#pragma once

class ShopUI
{
public:
    void Open()
    {
        visible_ = true;
        selectedIndex_ = 0;
        merchantIndex_ = -1;
    }

    void OpenForMerchant(int merchantIndex)
    {
        visible_ = true;
        selectedIndex_ = 0;
        merchantIndex_ = merchantIndex;
    }

    void Close()
    {
        visible_ = false;
        merchantIndex_ = -1;
    }

    bool IsOpen() const
    {
        return visible_;
    }

    void ResetSelection()
    {
        selectedIndex_ = 0;
    }

    void MoveSelection(int delta, int itemCount)
    {
        if (itemCount <= 0)
        {
            selectedIndex_ = 0;
            return;
        }

        selectedIndex_ += delta;

        if (selectedIndex_ < 0)
            selectedIndex_ = itemCount - 1;
        if (selectedIndex_ >= itemCount)
            selectedIndex_ = 0;
    }

    int SelectedIndex() const
    {
        return selectedIndex_;
    }

    int MerchantIndex() const
    {
        return merchantIndex_;
    }

private:
    bool visible_ = false;
    int selectedIndex_ = 0;
    int merchantIndex_ = -1;
};