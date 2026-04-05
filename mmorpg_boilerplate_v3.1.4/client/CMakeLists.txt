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
    src/main.cpp
    src/core/Game.cpp
    src/net/NetworkClient.cpp
    src/world/World.cpp
    src/world/World_Render.cpp
    src/world/World_Choice.cpp
    src/world/World_Actors.cpp
    src/world/World_Combat.cpp
    src/world/World_Interaction.cpp
    src/world/WorldUI.cpp
    src/world/World_Gameplay.cpp
    src/world/Data.cpp
    src/game/QuestSystem.cpp
    src/ui/DialogueUI.cpp
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
