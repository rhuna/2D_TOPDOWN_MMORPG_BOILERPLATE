#include "game/world/World.h"
#include "game/systems/ShopSystem.h"
#include "game/systems/ItemFactory.h"
#include "raylib.h"
//services_
#include "game/systems/GameServices.h"

void World::SyncUiStateFromLegacy()
{
    uiState_.inventoryOpen = inventoryUi_.IsOpen();
    uiState_.equipmentOpen = equipmentUi_.IsOpen();
    uiState_.questLogOpen = questLogUi_.IsOpen();
    uiState_.shopOpen = shopUi_.IsOpen();
    uiState_.dialogueOpen = dialogueUi_.IsOpen();
    uiState_.choiceOpen = choiceUi_.visible;

    uiState_.selectedInventoryIndex = inventoryUi_.SelectedIndex();
    uiState_.selectedEquipmentIndex = equipmentUi_.SelectedIndex();
    uiState_.selectedQuestIndex = questLogUi_.SelectedIndex();
    uiState_.selectedShopIndex = shopUi_.SelectedIndex();
    uiState_.merchantIndex = shopUi_.MerchantIndex();

    uiState_.statusMessage = message_;
    uiState_.choiceQuestId = choiceUi_.questId;
    uiState_.choiceTexts = choiceUi_.choiceTexts;
    uiState_.selectedChoiceIndex = choiceUi_.selectedIndex;
}

void World::ApplyUiStateToLegacy()
{
    const bool inventoryWasOpen = inventoryUi_.IsOpen();
    const bool equipmentWasOpen = equipmentUi_.IsOpen();
    const bool questWasOpen = questLogUi_.IsOpen();

    if (uiState_.inventoryOpen && !inventoryWasOpen) inventoryUi_.Open();
    if (!uiState_.inventoryOpen && inventoryWasOpen) inventoryUi_.Close();

    if (uiState_.equipmentOpen && !equipmentWasOpen) equipmentUi_.Open();
    if (!uiState_.equipmentOpen && equipmentWasOpen) equipmentUi_.Close();

    if (uiState_.questLogOpen && !questWasOpen) questLogUi_.Open();
    if (!uiState_.questLogOpen && questWasOpen) questLogUi_.Close();

    if (!uiState_.shopOpen && shopUi_.IsOpen()) shopUi_.Close();
    if (!uiState_.dialogueOpen && dialogueUi_.IsOpen()) dialogueUi_.Close();
    if (!uiState_.choiceOpen && choiceUi_.visible) CloseChoiceUi();
}

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
        SyncUiStateFromLegacy();
    }
}

void World::ToggleEquipmentUi()
{
    const bool shouldOpen = !equipmentUi_.IsOpen();
    CloseAllOverlayUi();

    if (shouldOpen)
    {
        equipmentUi_.Open();
        SyncUiStateFromLegacy();
    }
}

void World::ToggleQuestLogUi()
{
    const bool shouldOpen = !questLogUi_.IsOpen();
    CloseAllOverlayUi();

    if (shouldOpen)
    {
        questLogUi_.Open();
        SyncUiStateFromLegacy();
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
    SyncUiStateFromLegacy();
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
        if (!services_.inventory.TryUseSelectedItem(player_, selectedIndex, message_))
        {
            services_.inventory.TryEquipSelectedItem(player_, selectedIndex, message_);
        }
    }
}

void World::DrawInventoryUi() const
{
    if (!inventoryUi_.IsOpen())
    {
        return;
    }


    uiRenderer_.DrawInventoryPanel(
        screenWidth_,
        screenHeight_,
        player_.inventory,
        inventoryUi_.SelectedIndex());
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

    auto weaponItem = gameplay::ItemConverter(player_.weapon);
    auto armorItem = gameplay::ItemConverter(player_.armor);

    uiRenderer_.DrawEquipmentPanel(
        screenWidth_,                        
        screenHeight_,
        weaponItem,
        armorItem,
        equipmentUi_.SelectedIndex());
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

    const auto activeQuests = services_.quests->GetActiveQuests();
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

    uiRenderer_.DrawQuestLogPanel(
        screenWidth_,
        screenHeight_,
        services_.quests->GetActiveQuests(),
        *services_.quests,
        questLogUi_.SelectedIndex());
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
    SyncUiStateFromLegacy();
}

void World::CloseShopUi()
{
    shopUi_.Close();
    SyncUiStateFromLegacy();
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
        services_.shop.TryBuy(player_, merchant, shopUi_.SelectedIndex(), message_);
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

    uiRenderer_.DrawShopPanel(
        screenWidth_,
        screenHeight_,
        merchant,
        shopUi_.SelectedIndex());
}