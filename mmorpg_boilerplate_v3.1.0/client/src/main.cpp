// Entry point for the client executable.
//
// This file stays intentionally small.
// The goal is to keep startup logic simple and let the Game class own
// the actual runtime flow.
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER // 🔥 prevents DrawText / LoadImage conflicts too
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif

#include "core/Game.h"

int main() {
    Game game;
    game.Run();
    return 0;
}
