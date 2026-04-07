#include <string>


#include "include/ui/ShopUI.h"

ShopUI::ShopUI()
{
}

ShopUI::~ShopUI()
{
}

void ShopUI::UpdateShopUi()
{
    if (!IsOpen())
        return;

    if (IsKeyPressed(KEY_ESCAPE))
    {
        CloseShopUi();
        message_ = "Closed shop.";
        return;
    }

    if (shopUi_.merchantIndex < 0 || shopUi_.merchantIndex >= static_cast<int>(npcs_.size()))
    {
        CloseShopUi();
        return;
    }

    const auto &stock = npcs_[shopUi_.merchantIndex].shopStock;
    if (stock.empty())
        return;

    if (IsKeyPressed(KEY_UP))
    {
        shopUi_.selectedIndex--;
        if (shopUi_.selectedIndex < 0)
            shopUi_.selectedIndex = static_cast<int>(stock.size()) - 1;
    }

    if (IsKeyPressed(KEY_DOWN))
    {
        shopUi_.selectedIndex++;
        if (shopUi_.selectedIndex >= static_cast<int>(stock.size()))
            shopUi_.selectedIndex = 0;
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        const ShopItem &item = stock[shopUi_.selectedIndex];

        if (player_.gold < item.price)
        {
            message_ = "Not enough gold for " + item.name + ".";
            return;
        }

        player_.gold -= item.price;

        // Everything purchased goes into inventory first.
        // The player then equips weapon/armor from the inventory UI.
        TryPickup(item.name, item.amount);
        message_ = "Bought " + item.name + " x" + std::to_string(item.amount) + ".";
    }
}

void ShopUI::DrawShopUi() const
{
    if (!IsOpen())
        return;

    if (shopUi_.merchantIndex < 0 || shopUi_.merchantIndex >= static_cast<int>(npcs_.size()))
        return;

    const Npc &merchant = npcs_[shopUi_.merchantIndex];
    const auto &stock = merchant.shopStock;

    DrawRectangle(0, 0, screenWidth_, screenHeight_, Fade(BLACK, 0.55f));

    const int boxW = 760;
    const int boxH = 360;
    const int boxX = (screenWidth_ - boxW) / 2;
    const int boxY = (screenHeight_ - boxH) / 2;

    DrawRectangle(boxX, boxY, boxW, boxH, Fade(BLACK, 0.92f));
    DrawRectangleLines(boxX, boxY, boxW, boxH, WHITE);

    DrawText(merchant.name.c_str(), boxX + 20, boxY + 18, 28, YELLOW);
    DrawText("Use Up/Down to select, Enter to buy, Esc to close",
             boxX + 20, boxY + 52, 18, LIGHTGRAY);

    int y = boxY + 95;
    for (int i = 0; i < static_cast<int>(stock.size()); ++i)
    {
        const bool selected = (i == shopUi_.selectedIndex);
        const ShopItem &item = stock[i];

        if (selected)
        {
            DrawRectangle(boxX + 16, y - 4, 320, 30, Fade(SKYBLUE, 0.25f));
            DrawRectangleLines(boxX + 16, y - 4, 320, 30, SKYBLUE);
        }

        std::string line = item.name + "  -  " + std::to_string(item.price) + " gold";
        DrawText(line.c_str(), boxX + 28, y, 22, selected ? WHITE : LIGHTGRAY);
        y += 38;
    }

    if (!stock.empty())
    {
        const ShopItem &item = stock[shopUi_.selectedIndex];

        const int detailX = boxX + 380;
        const int detailY = boxY + 100;

        DrawText(item.name.c_str(), detailX, detailY, 28, WHITE);
        DrawText(ItemCategoryLabel(item.category), detailX, detailY + 40, 20, ItemCategoryColor(item.category));
        DrawText(item.description.c_str(), detailX, detailY + 72, 18, LIGHTGRAY);

        if (item.attackBonus > 0)
        {
            DrawText(TextFormat("Attack: +%d", item.attackBonus), detailX, detailY + 120, 20, ORANGE);
        }

        if (item.defenseBonus > 0)
        {
            DrawText(TextFormat("Defense: +%d", item.defenseBonus), detailX, detailY + 150, 20, SKYBLUE);
        }

        if (item.healAmount > 0)
        {
            DrawText(TextFormat("Healing: %d", item.healAmount), detailX, detailY + 180, 20, GREEN);
        }
    }

    std::string goldLine = "Your Gold: " + std::to_string(player_.gold);
    DrawText(goldLine.c_str(), boxX + 20, boxY + boxH - 36, 22, GOLD);
}

void ShopUI::OpenShopUi(int merchantIndex)
{
    Open();
    selectedIndex_ = 0;
    merchantIndex = merchantIndex;
}

void ShopUI::CloseShopUi()
{
    Close();
    selectedIndex_ = 0;
    // Clear the merchant index when closing the shop UI to avoid stale references.
    //merchant index is a reference to the NPC that the player is currently
    //interacting with. When the shop UI is closed, we should clear this 
    //reference to avoid potential issues if the player interacts with a 
    //different merchant later on.
    world_.merchantIndex = -1;
}
