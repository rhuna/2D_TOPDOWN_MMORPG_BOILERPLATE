cmake_minimum_required(VERSION 3.16)
project(TopDownMMOStarterClean LANGUAGES CXX)

# Build the playable client by default.
add_subdirectory(client)

# Optional server target. This is OFF by default so the client stays easy to build.
option(BUILD_MMO_SERVER "Build the prototype multiplayer server" OFF)

if (BUILD_MMO_SERVER)
    add_subdirectory(server)
endif()
