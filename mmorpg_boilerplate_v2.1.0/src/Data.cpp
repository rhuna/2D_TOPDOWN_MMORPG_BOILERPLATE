#include "Data.h"
#include <algorithm>

std::vector<std::string> BuildMap() {
    const int width = 140;
    const int height = 140;
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

    auto carveH = [&](int x0, int x1, int y) {
        if (x0 > x1) std::swap(x0, x1);
        for (int x = x0; x <= x1; ++x) setTile(x, y, 'p');
    };

    auto carveV = [&](int x, int y0, int y1) {
        if (y0 > y1) std::swap(y0, y1);
        for (int y = y0; y <= y1; ++y) setTile(x, y, 'p');
    };

    for (int x = 0; x < width; ++x) {
        map[0][x] = '#';
        map[height - 1][x] = '#';
    }
    for (int y = 0; y < height; ++y) {
        map[y][0] = '#';
        map[y][width - 1] = '#';
    }

    fillRect(14, 12, 18, 12, '#');
    fillRect(70, 18, 20, 16, '#');
    fillRect(102, 68, 22, 18, '#');
    fillRect(24, 90, 26, 20, '#');
    fillRect(78, 102, 28, 20, '#');
    fillRect(112, 24, 14, 14, '#');
    fillRect(48, 48, 12, 30, '#');
    fillRect(90, 46, 14, 10, '#');
    fillRect(16, 56, 10, 18, '#');
    fillRect(58, 114, 12, 12, '#');

    frameRect(8, 8, 28, 20, '#');
    frameRect(62, 12, 34, 26, '#');
    frameRect(98, 62, 30, 26, '#');
    frameRect(18, 84, 38, 30, '#');
    frameRect(74, 96, 36, 28, '#');

    carveH(5, 134, 18);
    carveH(10, 126, 50);
    carveH(16, 130, 82);
    carveH(8, 118, 118);

    carveV(18, 5, 130);
    carveV(44, 14, 122);
    carveV(76, 8, 132);
    carveV(108, 18, 130);

    carveH(18, 44, 32);
    carveH(44, 76, 66);
    carveH(76, 108, 102);
    carveH(18, 76, 96);
    carveH(44, 108, 28);

    for (int x = 20; x <= 26; ++x) setTile(x, 18, '.');
    for (int y = 16; y <= 22; ++y) setTile(18, y, '.');

    for (int y = 16; y <= 20; ++y) {
        for (int x = 20; x <= 24; ++x) {
            setTile(x, y, '.');
        }
    }
    setTile(22, 18, 'P');
    setTile(26, 18, 'E');

    const std::vector<std::pair<int, int>> slimes = {
        {34, 16}, {52, 24}, {66, 28}, {88, 22}, {118, 20},
        {30, 40}, {60, 54}, {84, 52}, {112, 48}, {126, 58},
        {22, 76}, {40, 88}, {68, 84}, {92, 78}, {118, 84},
        {26, 110}, {58, 118}, {86, 108}, {112, 118}, {128, 108}
    };
    for (const auto& [x, y] : slimes) setTile(x, y, 'S');

    const std::vector<std::pair<int, int>> elders = {{26, 18}, {82, 50}, {110, 82}};
    for (std::size_t i = 0; i < elders.size(); ++i) {
        setTile(elders[i].first, elders[i].second, i == 0 ? 'E' : 'N');
    }

    return map;
}
