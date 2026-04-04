#include "world/Data.h"

#include <algorithm>
#include <cmath>
#include <vector>

// Build the 320x320 overworld.
//
// Design goals:
// - very large traversal space
// - visible road network
// - a few named regions
// - blocked zones to keep the map from feeling empty
// - starter village with player spawn and elder
std::vector<std::string> BuildMap() {
    const int width = 320;
    const int height = 320;

    // Start with fully walkable grass.
    std::vector<std::string> map(height, std::string(width, '.'));



    //////////////////////////////////////////////////////////////////////  
    // Helper lambdas for map construction.
    //////////////////////////////////////////////////////////////////////

    // Check if (x, y) is within the map bounds.
    auto inBounds = [&](int x, int y) {
        // Note: the map is indexed as map[y][x] since it's a vector of strings.
        // This is a common source of confusion, so be careful when working with (x, y) coordinates vs. map indexing.
        // The map is accessed as map[row][column], which corresponds to map[y][x].
        return x >= 0 && x < width && y >= 0 && y < height;
    };

    // Set a single tile at (x, y) to the given character if it's in bounds.
    // This is a safe way to modify the map without risking out-of-bounds errors.
    auto setTile = [&](int x, int y, char tile) {
        // Only modify the map if (x, y) is within bounds.
        // This prevents out-of-bounds access and keeps the map construction robust.
        if (inBounds(x, y)) {
            // Remember that map is indexed as map[y][x] since it's a vector of strings.
            // This means that x corresponds to the column index and y corresponds to the row index.
            map[y][x] = tile;
        }
    };

    //////////////////////////////////////////////////////////////////////
    // Map construction logic using the helper lambdas.
    //////////////////////////////////////////////////////////////////////

    // Fill a rectangle area with a tile character.
    auto fillRect = [&](int x0, int y0, int w, int h, char tile) {
        // Loop through the rectangle defined by (x0, y0) as the top-left corner and (w, h) as the width and height.
        for (int y = y0; y < y0 + h; ++y) {
            // For each row in the rectangle, set the tiles from x0 to x0 + w to the specified tile character.
            for (int x = x0; x < x0 + w; ++x) {
                // Use the setTile helper to safely modify the map.
                setTile(x, y, tile);
            }
        }
    };
    // Draw a rectangle frame with a tile character.
    auto frameRect = [&](int x0, int y0, int w, int h, char tile) {
        // Draw the top and bottom edges of the rectangle.
        for (int x = x0; x < x0 + w; ++x) {
            setTile(x, y0, tile);
            setTile(x, y0 + h - 1, tile);
        }
        // Draw the left and right edges of the rectangle.
        for (int y = y0; y < y0 + h; ++y) {
            setTile(x0, y, tile);
            setTile(x0 + w - 1, y, tile);
        }
    };

    // Carve a horizontal road.
    // The thickness parameter allows us to create wider roads by carving multiple adjacent rows.
    auto carveH = [&](int x0, int x1, int y, int thickness = 1) {
        // Ensure x0 is the leftmost coordinate and x1 is the rightmost coordinate for the loop.
        if (x0 > x1) std::swap(x0, x1);
        // Loop through the specified thickness of rows to carve a wider road.
        for (int dy = 0; dy < thickness; ++dy) {
            // For each row in the thickness, set the tiles from x0 to x1 to 'p' to represent a path.
            for (int x = x0; x <= x1; ++x) {
                setTile(x, y + dy, 'p');
            }
        }
    };

    // Carve a vertical road.
    auto carveV = [&](int x, int y0, int y1, int thickness = 1) {
        // Ensure y0 is the topmost coordinate and y1 is the bottommost coordinate for the loop.
        if (y0 > y1) std::swap(y0, y1);
        // Loop through the specified thickness of columns to carve a wider road.
        for (int dx = 0; dx < thickness; ++dx) {
            // For each column in the thickness, set the tiles from y0 to y1 to 'p' to represent a path.
            for (int y = y0; y <= y1; ++y) {
                setTile(x + dx, y, 'p');
            }
        }
    };

    // Add a simple ellipse-shaped blocked area.
    // In this prototype, blocked areas are reused for lakes and heavy brush.
    auto addLake = [&](int cx, int cy, int radiusX, int radiusY) {
        // Loop through a bounding box around the center of the ellipse.
        for (int y = cy - radiusY; y <= cy + radiusY; ++y) {
            // For each row in the bounding box, loop through the columns to check if they fall within the ellipse.
            for (int x = cx - radiusX; x <= cx + radiusX; ++x) {
                // Check if the current (x, y) coordinate is within the bounds of the map before accessing it.
                if (!inBounds(x, y)) {
                    continue;
                }
                // Use the standard ellipse formula to determine if the (x, y) coordinate is inside the ellipse defined by (cx, cy) as the center and (radiusX, radiusY) as the radii.
                const float nx = static_cast<float>(x - cx) / static_cast<float>(radiusX);
                const float ny = static_cast<float>(y - cy) / static_cast<float>(radiusY);

                // If the point is within the ellipse, set the tile to '#' to represent a blocked area.
                if (nx * nx + ny * ny <= 1.0f) {
                    setTile(x, y, '#');
                }
            }
        }
    };

    // Scatter blocked forest tiles with a repeating pattern.
    auto addForestBlock = [&](int x0, int y0, int w, int h, int stride) {
        // Loop through the specified rectangle area and set tiles to '#' based on a pseudo-random pattern that depends on the coordinates.
        for (int y = y0; y < y0 + h; ++y) {
            // For each row in the rectangle, loop through the columns and use a deterministic pattern to decide where to place blocked tiles.
            for (int x = x0; x < x0 + w; ++x) {
                // Check if the current (x, y) coordinate is within the bounds of the map before accessing it.
                if (!inBounds(x, y)) {
                    continue;
                }
                // Use a stable pseudo-random pattern based on the coordinates to create a natural-looking distribution of blocked tiles without needing to store additional state.
                if (((x + y) % stride) == 0 || ((x * 3 + y * 5) % (stride + 2)) == 1) {
                    setTile(x, y, '#');
                }
            }
        }
    };

    // Solid border around the map.
    for (int x = 0; x < width; ++x) {
        map[0][x] = '#';
        map[height - 1][x] = '#';
    }
    for (int y = 0; y < height; ++y) {
        map[y][0] = '#';
        map[y][width - 1] = '#';
    }

    // Main roads.
    carveH(6, width - 8, 24, 2);
    carveH(10, width - 12, 72, 2);
    carveH(8, width - 16, 128, 2);
    carveH(12, width - 18, 184, 2);
    carveH(14, width - 14, 240, 2);
    carveH(18, width - 10, 292, 2);

    carveV(24, 6, height - 8, 2);
    carveV(80, 8, height - 10, 2);
    carveV(136, 8, height - 10, 2);
    carveV(192, 8, height - 10, 2);
    carveV(248, 8, height - 10, 2);
    carveV(296, 10, height - 14, 2);

    // Secondary connectors between major roads.
    for (int i = 0; i < 6; ++i) {
        carveH(24 + i * 24, 80 + i * 24, 48 + i * 40, 1);
        carveH(80 + i * 20, 136 + i * 18, 96 + i * 34, 1);
        carveH(136 + i * 18, 192 + i * 18, 144 + i * 26, 1);
        carveH(192 + i * 16, 248 + i * 14, 208 + i * 18, 1);
    }

    // Starter village walls/openings.
    fillRect(14, 14, 24, 18, '.');
    frameRect(12, 12, 30, 22, '#');
    carveH(18, 36, 24, 2);
    carveV(24, 18, 28, 2);

    // Larger blocked landmarks.
    addLake(58, 154, 16, 12);
    addLake(154, 44, 20, 10);
    addLake(212, 164, 18, 14);
    addLake(276, 92, 14, 10);
    addLake(268, 258, 18, 12);

    // Forest clusters.
    addForestBlock(42, 38, 24, 28, 4);
    addForestBlock(98, 98, 32, 30, 5);
    addForestBlock(154, 212, 36, 32, 4);
    addForestBlock(210, 42, 30, 30, 5);
    addForestBlock(250, 190, 32, 28, 4);
    addForestBlock(72, 236, 36, 26, 5);

    // More blocked regions for visual variety.
    fillRect(104, 16, 20, 18, '#');
    fillRect(164, 100, 28, 16, '#');
    fillRect(220, 222, 22, 18, '#');
    fillRect(40, 206, 18, 24, '#');
    frameRect(150, 148, 26, 22, '#');
    frameRect(232, 132, 24, 22, '#');

    // Starter village actors.
    for (int y = 18; y <= 28; ++y) {
        for (int x = 18; x <= 30; ++x) {
            setTile(x, y, '.');
        }
    }
    setTile(22, 22, 'P');
    setTile(28, 22, 'E');

    // ------------------------------------------------------------
    // Starter village shop building in the overworld
    // ------------------------------------------------------------
    // Building footprint near the starter village.
    // 'B' marks the door tile.
    fillRect(32, 18, 8, 6, '#'); // outer building
    fillRect(33, 19, 6, 4, '.'); // interior floor area of the building shell
    setTile(35, 23, 'B');        // front door tile

    // ------------------------------------------------------------
    // Hidden interior room for the shop
    // ------------------------------------------------------------
    // This is a fake "indoor map" placed elsewhere on the big map.
    // The player is teleported here when entering the building.
    fillRect(300, 20, 12, 10, '#'); // walls
    fillRect(301, 21, 10, 8, '.');  // floor
    setTile(306, 28, 'D');          // exit door tile
    setTile(306, 24, 'M');          // merchant NPC

    const std::vector<std::pair<int, int>> npcPositions = {
        {84, 72}, {138, 128}, {196, 184}, {248, 240}, {292, 292}, {240, 72}, {80, 240}
    };
    for (const auto& [x, y] : npcPositions) {
        setTile(x, y, 'N');
    }

    // Scatter slimes around the world.
    for (int y = 16; y < height - 16; y += 18) {
        for (int x = 20; x < width - 20; x += 20) {
            if ((x + y) % 40 == 0 || (x * 7 + y * 11) % 37 == 0) {
                if (inBounds(x, y) && map[y][x] == '.') {
                    setTile(x, y, 'S');
                }
            }
        }
    }

    // Recarve major roads so later blockers never permanently overwrite them.
    carveH(6, width - 8, 24, 2);
    carveH(10, width - 12, 72, 2);
    carveH(8, width - 16, 128, 2);
    carveH(12, width - 18, 184, 2);
    carveH(14, width - 14, 240, 2);
    carveH(18, width - 10, 292, 2);
    carveV(24, 6, height - 8, 2);
    carveV(80, 8, height - 10, 2);
    carveV(136, 8, height - 10, 2);
    carveV(192, 8, height - 10, 2);
    carveV(248, 8, height - 10, 2);
    carveV(296, 10, height - 14, 2);

    // Restore special actors after road recarving.
    setTile(22, 22, 'P');
    setTile(28, 22, 'E');
    for (const auto& [x, y] : npcPositions) {
        setTile(x, y, 'N');
    }
    // Restore shop building + interior markers
    // Note: the shop building is placed after the road carving 
    //to ensure that the building and its door are not 
    //accidentally overwritten by the road carving logic.
    fillRect(32, 18, 8, 6, '#');
    // The interior floor area of the building shell is filled 
    //with '.' to represent walkable space inside the building. 
    //This is done after the road carving to ensure that the 
    //interior of the building is not overwritten by any roads 
    //that might intersect with it.
    fillRect(33, 19, 6, 4, '.');
    // The front door tile is set to 'B' to mark the entrance 
    //to the building. This is done after the road carving to 
    //ensure that the door tile is not overwritten by any 
    //roads that might intersect with it.
    setTile(35, 23, 'B');

    // The hidden interior room for the shop is created after the
    // main map features to ensure that it is not overwritten by any
    // of the earlier map construction steps. This interior is a
    // separate area that the player is teleported to when entering the shop, so it needs
    // to be defined on the same map but in a different location.
    fillRect(300, 20, 12, 10, '#');
    // The interior floor area of the shop is filled with 
    //'.' to represent walkable space inside the shop. 
    //This is done after the walls are created to ensure 
    //that the floor tiles are not overwritten by the wall tiles.
    fillRect(301, 21, 10, 8, '.');
    // The exit door tile is set to 'D' to mark the location where
    //the player will be teleported back to the overworld when exiting the shop.
    //This is done after the floor is created to ensure that the
    //door tile is not overwritten by the floor creation logic.
    setTile(306, 28, 'D');
    // The merchant NPC is placed at 'M' inside the shop interior.
    // This is done after the floor and walls are created to ensure
    // that the NPC tile is not overwritten by those steps. 
    //The NPC is placed in a location that is easily visible to 
    //the player when they enter the shop.
    setTile(306, 24, 'M');

    return map;
}
