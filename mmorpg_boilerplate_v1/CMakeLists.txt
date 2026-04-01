cmake_minimum_required(VERSION 3.16)
project(TopDownMMOStarter LANGUAGES CXX)

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

add_executable(TopDownMMOStarter
    src/main.cpp
    src/Game.cpp
    src/World.cpp
    src/Data.cpp
)

target_include_directories(TopDownMMOStarter PRIVATE include)
target_link_libraries(TopDownMMOStarter PRIVATE raylib)

if (WIN32)
    target_link_libraries(TopDownMMOStarter PRIVATE winmm)
endif()
