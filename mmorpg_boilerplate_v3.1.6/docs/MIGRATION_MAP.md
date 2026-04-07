# Migration map

## Moved files
- `client/src/main.cpp` -> `client/src/app/main.cpp`
- `client/include/core/Game.h` -> `client/include/app/Game.h`
- `client/src/core/Game.cpp` -> `client/src/app/Game.cpp`
- `client/include/net/NetworkClient.h` -> `client/include/network/NetworkClient.h`
- `client/src/net/NetworkClient.cpp` -> `client/src/network/NetworkClient.cpp`
- `client/include/game/QuestSystem.h` -> `client/include/game/quests/QuestSystem.h`
- `client/src/game/QuestSystem.cpp` -> `client/src/game/quests/QuestSystem.cpp`
- `client/include/gameplay/*` -> `client/include/game/systems/*`
- `client/src/gameplay/*` -> `client/src/game/systems/*`
- `client/include/ui/*` -> `client/include/ui/windows/*`
- `client/src/ui/*` -> `client/src/ui/windows/*`
- `client/include/world/*` -> `client/include/game/world/*`
- active world sources -> `client/src/game/world/*`
- duplicate world sources -> `client/archive/legacy_duplicates/*`

## Build impact
`client/CMakeLists.txt` was updated to point at the new file locations and to exclude archived duplicates from compilation.
