# TopDownMMOStarter - Sprite Atlas Edition

This build turns the one-off hardcoded sprite rectangles into a cleaner atlas-driven setup.

## What changed
- Added a reusable atlas layer for:
  - directional idle clips
  - directional walk clips
  - tile palettes for grass, path, dirt, and walls
- Added directional facing for the player, NPC, and slime enemies
- Added walk animation playback with frame timing and per-frame bobbing
- Added better tile variety using atlas palettes and deterministic tile variation
- Added path tiles directly into the demo map for a more readable village layout

## Controls
- `W A S D` move
- `Space` attack
- `E` talk / loot

## Build on Windows with MSYS2 UCRT64
```powershell
Remove-Item -Recurse -Force build
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_C_COMPILER="C:\msys64\ucrt64\bin\gcc.exe" -DCMAKE_CXX_COMPILER="C:\msys64\ucrt64\bin\g++.exe"
cmake --build build
.\build\TopDownMMOStarter.exe
```

## Notes
The atlas selections live in `World::SetupAtlas()`. That gives you one place to swap in new cells later without rewriting the game loop or draw code.
