#ifndef MAP_H
#define MAP_H

#include <vector>
#include <string>
#include <functional>
#include <unordered_map>

class Map {
private:
    std::vector<std::string> grid;

    // Trigger system
    std::unordered_map<char, std::function<void()>> triggers;

public:
    Map();

    // Load map
    void load(const std::vector<std::string>& mapData);

    // Access
    const std::vector<std::string>& getGrid() const;

    // Collision
    bool isBlocked(int x, int y) const;

    // Tile access
    char getTile(int x, int y) const;
    void setTile(int x, int y, char value);

    // Trigger system
    void addTrigger(char symbol, std::function<void()> callback);
    void checkTrigger(int x, int y);
};

#endif