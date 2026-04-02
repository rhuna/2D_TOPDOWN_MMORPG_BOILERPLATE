# TopDownMMOStarter Clean Commented Build

This zip contains a cleaned-up, commented version of the large overworld build.

## What is included

- A playable **single-player overworld client** using raylib
- A cleaner project layout:
  - `client/include/...` for headers
  - `client/src/...` for implementation
- The original tileset integrated under `assets/tileset.png`
- An **optional** prototype multiplayer server folder
- A shared `Protocol.h` file for future networking work

## Build the client

From the project root:

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER="C:\msys64\ucrt64\bin\g++.exe"
cmake --build build
.\build\client\TopDownMMOClient.exe
```

## Build the optional server

1. Download standalone Asio
2. Put `asio.hpp` here:

```text
third_party/asio/include/asio.hpp
```

Then build:

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DBUILD_MMO_SERVER=ON -DCMAKE_CXX_COMPILER="C:\msys64\ucrt64\bin\g++.exe"
cmake --build build
.\build\server\TopDownMMOServer.exe
```

## Controls

- `WASD` move
- `Space` attack
- `E` interact
- `TAB` toggle world map
- `Mouse wheel` zoom in and out
