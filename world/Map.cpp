#include "Map.h"

Map::Map() {}

void Map::load(const std::vector<std::string>& mapData) {
    grid = mapData;
}

const std::vector<std::string>& Map::getGrid() const {
    return grid;
}

char Map::getTile(int x, int y) const {
    if (y < 0 || y >= (int)grid.size()) return '#';
    if (x < 0 || x >= (int)grid[y].size()) return '#';

    return grid[y][x];
}

void Map::setTile(int x, int y, char value) {
    if (y < 0 || y >= (int)grid.size()) return;
    if (x < 0 || x >= (int)grid[y].size()) return;

    grid[y][x] = value;
}

bool Map::isBlocked(int x, int y) const {
    char tile = getTile(x, y);

    // Define blocked tiles
    return (tile == '#');
}

void Map::addTrigger(char symbol, std::function<void()> callback) {
    triggers[symbol] = callback;
}

void Map::checkTrigger(int x, int y) {
    char tile = getTile(x, y);

    if (triggers.find(tile) != triggers.end()) {
        setTile(x, y, '.');
        triggers[tile]();
    }
}