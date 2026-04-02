// Entry point for the client executable.
//
// This file stays intentionally small.
// The goal is to keep startup logic simple and let the Game class own
// the actual runtime flow.

#include "core/Game.h"

int main() {
    Game game;
    game.Run();
    return 0;
}
