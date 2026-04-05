cmake_minimum_required(VERSION 3.16)
project(TopDownMMOStarterClean LANGUAGES CXX)

# Build the playable client by default.
add_subdirectory(client)

target_compile_definitions(TopDownMMOClient PRIVATE
    WIN32_LEAN_AND_MEAN
    NOGDI
    NOUSER
    _WIN32_WINNT=0x0601
)
# Optional server target. This is OFF by default so the client stays easy to build.
option(BUILD_MMO_SERVER "Build the prototype multiplayer server" ON)

if (BUILD_MMO_SERVER)
    add_subdirectory(server)
endif()
