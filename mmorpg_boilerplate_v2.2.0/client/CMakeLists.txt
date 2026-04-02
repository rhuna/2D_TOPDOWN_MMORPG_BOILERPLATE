cmake_minimum_required(VERSION 3.10)
project(MMOClient)

add_executable(MMOClient
    src/main.cpp
    src/Game.cpp
    src/NetworkClient.cpp
)

target_include_directories(MMOClient PRIVATE
    ${CMAKE_SOURCE_DIR}/../third_party/asio/include
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(MMOClient
    ws2_32
)