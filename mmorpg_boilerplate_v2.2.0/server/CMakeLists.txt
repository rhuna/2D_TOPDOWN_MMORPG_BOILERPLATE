cmake_minimum_required(VERSION 3.10)
project(MMOServer)

# Build the server executable.
add_executable(MMOServer
    src/main.cpp
)

# Tell the compiler where standalone Asio headers live.
# You need:
# third_party/asio/include/asio.hpp
target_include_directories(MMOServer PRIVATE
    ${CMAKE_SOURCE_DIR}/../third_party/asio/include
)

# Windows socket library.
target_link_libraries(MMOServer ws2_32)