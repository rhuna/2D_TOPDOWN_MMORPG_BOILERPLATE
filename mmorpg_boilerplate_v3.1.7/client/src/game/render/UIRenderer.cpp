#include "game/render/UIRenderer.h"
#include "raylib.h"
#include <string>

namespace gameplay
{
    void UIRenderer::DrawInventoryPanel(
        int screenWidth,
        int screenHeight,
        const std::vector<InventoryItem>& inventory,
        int selectedIndex) const
    {
        const int boxW = 460;
        const int boxH = 300;
        const int boxX = (screenWidth - boxW) / 2;
        const int boxY = (screenHeight - boxH) / 2;

        DrawRectangle(boxX, boxY, boxW, boxH, Fade(BLACK, 0.90f));
        DrawRectangleLines(boxX, boxY, boxW, boxH, WHITE);

        DrawText("Inventory", boxX + 20, boxY + 18, 28, YELLOW);
        DrawText("Esc = close, Enter = use", boxX + 20, boxY + 52, 18, LIGHTGRAY);

        int y = boxY + 95;

        if (inventory.empty())
        {
            DrawText("Inventory is empty.", boxX + 28, y, 22, LIGHTGRAY);
            return;
        }

        for (int i = 0; i < static_cast<int>(inventory.size()); ++i)
        {
            const bool selected = (i == selectedIndex);

            if (selected)
            {
                DrawRectangle(boxX + 16, y - 4, boxW - 32, 30, Fade(SKYBLUE, 0.25f));
                DrawRectangleLines(boxX + 16, y - 4, boxW - 32, 30, SKYBLUE);
            }

            const auto& item = inventory[i];
            const std::string line = item.name + " - " + std::to_string(item.price) + "g";
            DrawText(line.c_str(), boxX + 28, y, 22, selected ? WHITE : LIGHTGRAY);
            y += 38;
        }
    }

    void UIRenderer::DrawEquipmentPanel(
        int screenWidth,
        int screenHeight,
        const InventoryItem& weapon,
        const InventoryItem& armor,
        int selectedIndex) const
    {
        const int boxW = 360;
        const int boxH = 220;
        const int boxX = (screenWidth - boxW) / 2;
        const int boxY = (screenHeight - boxH) / 2;

        DrawRectangle(boxX, boxY, boxW, boxH, Fade(BLACK, 0.90f));
        DrawRectangleLines(boxX, boxY, boxW, boxH, WHITE);

        DrawText("Equipment", boxX + 20, boxY + 18, 28, YELLOW);
        DrawText("Esc = close", boxX + 20, boxY + 52, 18, LIGHTGRAY);

        const bool weaponSelected = (selectedIndex == 0);
        const bool armorSelected = (selectedIndex == 1);

        if (weaponSelected)
        {
            DrawRectangle(boxX + 16, boxY + 92, boxW - 32, 30, Fade(SKYBLUE, 0.25f));
            DrawRectangleLines(boxX + 16, boxY + 92, boxW - 32, 30, SKYBLUE);
        }

        DrawText((std::string("Weapon: ") + weapon.name).c_str(), boxX + 28, boxY + 98, 22, weaponSelected ? WHITE : LIGHTGRAY);

        if (armorSelected)
        {
            DrawRectangle(boxX + 16, boxY + 130, boxW - 32, 30, Fade(SKYBLUE, 0.25f));
            DrawRectangleLines(boxX + 16, boxY + 130, boxW - 32, 30, SKYBLUE);
        }

        DrawText((std::string("Armor: ") + armor.name).c_str(), boxX + 28, boxY + 136, 22, armorSelected ? WHITE : LIGHTGRAY);
    }

    void UIRenderer::DrawQuestLogPanel(
        int screenWidth,
        int screenHeight,
        const std::vector<const QuestState*>& activeQuests,
        const QuestSystem& questSystem,
        int selectedIndex) const
    {
        const int boxW = 540;
        const int boxH = 320;
        const int boxX = (screenWidth - boxW) / 2;
        const int boxY = (screenHeight - boxH) / 2;

        DrawRectangle(boxX, boxY, boxW, boxH, Fade(BLACK, 0.92f));
        DrawRectangleLines(boxX, boxY, boxW, boxH, WHITE);

        DrawText("Quest Log", boxX + 20, boxY + 18, 28, YELLOW);
        DrawText("Esc = close", boxX + 20, boxY + 52, 18, LIGHTGRAY);

        int y = boxY + 95;

        if (activeQuests.empty())
        {
            DrawText("No active quests.", boxX + 28, y, 22, LIGHTGRAY);
            return;
        }

        for (int i = 0; i < static_cast<int>(activeQuests.size()); ++i)
        {
            const bool selected = (i == selectedIndex);
            const QuestState* quest = activeQuests[i];

            if (selected)
            {
                DrawRectangle(boxX + 16, y - 4, boxW - 32, 30, Fade(SKYBLUE, 0.25f));
                DrawRectangleLines(boxX + 16, y - 4, boxW - 32, 30, SKYBLUE);
            }

            if (quest != nullptr)
            {
                const QuestDefinition* definition = questSystem.GetDefinition(quest->questId);
                const std::string label = (definition != nullptr && !definition->title.empty()) ? definition->title : quest->questId;
                DrawText(label.c_str(), boxX + 28, y, 22, selected ? WHITE : LIGHTGRAY);
            }

            y += 38;
        }
    }

    void UIRenderer::DrawShopPanel(
        int screenWidth,
        int screenHeight,
        const Npc& merchant,
        int selectedIndex) const
    {
        const int boxW = 560;
        const int boxH = 340;
        const int boxX = (screenWidth - boxW) / 2;
        const int boxY = (screenHeight - boxH) / 2;

        DrawRectangle(boxX, boxY, boxW, boxH, Fade(BLACK, 0.92f));
        DrawRectangleLines(boxX, boxY, boxW, boxH, WHITE);

        DrawText(merchant.name.c_str(), boxX + 20, boxY + 18, 28, YELLOW);
        DrawText("Esc = close, Enter = buy", boxX + 20, boxY + 52, 18, LIGHTGRAY);

        int y = boxY + 95;

        if (merchant.shopInventory.empty())
        {
            DrawText("This shop is empty.", boxX + 28, y, 22, LIGHTGRAY);
            return;
        }

        for (int i = 0; i < static_cast<int>(merchant.shopInventory.size()); ++i)
        {
            const bool selected = (i == selectedIndex);
            const ShopItem& item = merchant.shopInventory[i];

            if (selected)
            {
                DrawRectangle(boxX + 16, y - 4, boxW - 32, 30, Fade(SKYBLUE, 0.25f));
                DrawRectangleLines(boxX + 16, y - 4, boxW - 32, 30, SKYBLUE);
            }

            const std::string line = item.name + " - " + std::to_string(item.price) + "g";
            DrawText(line.c_str(), boxX + 28, y, 22, selected ? WHITE : LIGHTGRAY);
            y += 38;
        }
    }

    void UIRenderer::DrawChoicePanel(
        int screenWidth,
        int screenHeight,
        const std::vector<std::string>& choices,
        int selectedIndex) const
    {
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.55f));

        const int boxW = 620;
        const int boxH = 240;
        const int boxX = (screenWidth - boxW) / 2;
        const int boxY = (screenHeight - boxH) / 2;

        DrawRectangle(boxX, boxY, boxW, boxH, Fade(BLACK, 0.90f));
        DrawRectangleLines(boxX, boxY, boxW, boxH, WHITE);

        DrawText("Choose Your Path", boxX + 20, boxY + 18, 28, YELLOW);
        DrawText("Use Up/Down to select, Enter to confirm, Esc to cancel",
                 boxX + 20, boxY + 52, 18, LIGHTGRAY);

        int y = boxY + 95;
        for (int i = 0; i < static_cast<int>(choices.size()); ++i)
        {
            const bool selected = (i == selectedIndex);

            if (selected)
            {
                DrawRectangle(boxX + 16, y - 4, boxW - 32, 30, Fade(SKYBLUE, 0.25f));
                DrawRectangleLines(boxX + 16, y - 4, boxW - 32, 30, SKYBLUE);
            }

            std::string line = std::to_string(i + 1) + ". " + choices[i];
            DrawText(line.c_str(), boxX + 28, y, 22, selected ? WHITE : LIGHTGRAY);
            y += 38;
        }
    }
}
