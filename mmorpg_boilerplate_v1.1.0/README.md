# TopDownMMOStarter - Tileset Edition

This version of the repo has been patched to use the provided `assets/tileset.png` sprite sheet.

## What changed
- Added `assets/tileset.png`
- Replaced flat-color map tiles with textured floor and wall tiles from the sheet
- Replaced player, NPC, slime, herb drop, and HUD weapon marker with tilesheet sprites
- Delayed world creation until after `InitWindow()` so texture loading works correctly in raylib
- Added cleanup for the tileset texture in `World::~World()`

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
The source rectangles used for the tilesheet are hardcoded in `include/World.h`. If you want, those can be expanded into a real atlas / animation system next.
