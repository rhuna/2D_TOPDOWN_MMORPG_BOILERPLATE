# Scalable gameplay architecture

This stage moves long-term game rules out of `World` and into dedicated gameplay systems.

## World module responsibility

`World` is now mainly responsible for:
- world state ownership (`player_`, `npcs_`, `enemies_`, `drops_`)
- map/camera/render orchestration
- top-level update orchestration
- routing input to UI and systems

`World` should **not** be the place where item rules, shop purchase rules, or quest reward rules are invented.

## Gameplay systems

### `gameplay/ItemFactory`
Single source of truth for turning data names into runtime items/equipment.

Owns:
- inventory item construction by item name
- weapon construction by item name
- armor construction by item name

### `gameplay/InventorySystem`
Owns player inventory mutations.

Owns:
- adding items
- stacking logic
- consumable use logic
- equip logic for weapon/armor items

### `gameplay/ShopSystem`
Owns merchant purchase rules.

Owns:
- gold checks
- selected item validation
- adding purchased items to inventory
- purchase result messaging

### `gameplay/QuestRuntimeSystem`
Owns runtime quest handoff rules that connect NPC interactions and combat events to `QuestSystem`.

Owns:
- NPC conversation branches that accept / reward quests
- enemy kill quest event routing
- item collection quest event routing
- quest reward application to player state

## Scaling rules

1. Add new item definitions in `ItemFactory` or migrate that file to data-driven JSON later.
2. Add new player inventory behavior in `InventorySystem`.
3. Add buy/sell/currency rules in `ShopSystem`.
4. Add quest runtime scripting and NPC reward logic in `QuestRuntimeSystem`.
5. Keep `World_Interaction.cpp` thin: detect nearby actor, then hand off to a system.
6. Keep `WorldUI.cpp` thin: handle menu cursor movement, then hand off to a system when Enter is pressed.

## Next recommended step

The next long-term scalability upgrade is to move spawn definitions and NPC/merchant setup out of `World.cpp` into a data-driven spawn/content loader so worlds can be authored without recompiling code.
