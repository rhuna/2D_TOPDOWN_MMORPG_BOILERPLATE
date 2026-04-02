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

    auto inBounds = [&](int x, int y) {
        return x >= 0 && x < width && y >= 0 && y < height;
    };

    auto setTile = [&](int x, int y, char tile) {
        if (inBounds(x, y)) {
            map[y][x] = tile;
        }
    };

    auto fillRect = [&](int x0, int y0, int w, int h, char tile) {
        for (int y = y0; y < y0 + h; ++y) {
            for (int x = x0; x < x0 + w; ++x) {
                setTile(x, y, tile);
            }
        }
    };

    auto frameRect = [&](int x0, int y0, int w, int h, char tile) {
        for (int x = x0; x < x0 + w; ++x) {
            setTile(x, y0, tile);
            setTile(x, y0 + h - 1, tile);
        }
        for (int y = y0; y < y0 + h; ++y) {
            setTile(x0, y, tile);
            setTile(x0 + w - 1, y, tile);
        }
    };

    // Carve a horizontal road.
    auto carveH = [&](int x0, int x1, int y, int thickness = 1) {
        if (x0 > x1) std::swap(x0, x1);
        for (int dy = 0; dy < thickness; ++dy) {
            for (int x = x0; x <= x1; ++x) {
                setTile(x, y + dy, 'p');
            }
        }
    };

    // Carve a vertical road.
    auto carveV = [&](int x, int y0, int y1, int thickness = 1) {
        if (y0 > y1) std::swap(y0, y1);
        for (int dx = 0; dx < thickness; ++dx) {
            for (int y = y0; y <= y1; ++y) {
                setTile(x + dx, y, 'p');
            }
        }
    };

    // Add a simple ellipse-shaped blocked area.
    // In this prototype, blocked areas are reused for lakes and heavy brush.
    auto addLake = [&](int cx, int cy, int radiusX, int radiusY) {
        for (int y = cy - radiusY; y <= cy + radiusY; ++y) {
            for (int x = cx - radiusX; x <= cx + radiusX; ++x) {
                if (!inBounds(x, y)) {
                    continue;
                }

                const float nx = static_cast<float>(x - cx) / static_cast<float>(radiusX);
                const float ny = static_cast<float>(y - cy) / static_cast<float>(radiusY);

                if (nx * nx + ny * ny <= 1.0f) {
                    setTile(x, y, '#');
                }
            }
        }
    };

    // Scatter blocked forest tiles with a repeating pattern.
    auto addForestBlock = [&](int x0, int y0, int w, int h, int stride) {
        for (int y = y0; y < y0 + h; ++y) {
            for (int x = x0; x < x0 + w; ++x) {
                if (!inBounds(x, y)) {
                    continue;
                }

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

    return map;
}
