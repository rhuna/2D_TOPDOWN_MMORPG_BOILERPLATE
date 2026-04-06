# Engine-Style Architecture Plan

This package is a **drop-in refactor scaffold** for evolving the current project into a long-term, scalable codebase.

## Goals
- Keep `World` as an orchestrator instead of a rules bucket.
- Move gameplay rules into dedicated systems.
- Keep rendering isolated from gameplay logic.
- Prepare for save/load, multiplayer, content-driven data, and future ECS-style work.

## Module ownership

### world/
Owns world state and high-level orchestration:
- map data and spawn setup
- actor containers
- camera and collision helpers
- top-level update order and draw order

### gameplay/
Owns rules and mutation of state:
- item creation
- inventory changes
- equipment changes
- shopping rules
- quest runtime handoff

### ui/
Owns modal state, local selection state, and drawing of menus.

### data/
Owns content presets and definitions.

## Recommended next phases
1. Keep existing world split stable.
2. Gradually route old helper calls to `gameplay::` or `services_`.
3. Move hardcoded merchant stock and item presets into data tables.
4. Introduce a save-game serializer and content database.
5. Later: convert to ECS-friendly actor storage if needed.
