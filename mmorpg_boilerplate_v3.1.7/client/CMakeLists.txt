cmake_minimum_required(VERSION 3.16)
project(TopDownMMOClient LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)

FetchContent_MakeAvailable(raylib)

add_executable(TopDownMMOClient
    src/app/main.cpp
    src/app/Game.cpp
    src/network/NetworkClient.cpp
    src/game/world/World.cpp
    src/game/world/World_Update.cpp
    src/game/world/World_Draw.cpp
    src/game/world/World_UI.cpp
    src/game/world/World_Actors.cpp
    src/game/world/World_Choice.cpp
    src/game/world/World_Combat.cpp
    src/game/world/World_Interaction.cpp
    src/game/world/World_Remote.cpp
    src/game/world/Data.cpp
    src/game/systems/ItemFactory.cpp
    src/game/systems/InventorySystem.cpp
    src/game/systems/ShopSystem.cpp
    src/game/systems/QuestRuntimeSystem.cpp
    src/game/quests/QuestSystem.cpp
    src/game/ui/UIController.cpp
    src/game/render/UIRenderer.cpp
    src/ui/windows/DialogueUI.cpp
)


target_include_directories(TopDownMMOClient PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/shared
    ${CMAKE_SOURCE_DIR}/third_party
    ${CMAKE_SOURCE_DIR}/third_party/asio/include
)

target_link_libraries(TopDownMMOClient PRIVATE raylib ws2_32)

if (WIN32)
    target_link_libraries(TopDownMMOClient PRIVATE winmm)
endif()

add_custom_command(TARGET TopDownMMOClient POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:TopDownMMOClient>/assets
)
