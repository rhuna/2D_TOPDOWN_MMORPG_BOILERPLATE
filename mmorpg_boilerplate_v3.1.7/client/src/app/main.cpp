// Entry point for the client executable.
//
// This file stays intentionally small.
// The goal is to keep startup logic simple and let the Game class own
// the actual runtime flow.

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

#include "app/Game.h"

int main() {
    Game game;
    game.Run();
    return 0;
}
