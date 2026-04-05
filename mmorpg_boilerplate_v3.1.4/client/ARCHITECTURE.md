# World module layout

This refactor splits the old `World.cpp` into responsibility-oriented translation units:

- `World.cpp` — construction, asset loading, camera, helpers, shared world utilities
- `World_Gameplay.cpp` — top-level `World::Update(float dt)` orchestration only
- `World_Render.cpp` — all world and actor rendering
- `World_Actors.cpp` — local movement, enemy AI, drop cleanup, door transitions
- `World_Combat.cpp` — player attack resolution and kill rewards
- `World_Interaction.cpp` — NPC conversations, quest progression, item pickup interactions
- `World_Choice.cpp` — branching choice UI logic
- `WorldUI.cpp` — inventory, equipment, quest log, and shop UI
- `Data.cpp` — map generation data only

## Scaling rules

1. Keep orchestration in `World_Gameplay.cpp`; keep heavy implementation elsewhere.
2. Add new render-only functions to `World_Render.cpp`.
3. Add new combat rules to `World_Combat.cpp`.
4. Add new NPC interaction / quest handoff logic to `World_Interaction.cpp`.
5. Keep pure helpers that multiple files need in `World.cpp` or move them into dedicated shared utility headers later.
6. Do not add duplicate state fields on derived structs; `Npc` now uses the inherited `Actor::position`.
