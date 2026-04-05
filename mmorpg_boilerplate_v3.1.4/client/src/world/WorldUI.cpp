#include "world/World.h"
#include "gameplay/ShopSystem.h"
#include "raylib.h"

// ==========================
// TOGGLES
// ==========================

void World::ToggleInventoryUi()
{
    const bool shouldOpen = !inventoryUi_.IsOpen();
    CloseAllOverlayUi();

    if (shouldOpen)
    {
        inventoryUi_.Open();
    }
}

void World::ToggleEquipmentUi()
{
    const bool shouldOpen = !equipmentUi_.IsOpen();
    CloseAllOverlayUi();

    if (shouldOpen)
    {
        equipmentUi_.Open();
    }
}

void World::ToggleQuestLogUi()
{
    const bool shouldOpen = !questLogUi_.IsOpen();
    CloseAllOverlayUi();

    if (shouldOpen)
    {
        questLogUi_.Open();
    }
}

// ==========================
// GLOBAL CLOSE
// ==========================

void World::CloseAllOverlayUi()
{
    dialogueUi_.Close();
    inventoryUi_.Close();
    equipmentUi_.Close();
    questLogUi_.Close();
    shopUi_.Close();
    CloseChoiceUi();
}

// ==========================
// BLOCKING UI CHECK
// ==========================

bool World::IsBlockingUiOpen() const
{
    return choiceUi_.visible || shopUi_.IsOpen() || inventoryUi_.IsOpen() || equipmentUi_.IsOpen() || questLogUi_.IsOpen() || dialogueUi_.IsOpen();
}

// ==========================
// INVENTORY
// ==========================

void World::UpdateInventoryUi()
{
    if (!inventoryUi_.IsOpen())
    {
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE))
    {
        inventoryUi_.Close();
        return;
    }

    const int itemCount = static_cast<int>(player_.inventory.size());

    if (IsKeyPressed(KEY_UP))
    {
        inventoryUi_.MoveSelection(-1, itemCount);
    }

    if (IsKeyPressed(KEY_DOWN))
    {
        inventoryUi_.MoveSelection(1, itemCount);
    }

    if (IsKeyPressed(KEY_ENTER) && itemCount > 0)
    {
        const int selectedIndex = inventoryUi_.SelectedIndex();
        if (!inventorySystem_.UseSelectedItem(player_, selectedIndex, message_))
        {
            inventorySystem_.EquipSelectedItem(player_, selectedIndex, message_);
        }
    }
}

void World::DrawInventoryUi() const
{
    if (!inventoryUi_.IsOpen())
    {
        return;
    }

    const int boxW = 460;
    const int boxH = 300;
    const int boxX = (screenWidth_ - boxW) / 2;
    const int boxY = (screenHeight_ - boxH) / 2;

    DrawRectangle(boxX, boxY, boxW, boxH, Fade(BLACK, 0.90f));
    DrawRectangleLines(boxX, boxY, boxW, boxH, WHITE);

    DrawText("Inventory", boxX + 20, boxY + 18, 28, YELLOW);
    DrawText("Esc = close, Enter = use", boxX + 20, boxY + 52, 18, LIGHTGRAY);

    int y = boxY + 95;

    if (player_.inventory.empty())
    {
        DrawText("Inventory is empty.", boxX + 28, y, 22, LIGHTGRAY);
        return;
    }

    for (int i = 0; i < static_cast<int>(player_.inventory.size()); ++i)
    {
        const bool selected = (i == inventoryUi_.SelectedIndex());

        if (selected)
        {
            DrawRectangle(boxX + 16, y - 4, boxW - 32, 30, Fade(SKYBLUE, 0.25f));
            DrawRectangleLines(boxX + 16, y - 4, boxW - 32, 30, SKYBLUE);
        }

        const auto &item = player_.inventory[i];
        const std::string line = item.name + " - " + std::to_string(item.price) + "g";
        DrawText(line.c_str(), boxX + 28, y, 22, selected ? WHITE : LIGHTGRAY);
        y += 38;
    }
}

// ==========================
// EQUIPMENT
// ==========================

void World::UpdateEquipmentUi()
{
    if (!equipmentUi_.IsOpen())
    {
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE))
    {
        equipmentUi_.Close();
        return;
    }

    constexpr int slotCount = 2;

    if (IsKeyPressed(KEY_UP))
    {
        equipmentUi_.MoveSelection(-1, slotCount);
    }

    if (IsKeyPressed(KEY_DOWN))
    {
        equipmentUi_.MoveSelection(1, slotCount);
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        message_ = "Equipment management logic goes here.";
    }
}

void World::DrawEquipmentUi() const
{
    if (!equipmentUi_.IsOpen())
    {
        return;
    }

    const int boxW = 360;
    const int boxH = 220;
    const int boxX = (screenWidth_ - boxW) / 2;
    const int boxY = (screenHeight_ - boxH) / 2;

    DrawRectangle(boxX, boxY, boxW, boxH, Fade(BLACK, 0.90f));
    DrawRectangleLines(boxX, boxY, boxW, boxH, WHITE);

    DrawText("Equipment", boxX + 20, boxY + 18, 28, YELLOW);
    DrawText("Esc = close", boxX + 20, boxY + 52, 18, LIGHTGRAY);

    const bool weaponSelected = (equipmentUi_.SelectedIndex() == 0);
    const bool armorSelected = (equipmentUi_.SelectedIndex() == 1);

    if (weaponSelected)
    {
        DrawRectangle(boxX + 16, boxY + 92, boxW - 32, 30, Fade(SKYBLUE, 0.25f));
        DrawRectangleLines(boxX + 16, boxY + 92, boxW - 32, 30, SKYBLUE);
    }

    DrawText((std::string("Weapon: ") + player_.weapon.name).c_str(), boxX + 28, boxY + 98, 22, weaponSelected ? WHITE : LIGHTGRAY);

    if (armorSelected)
    {
        DrawRectangle(boxX + 16, boxY + 130, boxW - 32, 30, Fade(SKYBLUE, 0.25f));
        DrawRectangleLines(boxX + 16, boxY + 130, boxW - 32, 30, SKYBLUE);
    }

    DrawText((std::string("Armor: ") + player_.armor.name).c_str(), boxX + 28, boxY + 136, 22, armorSelected ? WHITE : LIGHTGRAY);
}

// ==========================
// QUEST LOG
// ==========================

void World::UpdateQuestLogUi()
{
    if (!questLogUi_.IsOpen())
    {
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE))
    {
        questLogUi_.Close();
        return;
    }

    const auto activeQuests = questSystem_.GetActiveQuests();
    const int questCount = static_cast<int>(activeQuests.size());

    if (IsKeyPressed(KEY_UP))
    {
        questLogUi_.MoveSelection(-1, questCount);
    }

    if (IsKeyPressed(KEY_DOWN))
    {
        questLogUi_.MoveSelection(1, questCount);
    }
}

void World::DrawQuestLogUi() const
{
    if (!questLogUi_.IsOpen())
    {
        return;
    }

    const int boxW = 540;
    const int boxH = 320;
    const int boxX = (screenWidth_ - boxW) / 2;
    const int boxY = (screenHeight_ - boxH) / 2;

    DrawRectangle(boxX, boxY, boxW, boxH, Fade(BLACK, 0.92f));
    DrawRectangleLines(boxX, boxY, boxW, boxH, WHITE);

    DrawText("Quest Log", boxX + 20, boxY + 18, 28, YELLOW);
    DrawText("Esc = close", boxX + 20, boxY + 52, 18, LIGHTGRAY);

    const auto activeQuests = questSystem_.GetActiveQuests();
    int y = boxY + 95;

    if (activeQuests.empty())
    {
        DrawText("No active quests.", boxX + 28, y, 22, LIGHTGRAY);
        return;
    }

    for (int i = 0; i < static_cast<int>(activeQuests.size()); ++i)
    {
        const bool selected = (i == questLogUi_.SelectedIndex());
        const QuestState *quest = activeQuests[i];

        if (selected)
        {
            DrawRectangle(boxX + 16, y - 4, boxW - 32, 30, Fade(SKYBLUE, 0.25f));
            DrawRectangleLines(boxX + 16, y - 4, boxW - 32, 30, SKYBLUE);
        }

        if (quest != nullptr)
        {
            const QuestDefinition* definition = questSystem_.GetDefinition(quest->questId);
            const std::string label = (definition != nullptr && !definition->title.empty()) ? definition->title : quest->questId;
            DrawText(label.c_str(), boxX + 28, y, 22, selected ? WHITE : LIGHTGRAY);
        }

        y += 38;
    }
}

// ==========================
// SHOP
// ==========================

void World::OpenShopUi(int merchantIndex)
{
    if (merchantIndex < 0 || merchantIndex >= static_cast<int>(npcs_.size()))
    {
        return;
    }

    if (!npcs_[merchantIndex].isMerchant)
    {
        return;
    }

    CloseAllOverlayUi();
    shopUi_.OpenForMerchant(merchantIndex);
}

void World::CloseShopUi()
{
    shopUi_.Close();
}

void World::UpdateShopUi()
{
    if (!shopUi_.IsOpen())
    {
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE))
    {
        shopUi_.Close();
        return;
    }

    const int merchantIndex = shopUi_.MerchantIndex();

    if (merchantIndex < 0 || merchantIndex >= static_cast<int>(npcs_.size()))
    {
        shopUi_.Close();
        return;
    }

    const Npc &merchant = npcs_[merchantIndex];

    if (!merchant.isMerchant)
    {
        shopUi_.Close();
        return;
    }

    const int itemCount = static_cast<int>(merchant.shopInventory.size());

    if (IsKeyPressed(KEY_UP))
    {
        shopUi_.MoveSelection(-1, itemCount);
    }

    if (IsKeyPressed(KEY_DOWN))
    {
        shopUi_.MoveSelection(1, itemCount);
    }

    if (IsKeyPressed(KEY_ENTER) && itemCount > 0)
    {
        shopSystem_.TryBuy(player_, merchant, shopUi_.SelectedIndex(), inventorySystem_, message_);
    }
}

void World::DrawShopUi() const
{
    if (!shopUi_.IsOpen())
    {
        return;
    }

    const int merchantIndex = shopUi_.MerchantIndex();

    if (merchantIndex < 0 || merchantIndex >= static_cast<int>(npcs_.size()))
    {
        return;
    }

    const Npc &merchant = npcs_[merchantIndex];

    if (!merchant.isMerchant)
    {
        return;
    }

    const int boxW = 560;
    const int boxH = 340;
    const int boxX = (screenWidth_ - boxW) / 2;
    const int boxY = (screenHeight_ - boxH) / 2;

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
        const bool selected = (i == shopUi_.SelectedIndex());
        const ShopItem &item = merchant.shopInventory[i];

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