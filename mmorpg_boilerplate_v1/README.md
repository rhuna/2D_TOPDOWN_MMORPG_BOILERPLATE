# TopDownMMOStarter

A **small C++ top-down action-RPG/MMO-style starter repo** built with **raylib**.

This is **not** a real production MMO server stack. It is a **playable single-process prototype** that gives you the foundation for:
- player movement
- enemies
- weapons
- quests
- NPC interaction
- drops and inventory
- HUD and combat loop

## Controls
- **WASD**: move
- **Space**: attack
- **E**: talk / loot
- **Esc**: close window

## What is included
- a tile-based map
- one quest-giver NPC
- several slimes
- a starting weapon and a quest reward weapon
- XP, gold, item drops, and inventory counts
- a clean CMake project

## What is NOT included yet
- networking
- login/account services
- database persistence
- animation system
- multiplayer synchronization
- large world streaming
- crafting, parties, guilds, trading, or chat server

## Build on Windows

### Option 1: Visual Studio 2022
```powershell
cmake -S . -B build
cmake --build build --config Release
.\build\Release\TopDownMMOStarter.exe
```

### Option 2: MinGW
```powershell
cmake -S . -B build -G "MinGW Makefiles"
cmake --build build
.\build\TopDownMMOStarter.exe
```

## Project layout
```text
TopDownMMOStarter/
├── CMakeLists.txt
├── README.md
├── include/
│   ├── Data.h
│   ├── Game.h
│   ├── Types.h
│   └── World.h
├── src/
│   ├── Data.cpp
│   ├── Game.cpp
│   ├── World.cpp
│   └── main.cpp
└── docs/
    └── LINE_BY_LINE_GUIDE.md
```

## Architecture overview
- `main.cpp` creates the game object and starts the loop.
- `Game.cpp` owns the window and frame loop.
- `World.cpp` contains the actual game logic.
- `Types.h` stores the main data models.
- `Data.cpp` builds the tile map.

## How to grow this into a real MMO
1. Split client and server.
2. Move entity state to an authoritative server.
3. Store player inventory, quests, and stats in a database.
4. Add packet serialization and interpolation.
5. Add content pipelines for maps, NPCs, quests, and items.
6. Replace hard-coded data with JSON, Lua, YAML, or SQL-backed content.
7. Add animation, pathfinding, and zone loading.
