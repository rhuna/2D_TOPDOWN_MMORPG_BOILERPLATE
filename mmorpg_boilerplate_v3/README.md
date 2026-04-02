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


## Multiplayer test

This build now includes a prototype client/server sync path.

### Build client + server

```powershell
cmake -S . -B build -G "MinGW Makefiles" -DBUILD_MMO_SERVER=ON -DCMAKE_CXX_COMPILER="C:\msys64\ucrt64\bin\g++.exe"
cmake --build build
```

### Run the server

```powershell
.\build\server\TopDownMMOServer.exe
```

### Run two clients

Open two terminals and run:

```powershell
.\build\client\TopDownMMOClient.exe
```

You should see:
- your local player rendered with the main game sprite system
- other connected players rendered as blue markers with labels like `P2`

Notes:
- This is still a prototype sync layer, not a production MMO server.
- Remote players currently use simple marker rendering instead of full animated sprite replication.
