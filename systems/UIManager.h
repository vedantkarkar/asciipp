#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <string>
#include <utility>
#include <vector>

using ColoredTextSegment = std::pair<std::string, std::string>;
using ColoredArtLine = std::vector<ColoredTextSegment>;

class UIManager {
private:
    int health;
    int maxHealth;
    int score;
    int timeRemaining;

    int uiHeight;
    int sideWidth;

    std::vector<std::string> messages;
    std::vector<std::string> messageColors;
    std::vector<std::string> sideArt;
    std::vector<std::vector<std::string>> sideArtColors;

public:
    UIManager(int uiH = 4, int sideW = 30);

    // Stats
    void setStats(int h, int maxH, int s, int t);

    // Messages (multi-line)
    void setMessages(const std::vector<std::string>& msgs);
    void setMessageColors(const std::vector<std::string>& colors);

    // Side panel
    void setSideArt(const std::vector<std::string>& art);
    void setColoredSideArt(const std::vector<ColoredArtLine>& art);

    // Layout getters
    int getUIHeight() const;
    int getSideWidth() const;
    std::string getMessageColor(int index) const;
    std::string getSideArtColor(int row, int col) const;

    // Render
    void renderToBuffer(std::vector<std::string>& buffer) const;

private:
    std::string buildHealthBar(int width) const;
};

#endif
