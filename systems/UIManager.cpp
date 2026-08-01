#include "UIManager.h"
#include <sstream>

UIManager::UIManager(int uiH, int sideW)
    : health(100), maxHealth(100), score(0), timeRemaining(0),
      uiHeight(uiH), sideWidth(sideW) {}

void UIManager::setStats(int h, int maxH, int s, int t = 360) {
    health = h;
    maxHealth = maxH;
    score = s;
    timeRemaining = t;
}

void UIManager::setMessages(const std::vector<std::string>& msgs) {
    messages = msgs;
}

void UIManager::setMessageColors(const std::vector<std::string>& colors) {
    messageColors = colors;
}

void UIManager::setSideArt(const std::vector<std::string>& art) {
    sideArt = art;
    sideArtColors.clear();
}

void UIManager::setColoredSideArt(const std::vector<ColoredArtLine>& art) {
    sideArt.clear();
    sideArtColors.clear();

    for (const ColoredArtLine& line : art) {
        std::string flattened;
        std::vector<std::string> colors;

        for (const auto& segment : line) {
            flattened += segment.first;
            for (std::size_t i = 0; i < segment.first.size(); ++i) {
                colors.push_back(segment.second);
            }
        }

        sideArt.push_back(flattened);
        sideArtColors.push_back(colors);
    }
}

int UIManager::getUIHeight() const {
    return uiHeight;
}

int UIManager::getSideWidth() const {
    return sideWidth;
}

std::string UIManager::getMessageColor(int index) const {
    if (index < 0 || index >= static_cast<int>(messageColors.size())) {
        return "";
    }

    return messageColors[index];
}

std::string UIManager::getSideArtColor(int row, int col) const {
    if (row < 0 || row >= static_cast<int>(sideArtColors.size())) {
        return "";
    }
    if (col < 0 || col >= static_cast<int>(sideArtColors[row].size())) {
        return "";
    }

    return sideArtColors[row][col];
}

std::string UIManager::buildHealthBar(int width) const {
    const std::string label = "INTEGRITY ";
    int barWidth = width - static_cast<int>(label.size()) - 12;
    if (barWidth < 5) barWidth = 5;

    float ratio = (float)health / maxHealth;
    int filled = ratio * barWidth;

    std::string bar = "[";
    for (int i = 0; i < barWidth; i++) {
        bar += (i < filled) ? '=' : '-';
    }
    bar += "]";

    return label + bar + " " + std::to_string(health) + "/" + std::to_string(maxHealth);
}

void UIManager::renderToBuffer(std::vector<std::string>& buffer) const {
    int height = buffer.size();
    if (height == 0) return;

    int width = buffer[0].size();

    int mapHeight = height - uiHeight;
    int mapWidth = width - sideWidth;

    // Draw Vertical Divider
    for (int y = 0; y < mapHeight; y++) {
        buffer[y][mapWidth] = '|';
    }

    // Draw Side Panel
    for (int y = 0; y < (int)sideArt.size() && y < mapHeight; y++) {
        for (int x = 0; x < (int)sideArt[y].size() && x < sideWidth - 1; x++) {
            buffer[y][mapWidth + 1 + x] = sideArt[y][x];
        }
    }

    // Draw Horizontal Divider 
    for (int x = 0; x < width; x++) {
        buffer[mapHeight][x] = '-';
    }

    // UI Area 
    int yStart = mapHeight + 1;

    // Row 1: Health bar + Score + Time
    std::string hpBar = buildHealthBar(mapWidth);

    std::stringstream ss;
    ss << hpBar << "  SCORE: " << score << "  TIME: " << timeRemaining;
    std::string statsLine = ss.str();

    for (int i = 0; i < (int)statsLine.size() && i < width; i++) {
        buffer[yStart][i] = statsLine[i];
    }

    // Multi-line messages
    for (int i = 0; i < (int)messages.size() && i + 1 < uiHeight; i++) {
        const std::string& msg = messages[i];
        for (int j = 0; j < (int)msg.size() && j < width; j++) {
            buffer[yStart + 1 + i][j] = msg[j];
        }
    }
}
