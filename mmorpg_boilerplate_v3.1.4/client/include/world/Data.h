#pragma once

#include <string>
#include <vector>

// Build the giant overworld used by the client.
//
// The returned map is a grid of characters.
// Each character has meaning:
//
// '.' = walkable grass
// '#' = solid wall / blocked tile / dense forest / lake edge
// 'p' = road / path tile
// 'P' = player spawn
// 'E' = elder NPC
// 'N' = generic NPC
// 'S' = slime spawn
std::vector<std::string> BuildMap();
