# LINE BY LINE GUIDE

This guide explains **every line** in the starter repo.


## `CMakeLists.txt`

- **Line 1:** `cmake_minimum_required(VERSION 3.16)`  
  Sets the minimum CMake version required to configure this project.
- **Line 2:** `project(TopDownMMOStarter LANGUAGES CXX)`  
  Declares the CMake project name and the language used by the project.
- **Line 3:** ` `  
  Blank line used to visually separate sections.
- **Line 4:** `set(CMAKE_CXX_STANDARD 17)`  
  Forces the project to compile as C++17.
- **Line 5:** `set(CMAKE_CXX_STANDARD_REQUIRED ON)`  
  Makes CMake require that exact C++ standard support.
- **Line 6:** `set(CMAKE_CXX_EXTENSIONS OFF)`  
  Disables compiler-specific language extensions so the code stays closer to standard C++.
- **Line 7:** ` `  
  Blank line used to visually separate sections.
- **Line 8:** `include(FetchContent)`  
  Loads CMake's FetchContent module so dependencies can be downloaded during configure time.
- **Line 9:** ` `  
  Blank line used to visually separate sections.
- **Line 10:** `FetchContent_Declare(`  
  Begins the declaration of an external dependency that CMake should fetch.
- **Line 11:** `    raylib`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 12:** `    GIT_REPOSITORY https://github.com/raysan5/raylib.git`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 13:** `    GIT_TAG 5.5`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 14:** `)`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 15:** ` `  
  Blank line used to visually separate sections.
- **Line 16:** `FetchContent_MakeAvailable(raylib)`  
  Downloads and adds the dependency to the current build.
- **Line 17:** ` `  
  Blank line used to visually separate sections.
- **Line 18:** `add_executable(TopDownMMOStarter`  
  Starts the definition of the program that will be built.
- **Line 19:** `    src/main.cpp`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 20:** `    src/Game.cpp`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 21:** `    src/World.cpp`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 22:** `    src/Data.cpp`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 23:** `)`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 24:** ` `  
  Blank line used to visually separate sections.
- **Line 25:** `target_include_directories(TopDownMMOStarter PRIVATE include)`  
  Tells the compiler where to look for this project's header files.
- **Line 26:** `target_link_libraries(TopDownMMOStarter PRIVATE raylib)`  
  Links the executable with the libraries it needs.
- **Line 27:** ` `  
  Blank line used to visually separate sections.
- **Line 28:** `if (WIN32)`  
  Starts a Windows-only configuration block.
- **Line 29:** `    target_link_libraries(TopDownMMOStarter PRIVATE winmm)`  
  Links the executable with the libraries it needs.
- **Line 30:** `endif()`  
  Ends the conditional CMake block.

## `include/Types.h`

- **Line 1:** `#pragma once`  
  Prevents this header from being included more than once in the same translation unit.
- **Line 2:** ` `  
  Blank line used to visually separate sections.
- **Line 3:** `#include <raylib.h>`  
  Includes another header so this file can use the types or functions declared there.
- **Line 4:** `#include <string>`  
  Includes another header so this file can use the types or functions declared there.
- **Line 5:** `#include <vector>`  
  Includes another header so this file can use the types or functions declared there.
- **Line 6:** ` `  
  Blank line used to visually separate sections.
- **Line 7:** `struct Weapon {`  
  Begins a plain-old-data style structure used to store game state.
- **Line 8:** `    std::string name;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 9:** `    int damage = 1;`  
  Assigns or initializes data used later in the file.
- **Line 10:** `    float range = 40.0f;`  
  Assigns or initializes data used later in the file.
- **Line 11:** `    float cooldown = 0.35f;`  
  Assigns or initializes data used later in the file.
- **Line 12:** `};`  
  Closes the current block.
- **Line 13:** ` `  
  Blank line used to visually separate sections.
- **Line 14:** `struct InventoryItem {`  
  Begins a plain-old-data style structure used to store game state.
- **Line 15:** `    std::string name;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 16:** `    int count = 0;`  
  Assigns or initializes data used later in the file.
- **Line 17:** `};`  
  Closes the current block.
- **Line 18:** ` `  
  Blank line used to visually separate sections.
- **Line 19:** `struct Quest {`  
  Begins a plain-old-data style structure used to store game state.
- **Line 20:** `    std::string title;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 21:** `    std::string description;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 22:** `    int killsNeeded = 0;`  
  Assigns or initializes data used later in the file.
- **Line 23:** `    int killsDone = 0;`  
  Assigns or initializes data used later in the file.
- **Line 24:** `    bool accepted = false;`  
  Assigns or initializes data used later in the file.
- **Line 25:** `    bool completed = false;`  
  Assigns or initializes data used later in the file.
- **Line 26:** `    bool rewarded = false;`  
  Assigns or initializes data used later in the file.
- **Line 27:** `};`  
  Closes the current block.
- **Line 28:** ` `  
  Blank line used to visually separate sections.
- **Line 29:** `struct Actor {`  
  Begins a plain-old-data style structure used to store game state.
- **Line 30:** `    Vector2 position{};`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 31:** `    Vector2 size{28.0f, 28.0f};`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 32:** `    float speed = 140.0f;`  
  Assigns or initializes data used later in the file.
- **Line 33:** `    int hp = 10;`  
  Assigns or initializes data used later in the file.
- **Line 34:** `    int maxHp = 10;`  
  Assigns or initializes data used later in the file.
- **Line 35:** `    Color color = WHITE;`  
  Assigns or initializes data used later in the file.
- **Line 36:** `};`  
  Closes the current block.
- **Line 37:** ` `  
  Blank line used to visually separate sections.
- **Line 38:** `struct Player : Actor {`  
  Begins a plain-old-data style structure used to store game state.
- **Line 39:** `    int xp = 0;`  
  Assigns or initializes data used later in the file.
- **Line 40:** `    int gold = 0;`  
  Assigns or initializes data used later in the file.
- **Line 41:** `    Weapon weapon{"Rusty Sword", 2, 42.0f, 0.35f};`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 42:** `    std::vector<InventoryItem> inventory{{"Herb", 0}, {"Potion", 1}};`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 43:** `};`  
  Closes the current block.
- **Line 44:** ` `  
  Blank line used to visually separate sections.
- **Line 45:** `struct Enemy : Actor {`  
  Begins a plain-old-data style structure used to store game state.
- **Line 46:** `    std::string name = "Slime";`  
  Assigns or initializes data used later in the file.
- **Line 47:** `    bool alive = true;`  
  Assigns or initializes data used later in the file.
- **Line 48:** `    float attackCooldown = 0.9f;`  
  Assigns or initializes data used later in the file.
- **Line 49:** `    float attackTimer = 0.0f;`  
  Assigns or initializes data used later in the file.
- **Line 50:** `};`  
  Closes the current block.
- **Line 51:** ` `  
  Blank line used to visually separate sections.
- **Line 52:** `struct Npc : Actor {`  
  Begins a plain-old-data style structure used to store game state.
- **Line 53:** `    std::string name;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 54:** `    std::string idleText;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 55:** `    std::string questText;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 56:** `};`  
  Closes the current block.
- **Line 57:** ` `  
  Blank line used to visually separate sections.
- **Line 58:** `struct Drop {`  
  Begins a plain-old-data style structure used to store game state.
- **Line 59:** `    Vector2 position{};`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 60:** `    std::string itemName;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 61:** `    int amount = 1;`  
  Assigns or initializes data used later in the file.
- **Line 62:** `    bool taken = false;`  
  Assigns or initializes data used later in the file.
- **Line 63:** `};`  
  Closes the current block.

## `include/Data.h`

- **Line 1:** `#pragma once`  
  Prevents this header from being included more than once in the same translation unit.
- **Line 2:** ` `  
  Blank line used to visually separate sections.
- **Line 3:** `#include <string>`  
  Includes another header so this file can use the types or functions declared there.
- **Line 4:** `#include <vector>`  
  Includes another header so this file can use the types or functions declared there.
- **Line 5:** ` `  
  Blank line used to visually separate sections.
- **Line 6:** `std::vector<std::string> BuildMap();`  
  Defines the helper that returns the hard-coded tile map.

## `include/World.h`

- **Line 1:** `#pragma once`  
  Prevents this header from being included more than once in the same translation unit.
- **Line 2:** ` `  
  Blank line used to visually separate sections.
- **Line 3:** `#include "Types.h"`  
  Includes another header so this file can use the types or functions declared there.
- **Line 4:** `#include <string>`  
  Includes another header so this file can use the types or functions declared there.
- **Line 5:** `#include <vector>`  
  Includes another header so this file can use the types or functions declared there.
- **Line 6:** ` `  
  Blank line used to visually separate sections.
- **Line 7:** `class World {`  
  Begins a class declaration that groups related data and behavior together.
- **Line 8:** `public:`  
  Marks the following class members as callable from outside the class.
- **Line 9:** `    World();`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 10:** ` `  
  Blank line used to visually separate sections.
- **Line 11:** `    void Update(float dt);`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 12:** `    void Draw() const;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 13:** ` `  
  Blank line used to visually separate sections.
- **Line 14:** `private:`  
  Marks the following class members as internal implementation details.
- **Line 15:** `    void UpdatePlayer(float dt);`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 16:** `    void UpdateEnemies(float dt);`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 17:** `    void HandleCombat(float dt);`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 18:** `    void HandleInteraction();`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 19:** `    void HandleDrops();`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 20:** `    void DrawMap() const;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 21:** `    void DrawHud() const;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 22:** `    void TryPickup(const std::string& itemName, int amount);`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 23:** `    bool IsWall(Vector2 position) const;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 24:** `    bool IsClose(Vector2 a, Vector2 b, float distance) const;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 25:** `    Rectangle RectOf(const Actor& actor) const;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 26:** ` `  
  Blank line used to visually separate sections.
- **Line 27:** `    std::vector<std::string> map_;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 28:** `    int tileSize_ = 32;`  
  Assigns or initializes data used later in the file.
- **Line 29:** `    Player player_;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 30:** `    std::vector<Enemy> enemies_;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 31:** `    std::vector<Npc> npcs_;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 32:** `    std::vector<Drop> drops_;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 33:** `    Quest slimeQuest_;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 34:** `    std::string message_;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 35:** `    float playerAttackTimer_ = 0.0f;`  
  Assigns or initializes data used later in the file.
- **Line 36:** `};`  
  Closes the current block.

## `include/Game.h`

- **Line 1:** `#pragma once`  
  Prevents this header from being included more than once in the same translation unit.
- **Line 2:** ` `  
  Blank line used to visually separate sections.
- **Line 3:** `#include "World.h"`  
  Includes another header so this file can use the types or functions declared there.
- **Line 4:** ` `  
  Blank line used to visually separate sections.
- **Line 5:** `class Game {`  
  Begins a class declaration that groups related data and behavior together.
- **Line 6:** `public:`  
  Marks the following class members as callable from outside the class.
- **Line 7:** `    void Run();`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 8:** ` `  
  Blank line used to visually separate sections.
- **Line 9:** `private:`  
  Marks the following class members as internal implementation details.
- **Line 10:** `    World world_;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 11:** `};`  
  Closes the current block.

## `src/main.cpp`

- **Line 1:** `#include "Game.h"`  
  Includes another header so this file can use the types or functions declared there.
- **Line 2:** ` `  
  Blank line used to visually separate sections.
- **Line 3:** `int main() {`  
  Defines the program entry point.
- **Line 4:** `    Game game;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 5:** `    game.Run();`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 6:** `    return 0;`  
  Returns a value from the current function.
- **Line 7:** `}`  
  Closes the current block.

## `src/Game.cpp`

- **Line 1:** `#include "Game.h"`  
  Includes another header so this file can use the types or functions declared there.
- **Line 2:** `#include <raylib.h>`  
  Includes another header so this file can use the types or functions declared there.
- **Line 3:** ` `  
  Blank line used to visually separate sections.
- **Line 4:** `void Game::Run() {`  
  Defines the function that creates the window and runs the frame loop.
- **Line 5:** `    const int screenWidth = 1024;`  
  Assigns or initializes data used later in the file.
- **Line 6:** `    const int screenHeight = 768;`  
  Assigns or initializes data used later in the file.
- **Line 7:** ` `  
  Blank line used to visually separate sections.
- **Line 8:** `    InitWindow(screenWidth, screenHeight, "TopDownMMOStarter");`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 9:** `    SetTargetFPS(60);`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 10:** ` `  
  Blank line used to visually separate sections.
- **Line 11:** `    while (!WindowShouldClose()) {`  
  Starts a loop that keeps running until the condition becomes false.
- **Line 12:** `        float dt = GetFrameTime();`  
  Assigns or initializes data used later in the file.
- **Line 13:** `        world_.Update(dt);`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 14:** ` `  
  Blank line used to visually separate sections.
- **Line 15:** `        BeginDrawing();`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 16:** `        ClearBackground(BLACK);`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 17:** `        world_.Draw();`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 18:** `        EndDrawing();`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 19:** `    }`  
  Closes the current block.
- **Line 20:** ` `  
  Blank line used to visually separate sections.
- **Line 21:** `    CloseWindow();`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 22:** `}`  
  Closes the current block.

## `src/Data.cpp`

- **Line 1:** `#include "Data.h"`  
  Includes another header so this file can use the types or functions declared there.
- **Line 2:** ` `  
  Blank line used to visually separate sections.
- **Line 3:** `std::vector<std::string> BuildMap() {`  
  Defines the helper that returns the hard-coded tile map.
- **Line 4:** `    return {`  
  Returns a value from the current function.
- **Line 5:** `        "################################",`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 6:** `        "#..............#...............#",`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 7:** `        "#..P...........#......S........#",`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 8:** `        "#..............#...............#",`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 9:** `        "#......######..#....#####......#",`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 10:** `        "#..............#...............#",`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 11:** `        "#..............#.........S.....#",`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 12:** `        "#..............#...............#",`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 13:** `        "#....E.........................#",`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 14:** `        "#...........###########........#",`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 15:** `        "#..............................#",`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 16:** `        "#.....S....................S...#",`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 17:** `        "#..............................#",`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 18:** `        "#..............######..........#",`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 19:** `        "#..............#....#..........#",`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 20:** `        "#..............#....#..........#",`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 21:** `        "#..............#....#..........#",`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 22:** `        "#..............#....#..........#",`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 23:** `        "#..............#....#..........#",`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 24:** `        "################################"`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 25:** `    };`  
  Closes the current block.
- **Line 26:** `}`  
  Closes the current block.

## `src/World.cpp`

- **Line 1:** `#include "World.h"`  
  Includes another header so this file can use the types or functions declared there.
- **Line 2:** `#include "Data.h"`  
  Includes another header so this file can use the types or functions declared there.
- **Line 3:** `#include <algorithm>`  
  Includes another header so this file can use the types or functions declared there.
- **Line 4:** `#include <cmath>`  
  Includes another header so this file can use the types or functions declared there.
- **Line 5:** `#include <sstream>`  
  Includes another header so this file can use the types or functions declared there.
- **Line 6:** ` `  
  Blank line used to visually separate sections.
- **Line 7:** `namespace {`  
  Starts an unnamed namespace so helper functions stay local to this source file.
- **Line 8:** `Vector2 NormalizeOrZero(Vector2 value) {`  
  Defines a helper that turns a vector into unit length unless it is too small.
- **Line 9:** `    float length = std::sqrt(value.x * value.x + value.y * value.y);`  
  Uses square root, usually to compute a distance or vector length.
- **Line 10:** `    if (length <= 0.0001f) {`  
  Checks a condition before deciding whether the block below should run.
- **Line 11:** `        return Vector2{0.0f, 0.0f};`  
  Returns a value from the current function.
- **Line 12:** `    }`  
  Closes the current block.
- **Line 13:** `    return Vector2{value.x / length, value.y / length};`  
  Returns a value from the current function.
- **Line 14:** `}`  
  Closes the current block.
- **Line 15:** `}`  
  Closes the current block.
- **Line 16:** ` `  
  Blank line used to visually separate sections.
- **Line 17:** `World::World() {`  
  Defines the world constructor that sets up the map and starting entities.
- **Line 18:** `    map_ = BuildMap();`  
  Assigns or initializes data used later in the file.
- **Line 19:** `    slimeQuest_.title = "Slime Cleanup";`  
  Assigns or initializes data used later in the file.
- **Line 20:** `    slimeQuest_.description = "Defeat 3 slimes for the village elder.";`  
  Assigns or initializes data used later in the file.
- **Line 21:** `    slimeQuest_.killsNeeded = 3;`  
  Assigns or initializes data used later in the file.
- **Line 22:** `    message_ = "Walk with WASD. E talks or loots. Space attacks.";`  
  Assigns or initializes data used later in the file.
- **Line 23:** ` `  
  Blank line used to visually separate sections.
- **Line 24:** `    for (int y = 0; y < static_cast<int>(map_.size()); ++y) {`  
  Starts a loop that repeats the enclosed code for each matching iteration.
- **Line 25:** `        for (int x = 0; x < static_cast<int>(map_[y].size()); ++x) {`  
  Starts a loop that repeats the enclosed code for each matching iteration.
- **Line 26:** `            Vector2 worldPos{static_cast<float>(x * tileSize_ + 2), static_cast<float>(y * tileSize_ + 2)};`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 27:** `            if (map_[y][x] == 'P') {`  
  Checks a condition before deciding whether the block below should run.
- **Line 28:** `                player_.position = worldPos;`  
  Assigns a position value used by gameplay or rendering.
- **Line 29:** `                player_.color = BLUE;`  
  Assigns or initializes data used later in the file.
- **Line 30:** `                player_.hp = 20;`  
  Assigns or initializes data used later in the file.
- **Line 31:** `                player_.maxHp = 20;`  
  Assigns or initializes data used later in the file.
- **Line 32:** `            } else if (map_[y][x] == 'S') {`  
  Opens a new scope block for the declaration or statement above.
- **Line 33:** `                Enemy slime;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 34:** `                slime.position = worldPos;`  
  Assigns a position value used by gameplay or rendering.
- **Line 35:** `                slime.color = GREEN;`  
  Assigns or initializes data used later in the file.
- **Line 36:** `                slime.name = "Slime";`  
  Assigns or initializes data used later in the file.
- **Line 37:** `                slime.hp = 6;`  
  Assigns or initializes data used later in the file.
- **Line 38:** `                slime.maxHp = 6;`  
  Assigns or initializes data used later in the file.
- **Line 39:** `                slime.speed = 65.0f;`  
  Assigns or initializes data used later in the file.
- **Line 40:** `                enemies_.push_back(slime);`  
  Appends a new element to the end of the vector.
- **Line 41:** `            } else if (map_[y][x] == 'E') {`  
  Opens a new scope block for the declaration or statement above.
- **Line 42:** `                Npc elder;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 43:** `                elder.position = worldPos;`  
  Assigns a position value used by gameplay or rendering.
- **Line 44:** `                elder.color = GOLD;`  
  Assigns or initializes data used later in the file.
- **Line 45:** `                elder.hp = 1;`  
  Assigns or initializes data used later in the file.
- **Line 46:** `                elder.maxHp = 1;`  
  Assigns or initializes data used later in the file.
- **Line 47:** `                elder.name = "Elder Rowan";`  
  Assigns or initializes data used later in the file.
- **Line 48:** `                elder.idleText = "The road is safer than it used to be.";`  
  Assigns or initializes data used later in the file.
- **Line 49:** `                elder.questText = "Please clear out 3 slimes near the village.";`  
  Assigns or initializes data used later in the file.
- **Line 50:** `                npcs_.push_back(elder);`  
  Appends a new element to the end of the vector.
- **Line 51:** `            }`  
  Closes the current block.
- **Line 52:** `        }`  
  Closes the current block.
- **Line 53:** `    }`  
  Closes the current block.
- **Line 54:** `}`  
  Closes the current block.
- **Line 55:** ` `  
  Blank line used to visually separate sections.
- **Line 56:** `void World::Update(float dt) {`  
  Defines the per-frame world update function.
- **Line 57:** `    playerAttackTimer_ -= dt;`  
  Assigns or initializes data used later in the file.
- **Line 58:** `    UpdatePlayer(dt);`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 59:** `    UpdateEnemies(dt);`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 60:** `    HandleCombat(dt);`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 61:** `    HandleInteraction();`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 62:** `    HandleDrops();`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 63:** `}`  
  Closes the current block.
- **Line 64:** ` `  
  Blank line used to visually separate sections.
- **Line 65:** `void World::Draw() const {`  
  Defines the rendering function for the world.
- **Line 66:** `    DrawMap();`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 67:** ` `  
  Blank line used to visually separate sections.
- **Line 68:** `    for (const auto& npc : npcs_) {`  
  Starts a loop that repeats the enclosed code for each matching iteration.
- **Line 69:** `        DrawRectangleV(npc.position, npc.size, npc.color);`  
  Draws a simple shape for the current object or UI element.
- **Line 70:** `        DrawText(npc.name.c_str(), static_cast<int>(npc.position.x) - 10, static_cast<int>(npc.position.y) - 18, 12, WHITE);`  
  Draws text to the screen.
- **Line 71:** `    }`  
  Closes the current block.
- **Line 72:** ` `  
  Blank line used to visually separate sections.
- **Line 73:** `    for (const auto& enemy : enemies_) {`  
  Starts a loop that repeats the enclosed code for each matching iteration.
- **Line 74:** `        if (!enemy.alive) {`  
  Checks a condition before deciding whether the block below should run.
- **Line 75:** `            continue;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 76:** `        }`  
  Closes the current block.
- **Line 77:** `        DrawRectangleV(enemy.position, enemy.size, enemy.color);`  
  Draws a simple shape for the current object or UI element.
- **Line 78:** `        DrawRectangle(static_cast<int>(enemy.position.x), static_cast<int>(enemy.position.y) - 8, 28, 4, DARKGRAY);`  
  Draws a simple shape for the current object or UI element.
- **Line 79:** `        DrawRectangle(static_cast<int>(enemy.position.x), static_cast<int>(enemy.position.y) - 8, 28 * enemy.hp / enemy.maxHp, 4, RED);`  
  Draws a simple shape for the current object or UI element.
- **Line 80:** `    }`  
  Closes the current block.
- **Line 81:** ` `  
  Blank line used to visually separate sections.
- **Line 82:** `    for (const auto& drop : drops_) {`  
  Starts a loop that repeats the enclosed code for each matching iteration.
- **Line 83:** `        if (!drop.taken) {`  
  Checks a condition before deciding whether the block below should run.
- **Line 84:** `            DrawCircle(static_cast<int>(drop.position.x) + 12, static_cast<int>(drop.position.y) + 12, 8.0f, SKYBLUE);`  
  Draws a simple shape for the current object or UI element.
- **Line 85:** `        }`  
  Closes the current block.
- **Line 86:** `    }`  
  Closes the current block.
- **Line 87:** ` `  
  Blank line used to visually separate sections.
- **Line 88:** `    DrawRectangleV(player_.position, player_.size, player_.color);`  
  Draws a simple shape for the current object or UI element.
- **Line 89:** `    DrawHud();`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 90:** `}`  
  Closes the current block.
- **Line 91:** ` `  
  Blank line used to visually separate sections.
- **Line 92:** `void World::UpdatePlayer(float dt) {`  
  Defines player movement and collision handling.
- **Line 93:** `    Vector2 move{};`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 94:** ` `  
  Blank line used to visually separate sections.
- **Line 95:** `    if (IsKeyDown(KEY_W)) move.y -= 1.0f;`  
  Checks a condition before deciding whether the block below should run.
- **Line 96:** `    if (IsKeyDown(KEY_S)) move.y += 1.0f;`  
  Checks a condition before deciding whether the block below should run.
- **Line 97:** `    if (IsKeyDown(KEY_A)) move.x -= 1.0f;`  
  Checks a condition before deciding whether the block below should run.
- **Line 98:** `    if (IsKeyDown(KEY_D)) move.x += 1.0f;`  
  Checks a condition before deciding whether the block below should run.
- **Line 99:** ` `  
  Blank line used to visually separate sections.
- **Line 100:** `    move = NormalizeOrZero(move);`  
  Assigns or initializes data used later in the file.
- **Line 101:** `    Vector2 next = player_.position;`  
  Assigns a position value used by gameplay or rendering.
- **Line 102:** `    next.x += move.x * player_.speed * dt;`  
  Assigns or initializes data used later in the file.
- **Line 103:** `    next.y += move.y * player_.speed * dt;`  
  Assigns or initializes data used later in the file.
- **Line 104:** ` `  
  Blank line used to visually separate sections.
- **Line 105:** `    if (!IsWall(Vector2{next.x, player_.position.y})) {`  
  Checks a condition before deciding whether the block below should run.
- **Line 106:** `        player_.position.x = next.x;`  
  Assigns a position value used by gameplay or rendering.
- **Line 107:** `    }`  
  Closes the current block.
- **Line 108:** `    if (!IsWall(Vector2{player_.position.x, next.y})) {`  
  Checks a condition before deciding whether the block below should run.
- **Line 109:** `        player_.position.y = next.y;`  
  Assigns a position value used by gameplay or rendering.
- **Line 110:** `    }`  
  Closes the current block.
- **Line 111:** `}`  
  Closes the current block.
- **Line 112:** ` `  
  Blank line used to visually separate sections.
- **Line 113:** `void World::UpdateEnemies(float dt) {`  
  Defines enemy pursuit AI and enemy attacks.
- **Line 114:** `    for (auto& enemy : enemies_) {`  
  Starts a loop that repeats the enclosed code for each matching iteration.
- **Line 115:** `        if (!enemy.alive) {`  
  Checks a condition before deciding whether the block below should run.
- **Line 116:** `            continue;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 117:** `        }`  
  Closes the current block.
- **Line 118:** ` `  
  Blank line used to visually separate sections.
- **Line 119:** `        enemy.attackTimer -= dt;`  
  Assigns or initializes data used later in the file.
- **Line 120:** `        Vector2 toPlayer{player_.position.x - enemy.position.x, player_.position.y - enemy.position.y};`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 121:** `        float distance = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);`  
  Uses square root, usually to compute a distance or vector length.
- **Line 122:** ` `  
  Blank line used to visually separate sections.
- **Line 123:** `        if (distance > 28.0f && distance < 220.0f) {`  
  Checks a condition before deciding whether the block below should run.
- **Line 124:** `            Vector2 direction = NormalizeOrZero(toPlayer);`  
  Assigns or initializes data used later in the file.
- **Line 125:** `            Vector2 next = enemy.position;`  
  Assigns a position value used by gameplay or rendering.
- **Line 126:** `            next.x += direction.x * enemy.speed * dt;`  
  Assigns or initializes data used later in the file.
- **Line 127:** `            next.y += direction.y * enemy.speed * dt;`  
  Assigns or initializes data used later in the file.
- **Line 128:** ` `  
  Blank line used to visually separate sections.
- **Line 129:** `            if (!IsWall(next)) {`  
  Checks a condition before deciding whether the block below should run.
- **Line 130:** `                enemy.position = next;`  
  Assigns a position value used by gameplay or rendering.
- **Line 131:** `            }`  
  Closes the current block.
- **Line 132:** `        }`  
  Closes the current block.
- **Line 133:** ` `  
  Blank line used to visually separate sections.
- **Line 134:** `        if (distance <= 34.0f && enemy.attackTimer <= 0.0f) {`  
  Checks a condition before deciding whether the block below should run.
- **Line 135:** `            player_.hp = std::max(0, player_.hp - 1);`  
  Uses the larger of two values.
- **Line 136:** `            message_ = enemy.name + " hits you for 1 damage.";`  
  Assigns or initializes data used later in the file.
- **Line 137:** `            enemy.attackTimer = enemy.attackCooldown;`  
  Assigns or initializes data used later in the file.
- **Line 138:** `        }`  
  Closes the current block.
- **Line 139:** `    }`  
  Closes the current block.
- **Line 140:** `}`  
  Closes the current block.
- **Line 141:** ` `  
  Blank line used to visually separate sections.
- **Line 142:** `void World::HandleCombat(float) {`  
  Defines the player's attack resolution.
- **Line 143:** `    if (!IsKeyPressed(KEY_SPACE) || playerAttackTimer_ > 0.0f) {`  
  Checks a condition before deciding whether the block below should run.
- **Line 144:** `        return;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 145:** `    }`  
  Closes the current block.
- **Line 146:** ` `  
  Blank line used to visually separate sections.
- **Line 147:** `    playerAttackTimer_ = player_.weapon.cooldown;`  
  Assigns or initializes data used later in the file.
- **Line 148:** ` `  
  Blank line used to visually separate sections.
- **Line 149:** `    for (auto& enemy : enemies_) {`  
  Starts a loop that repeats the enclosed code for each matching iteration.
- **Line 150:** `        if (!enemy.alive) {`  
  Checks a condition before deciding whether the block below should run.
- **Line 151:** `            continue;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 152:** `        }`  
  Closes the current block.
- **Line 153:** ` `  
  Blank line used to visually separate sections.
- **Line 154:** `        float dx = enemy.position.x - player_.position.x;`  
  Assigns a position value used by gameplay or rendering.
- **Line 155:** `        float dy = enemy.position.y - player_.position.y;`  
  Assigns a position value used by gameplay or rendering.
- **Line 156:** `        float distance = std::sqrt(dx * dx + dy * dy);`  
  Uses square root, usually to compute a distance or vector length.
- **Line 157:** ` `  
  Blank line used to visually separate sections.
- **Line 158:** `        if (distance <= player_.weapon.range) {`  
  Checks a condition before deciding whether the block below should run.
- **Line 159:** `            enemy.hp -= player_.weapon.damage;`  
  Assigns or initializes data used later in the file.
- **Line 160:** `            message_ = "You hit " + enemy.name + " with " + player_.weapon.name + ".";`  
  Assigns or initializes data used later in the file.
- **Line 161:** ` `  
  Blank line used to visually separate sections.
- **Line 162:** `            if (enemy.hp <= 0) {`  
  Checks a condition before deciding whether the block below should run.
- **Line 163:** `                enemy.alive = false;`  
  Assigns or initializes data used later in the file.
- **Line 164:** `                player_.xp += 10;`  
  Assigns or initializes data used later in the file.
- **Line 165:** `                player_.gold += 5;`  
  Assigns or initializes data used later in the file.
- **Line 166:** `                drops_.push_back(Drop{enemy.position, "Herb", 1, false});`  
  Appends a new element to the end of the vector.
- **Line 167:** `                message_ = "You defeated a " + enemy.name + " and found an Herb.";`  
  Assigns or initializes data used later in the file.
- **Line 168:** ` `  
  Blank line used to visually separate sections.
- **Line 169:** `                if (slimeQuest_.accepted && !slimeQuest_.completed) {`  
  Checks a condition before deciding whether the block below should run.
- **Line 170:** `                    slimeQuest_.killsDone += 1;`  
  Assigns or initializes data used later in the file.
- **Line 171:** `                    if (slimeQuest_.killsDone >= slimeQuest_.killsNeeded) {`  
  Checks a condition before deciding whether the block below should run.
- **Line 172:** `                        slimeQuest_.completed = true;`  
  Assigns or initializes data used later in the file.
- **Line 173:** `                        message_ = "Quest complete. Return to Elder Rowan.";`  
  Assigns or initializes data used later in the file.
- **Line 174:** `                    }`  
  Closes the current block.
- **Line 175:** `                }`  
  Closes the current block.
- **Line 176:** `            }`  
  Closes the current block.
- **Line 177:** `        }`  
  Closes the current block.
- **Line 178:** `    }`  
  Closes the current block.
- **Line 179:** `}`  
  Closes the current block.
- **Line 180:** ` `  
  Blank line used to visually separate sections.
- **Line 181:** `void World::HandleInteraction() {`  
  Defines talking to NPCs and picking up items.
- **Line 182:** `    if (!IsKeyPressed(KEY_E)) {`  
  Checks a condition before deciding whether the block below should run.
- **Line 183:** `        return;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 184:** `    }`  
  Closes the current block.
- **Line 185:** ` `  
  Blank line used to visually separate sections.
- **Line 186:** `    for (auto& npc : npcs_) {`  
  Starts a loop that repeats the enclosed code for each matching iteration.
- **Line 187:** `        if (!IsClose(player_.position, npc.position, 52.0f)) {`  
  Checks a condition before deciding whether the block below should run.
- **Line 188:** `            continue;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 189:** `        }`  
  Closes the current block.
- **Line 190:** ` `  
  Blank line used to visually separate sections.
- **Line 191:** `        if (!slimeQuest_.accepted) {`  
  Checks a condition before deciding whether the block below should run.
- **Line 192:** `            slimeQuest_.accepted = true;`  
  Assigns or initializes data used later in the file.
- **Line 193:** `            message_ = npc.name + ": " + npc.questText;`  
  Assigns or initializes data used later in the file.
- **Line 194:** `            return;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 195:** `        }`  
  Closes the current block.
- **Line 196:** ` `  
  Blank line used to visually separate sections.
- **Line 197:** `        if (slimeQuest_.completed && !slimeQuest_.rewarded) {`  
  Checks a condition before deciding whether the block below should run.
- **Line 198:** `            slimeQuest_.rewarded = true;`  
  Assigns or initializes data used later in the file.
- **Line 199:** `            player_.gold += 25;`  
  Assigns or initializes data used later in the file.
- **Line 200:** `            player_.weapon = Weapon{"Bronze Blade", 4, 48.0f, 0.28f};`  
  Assigns or initializes data used later in the file.
- **Line 201:** `            message_ = npc.name + ": Thank you. Take this Bronze Blade and 25 gold.";`  
  Assigns or initializes data used later in the file.
- **Line 202:** `            return;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 203:** `        }`  
  Closes the current block.
- **Line 204:** ` `  
  Blank line used to visually separate sections.
- **Line 205:** `        message_ = npc.name + ": " + npc.idleText;`  
  Assigns or initializes data used later in the file.
- **Line 206:** `        return;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 207:** `    }`  
  Closes the current block.
- **Line 208:** ` `  
  Blank line used to visually separate sections.
- **Line 209:** `    for (auto& drop : drops_) {`  
  Starts a loop that repeats the enclosed code for each matching iteration.
- **Line 210:** `        if (!drop.taken && IsClose(player_.position, drop.position, 40.0f)) {`  
  Checks a condition before deciding whether the block below should run.
- **Line 211:** `            drop.taken = true;`  
  Assigns or initializes data used later in the file.
- **Line 212:** `            TryPickup(drop.itemName, drop.amount);`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 213:** `            message_ = "Picked up " + drop.itemName + " x" + std::to_string(drop.amount) + ".";`  
  Assigns or initializes data used later in the file.
- **Line 214:** `            return;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 215:** `        }`  
  Closes the current block.
- **Line 216:** `    }`  
  Closes the current block.
- **Line 217:** `}`  
  Closes the current block.
- **Line 218:** ` `  
  Blank line used to visually separate sections.
- **Line 219:** `void World::HandleDrops() {`  
  Defines cleanup for collected drop objects.
- **Line 220:** `    drops_.erase(`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 221:** `        std::remove_if(drops_.begin(), drops_.end(), [](const Drop& drop) { return drop.taken; }),`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 222:** `        drops_.end()`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 223:** `    );`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 224:** `}`  
  Closes the current block.
- **Line 225:** ` `  
  Blank line used to visually separate sections.
- **Line 226:** `void World::DrawMap() const {`  
  Defines map rendering.
- **Line 227:** `    for (int y = 0; y < static_cast<int>(map_.size()); ++y) {`  
  Starts a loop that repeats the enclosed code for each matching iteration.
- **Line 228:** `        for (int x = 0; x < static_cast<int>(map_[y].size()); ++x) {`  
  Starts a loop that repeats the enclosed code for each matching iteration.
- **Line 229:** `            Rectangle tile{static_cast<float>(x * tileSize_), static_cast<float>(y * tileSize_), static_cast<float>(tileSize_), static_cast<float>(tileSize_)};`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 230:** `            bool isWall = map_[y][x] == '#';`  
  Assigns or initializes data used later in the file.
- **Line 231:** `            DrawRectangleRec(tile, isWall ? DARKGRAY : Color{30, 80, 35, 255});`  
  Draws a simple shape for the current object or UI element.
- **Line 232:** `            DrawRectangleLinesEx(tile, 1.0f, Fade(BLACK, 0.2f));`  
  Draws a simple shape for the current object or UI element.
- **Line 233:** `        }`  
  Closes the current block.
- **Line 234:** `    }`  
  Closes the current block.
- **Line 235:** `}`  
  Closes the current block.
- **Line 236:** ` `  
  Blank line used to visually separate sections.
- **Line 237:** `void World::DrawHud() const {`  
  Defines the lower-screen HUD drawing.
- **Line 238:** `    DrawRectangle(0, 640, 1024, 128, Fade(BLACK, 0.75f));`  
  Draws a simple shape for the current object or UI element.
- **Line 239:** ` `  
  Blank line used to visually separate sections.
- **Line 240:** `    std::ostringstream stats;`  
  Creates a string builder used to assemble display text.
- **Line 241:** `    stats << "HP: " << player_.hp << "/" << player_.maxHp`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 242:** `          << "   XP: " << player_.xp`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 243:** `          << "   Gold: " << player_.gold`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 244:** `          << "   Weapon: " << player_.weapon.name`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 245:** `          << " (DMG " << player_.weapon.damage << ")";`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 246:** `    DrawText(stats.str().c_str(), 16, 652, 20, WHITE);`  
  Draws text to the screen.
- **Line 247:** ` `  
  Blank line used to visually separate sections.
- **Line 248:** `    std::ostringstream quest;`  
  Creates a string builder used to assemble display text.
- **Line 249:** `    quest << "Quest: " << slimeQuest_.title << " ["`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 250:** `          << slimeQuest_.killsDone << "/" << slimeQuest_.killsNeeded << "]";`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 251:** `    if (!slimeQuest_.accepted) {`  
  Checks a condition before deciding whether the block below should run.
- **Line 252:** `        quest.str("");`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 253:** `        quest << "Quest: Talk to Elder Rowan to get your first quest.";`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 254:** `    }`  
  Closes the current block.
- **Line 255:** `    if (slimeQuest_.rewarded) {`  
  Checks a condition before deciding whether the block below should run.
- **Line 256:** `        quest.str("");`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 257:** `        quest << "Quest: Completed.";`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 258:** `    }`  
  Closes the current block.
- **Line 259:** `    DrawText(quest.str().c_str(), 16, 680, 20, YELLOW);`  
  Draws text to the screen.
- **Line 260:** ` `  
  Blank line used to visually separate sections.
- **Line 261:** `    std::ostringstream inventory;`  
  Creates a string builder used to assemble display text.
- **Line 262:** `    inventory << "Inventory: ";`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 263:** `    for (std::size_t i = 0; i < player_.inventory.size(); ++i) {`  
  Starts a loop that repeats the enclosed code for each matching iteration.
- **Line 264:** `        inventory << player_.inventory[i].name << " x" << player_.inventory[i].count;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 265:** `        if (i + 1 < player_.inventory.size()) {`  
  Checks a condition before deciding whether the block below should run.
- **Line 266:** `            inventory << ", ";`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 267:** `        }`  
  Closes the current block.
- **Line 268:** `    }`  
  Closes the current block.
- **Line 269:** `    DrawText(inventory.str().c_str(), 16, 708, 20, SKYBLUE);`  
  Draws text to the screen.
- **Line 270:** `    DrawText(message_.c_str(), 16, 736, 18, LIGHTGRAY);`  
  Draws text to the screen.
- **Line 271:** `}`  
  Closes the current block.
- **Line 272:** ` `  
  Blank line used to visually separate sections.
- **Line 273:** `void World::TryPickup(const std::string& itemName, int amount) {`  
  Defines inventory insertion for collected items.
- **Line 274:** `    for (auto& item : player_.inventory) {`  
  Starts a loop that repeats the enclosed code for each matching iteration.
- **Line 275:** `        if (item.name == itemName) {`  
  Checks a condition before deciding whether the block below should run.
- **Line 276:** `            item.count += amount;`  
  Assigns or initializes data used later in the file.
- **Line 277:** `            return;`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 278:** `        }`  
  Closes the current block.
- **Line 279:** `    }`  
  Closes the current block.
- **Line 280:** `    player_.inventory.push_back(InventoryItem{itemName, amount});`  
  Appends a new element to the end of the vector.
- **Line 281:** `}`  
  Closes the current block.
- **Line 282:** ` `  
  Blank line used to visually separate sections.
- **Line 283:** `bool World::IsWall(Vector2 position) const {`  
  Defines tile collision testing.
- **Line 284:** `    int left = static_cast<int>(position.x) / tileSize_;`  
  Assigns a position value used by gameplay or rendering.
- **Line 285:** `    int top = static_cast<int>(position.y) / tileSize_;`  
  Assigns a position value used by gameplay or rendering.
- **Line 286:** `    int right = static_cast<int>(position.x + player_.size.x - 1) / tileSize_;`  
  Assigns a position value used by gameplay or rendering.
- **Line 287:** `    int bottom = static_cast<int>(position.y + player_.size.y - 1) / tileSize_;`  
  Assigns a position value used by gameplay or rendering.
- **Line 288:** ` `  
  Blank line used to visually separate sections.
- **Line 289:** `    auto blocked = [&](int x, int y) {`  
  Opens a new scope block for the declaration or statement above.
- **Line 290:** `        if (y < 0 || y >= static_cast<int>(map_.size())) return true;`  
  Checks a condition before deciding whether the block below should run.
- **Line 291:** `        if (x < 0 || x >= static_cast<int>(map_[y].size())) return true;`  
  Checks a condition before deciding whether the block below should run.
- **Line 292:** `        return map_[y][x] == '#';`  
  Returns a value from the current function.
- **Line 293:** `    };`  
  Closes the current block.
- **Line 294:** ` `  
  Blank line used to visually separate sections.
- **Line 295:** `    return blocked(left, top) || blocked(right, top) || blocked(left, bottom) || blocked(right, bottom);`  
  Returns a value from the current function.
- **Line 296:** `}`  
  Closes the current block.
- **Line 297:** ` `  
  Blank line used to visually separate sections.
- **Line 298:** `bool World::IsClose(Vector2 a, Vector2 b, float distance) const {`  
  Defines a distance helper for interaction checks.
- **Line 299:** `    float dx = a.x - b.x;`  
  Assigns or initializes data used later in the file.
- **Line 300:** `    float dy = a.y - b.y;`  
  Assigns or initializes data used later in the file.
- **Line 301:** `    return std::sqrt(dx * dx + dy * dy) <= distance;`  
  Returns a value from the current function.
- **Line 302:** `}`  
  Closes the current block.
- **Line 303:** ` `  
  Blank line used to visually separate sections.
- **Line 304:** `Rectangle World::RectOf(const Actor& actor) const {`  
  Defines a helper that converts an actor into a rectangle.
- **Line 305:** `    return Rectangle{actor.position.x, actor.position.y, actor.size.x, actor.size.y};`  
  Returns a value from the current function.
- **Line 306:** `}`  
  Closes the current block.

## `README.md`

- **Line 1:** `# TopDownMMOStarter`  
  This is a Markdown heading introducing a new section.
- **Line 2:** ` `  
  Blank line used to visually separate sections.
- **Line 3:** `A **small C++ top-down action-RPG/MMO-style starter repo** built with **raylib**.`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 4:** ` `  
  Blank line used to visually separate sections.
- **Line 5:** `This is **not** a real production MMO server stack. It is a **playable single-process prototype** that gives you the foundation for:`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 6:** `- player movement`  
  This bullet lists a feature or explanation item.
- **Line 7:** `- enemies`  
  This bullet lists a feature or explanation item.
- **Line 8:** `- weapons`  
  This bullet lists a feature or explanation item.
- **Line 9:** `- quests`  
  This bullet lists a feature or explanation item.
- **Line 10:** `- NPC interaction`  
  This bullet lists a feature or explanation item.
- **Line 11:** `- drops and inventory`  
  This bullet lists a feature or explanation item.
- **Line 12:** `- HUD and combat loop`  
  This bullet lists a feature or explanation item.
- **Line 13:** ` `  
  Blank line used to visually separate sections.
- **Line 14:** `## Controls`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 15:** `- **WASD**: move`  
  This bullet lists a feature or explanation item.
- **Line 16:** `- **Space**: attack`  
  This bullet lists a feature or explanation item.
- **Line 17:** `- **E**: talk / loot`  
  This bullet lists a feature or explanation item.
- **Line 18:** `- **Esc**: close window`  
  This bullet lists a feature or explanation item.
- **Line 19:** ` `  
  Blank line used to visually separate sections.
- **Line 20:** `## What is included`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 21:** `- a tile-based map`  
  This bullet lists a feature or explanation item.
- **Line 22:** `- one quest-giver NPC`  
  This bullet lists a feature or explanation item.
- **Line 23:** `- several slimes`  
  This bullet lists a feature or explanation item.
- **Line 24:** `- a starting weapon and a quest reward weapon`  
  This bullet lists a feature or explanation item.
- **Line 25:** `- XP, gold, item drops, and inventory counts`  
  This bullet lists a feature or explanation item.
- **Line 26:** `- a clean CMake project`  
  This bullet lists a feature or explanation item.
- **Line 27:** ` `  
  Blank line used to visually separate sections.
- **Line 28:** `## What is NOT included yet`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 29:** `- networking`  
  This bullet lists a feature or explanation item.
- **Line 30:** `- login/account services`  
  This bullet lists a feature or explanation item.
- **Line 31:** `- database persistence`  
  This bullet lists a feature or explanation item.
- **Line 32:** `- animation system`  
  This bullet lists a feature or explanation item.
- **Line 33:** `- multiplayer synchronization`  
  This bullet lists a feature or explanation item.
- **Line 34:** `- large world streaming`  
  This bullet lists a feature or explanation item.
- **Line 35:** `- crafting, parties, guilds, trading, or chat server`  
  This bullet lists a feature or explanation item.
- **Line 36:** ` `  
  Blank line used to visually separate sections.
- **Line 37:** `## Build on Windows`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 38:** ` `  
  Blank line used to visually separate sections.
- **Line 39:** `### Option 1: Visual Studio 2022`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 40:** `\`\`\`powershell`  
  Starts or ends a fenced code block in Markdown.
- **Line 41:** `cmake -S . -B build`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 42:** `cmake --build build --config Release`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 43:** `.\build\Release\TopDownMMOStarter.exe`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 44:** `\`\`\``  
  Starts or ends a fenced code block in Markdown.
- **Line 45:** ` `  
  Blank line used to visually separate sections.
- **Line 46:** `### Option 2: MinGW`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 47:** `\`\`\`powershell`  
  Starts or ends a fenced code block in Markdown.
- **Line 48:** `cmake -S . -B build -G "MinGW Makefiles"`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 49:** `cmake --build build`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 50:** `.\build\TopDownMMOStarter.exe`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 51:** `\`\`\``  
  Starts or ends a fenced code block in Markdown.
- **Line 52:** ` `  
  Blank line used to visually separate sections.
- **Line 53:** `## Project layout`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 54:** `\`\`\`text`  
  Starts or ends a fenced code block in Markdown.
- **Line 55:** `TopDownMMOStarter/`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 56:** `├── CMakeLists.txt`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 57:** `├── README.md`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 58:** `├── include/`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 59:** `│   ├── Data.h`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 60:** `│   ├── Game.h`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 61:** `│   ├── Types.h`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 62:** `│   └── World.h`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 63:** `├── src/`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 64:** `│   ├── Data.cpp`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 65:** `│   ├── Game.cpp`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 66:** `│   ├── World.cpp`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 67:** `│   └── main.cpp`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 68:** `└── docs/`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 69:** `    └── LINE_BY_LINE_GUIDE.md`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 70:** `\`\`\``  
  Starts or ends a fenced code block in Markdown.
- **Line 71:** ` `  
  Blank line used to visually separate sections.
- **Line 72:** `## Architecture overview`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 73:** `- \`main.cpp\` creates the game object and starts the loop.`  
  This bullet lists a feature or explanation item.
- **Line 74:** `- \`Game.cpp\` owns the window and frame loop.`  
  This bullet lists a feature or explanation item.
- **Line 75:** `- \`World.cpp\` contains the actual game logic.`  
  This bullet lists a feature or explanation item.
- **Line 76:** `- \`Types.h\` stores the main data models.`  
  This bullet lists a feature or explanation item.
- **Line 77:** `- \`Data.cpp\` builds the tile map.`  
  This bullet lists a feature or explanation item.
- **Line 78:** ` `  
  Blank line used to visually separate sections.
- **Line 79:** `## How to grow this into a real MMO`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 80:** `1. Split client and server.`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 81:** `2. Move entity state to an authoritative server.`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 82:** `3. Store player inventory, quests, and stats in a database.`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 83:** `4. Add packet serialization and interpolation.`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 84:** `5. Add content pipelines for maps, NPCs, quests, and items.`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 85:** `6. Replace hard-coded data with JSON, Lua, YAML, or SQL-backed content.`  
  This line contributes normal structure or logic to the surrounding block.
- **Line 86:** `7. Add animation, pathfinding, and zone loading.`  
  This line contributes normal structure or logic to the surrounding block.