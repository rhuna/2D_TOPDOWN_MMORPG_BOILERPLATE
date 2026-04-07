# Client Architecture (restructured)

This zip preserves your current code, but reorganizes it into a clearer long-term layout.

## Folder ownership
- `app/` bootstrapping and runtime loop
- `game/world/` world orchestration and world-owned behavior
- `game/quests/` quest definitions and quest progression rules
- `game/systems/` inventory, shop, item factory, and quest-runtime bridges
- `ui/windows/` concrete menu / overlay widgets
- `network/` client networking
- `archive/legacy_duplicates/` old duplicate world files kept only for reference and removed from the build

## Important notes
- `World_Gameplay.cpp` and `World_Render.cpp` were moved out of the active build because they overlap with `World_Update.cpp` and `World_Draw.cpp`.
- The current `World` class still owns a lot. This zip is the **manageable intermediate architecture**, not the final end-state.
- The next real step is extracting `WorldState`, `WorldSession`, and dedicated `WorldInteractionSystem` / `WorldUiSystem` classes.
