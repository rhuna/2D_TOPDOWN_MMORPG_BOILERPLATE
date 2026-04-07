cmake_minimum_required(VERSION 3.16)
project(TopDownMMOServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# This tiny prototype server uses standalone Asio headers.
# Put asio.hpp in: third_party/asio/include/asio.hpp
if (NOT EXISTS "${CMAKE_SOURCE_DIR}/third_party/asio/include/asio.hpp")
    message(FATAL_ERROR
        "Standalone Asio headers not found.\n"
        "Place asio.hpp at: ${CMAKE_SOURCE_DIR}/third_party/asio/include/asio.hpp")
endif()



add_executable(TopDownMMOServer
    src/main.cpp
)


target_include_directories(TopDownMMOServer PRIVATE
    ${CMAKE_SOURCE_DIR}/shared
    ${CMAKE_SOURCE_DIR}/third_party/asio/include
)

target_compile_definitions(TopDownMMOServer PRIVATE
    _WIN32_WINNT=0x0601
)

target_link_libraries(TopDownMMOServer PRIVATE 
    ws2_32
    mswsock
    advapi32
)
