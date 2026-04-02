cmake_minimum_required(VERSION 3.16)
project(TopDownMMOClient LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# Pull raylib during configure time so the user does not need to install it manually.
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)

FetchContent_MakeAvailable(raylib)

add_executable(TopDownMMOClient
    src/main.cpp
    src/core/Game.cpp
    src/world/World.cpp
    src/world/Data.cpp
    src/net/NetworkClient.cpp
)

target_include_directories(TopDownMMOClient PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/shared
    ${CMAKE_SOURCE_DIR}/third_party/asio/include
)

target_link_libraries(TopDownMMOClient PRIVATE raylib ws2_32)

if (WIN32)
    target_link_libraries(TopDownMMOClient PRIVATE winmm)
endif()

# Copy assets next to the built executable so the tileset loads correctly when run from /build.
add_custom_command(TARGET TopDownMMOClient POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:TopDownMMOClient>/assets
)
