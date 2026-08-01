#include "Renderer.h"

#include <iostream>

// Forward includes
#include "../utils/Color.h"
#include "../world/Map.h"
#include "../entity/Entity.h"
#include "../systems/UIManager.h"

// UNIVERSAL DRAW PIPELINE
// Map → Entities → UI → Buffer → Screen
//i.e only buffer touches the screen, and everything else draws to the buffer. This allows us to easily manage rendering order and layering (e.g. entities on top of map, UI on top of everything).

Renderer::Renderer(int w, int h)
    : width(w),
      height(h),
      mapRenderWidth(0),
      mapRenderHeight(0),
      activeUI(nullptr) {
    buffer.resize(height, std::string(width, ' '));
}

// The clear() function resets the buffer to an empty state (filled with spaces). This is called at the beginning of each frame to prepare for new drawing operations.

void Renderer::clear() {
    mapRenderWidth = 0;
    mapRenderHeight = 0;
    activeUI = nullptr;

    for (int i = 0; i < height; i++) {
        buffer[i].assign(width, ' ');
    }
}

// The drawAt() function draws a character at a specific position in the buffer.

void Renderer::drawAt(int x, int y, char c) {
    if (x < 0 || x >= width || y < 0 || y >= height) return;
    buffer[y][x] = c;
}

void Renderer::drawMap(const Map& map) {
    const std::vector<std::string>& grid = map.getGrid();
    mapRenderHeight = std::min(static_cast<int>(grid.size()), height);
    mapRenderWidth = 0;

    //int uiHeight = 0;
    //int sideWidth = 0;

    // Get layout from UI (if exists)
    // NOTE: safe fallback handled in GameEngine later

    for (int y = 0; y < (int)grid.size() && y < height; y++) {
        mapRenderWidth = std::max(mapRenderWidth, std::min(static_cast<int>(grid[y].size()), width));
        for (int x = 0; x < (int)grid[y].size() && x < width; x++) {
            drawAt(x, y, grid[y][x]);
        }
    }
}

// The drawEntity() function takes an Entity object and draws it to the buffer based on its position and symbol. This allows entities to be rendered on top of the map.
// Just put entity object in parameter, hence every entity must be an object that is passable by reference. 

void Renderer::drawEntity(const Entity& entity) {
    if (!entity.alive()) {
        return;
    }
    drawAt(entity.getX(), entity.getY(), entity.getRenderSymbol());
}

//UI controls its own layout
//Renderer stays generic and just calls a renderToBuffer() function on the UIManager, which fills the buffer with the appropriate characters for the UI elements (e.g. health bars, inventory, etc.). This keeps the Renderer decoupled from the specifics of the UI design and allows for more flexible UI implementations in the future.

void Renderer::drawUI(const UIManager& ui) {
    activeUI = &ui;
    ui.renderToBuffer(buffer);
}


// The present() function then takes care of flushing the buffer to the terminal in one go, minimizing flickering and maximizing performance.

void Renderer::present() {
    // Move cursor to top-left
    std::cout << "\033[H";

    for (int y = 0; y < height; y++) {
        const auto& line = buffer[y];
        std::string currentColor = Color::RESET;
        const int uiHeight = activeUI ? activeUI->getUIHeight() : 0;
        const int mapHeight = height - uiHeight;
        const int mapWidth = activeUI ? (width - activeUI->getSideWidth()) : width;

        for (int x = 0; x < static_cast<int>(line.size()); x++) {
            char c = line[x];
            std::string nextColor = Color::RESET;
            bool inMapArea = (x < mapRenderWidth && y < mapRenderHeight);

            if (inMapArea && (c == 'P' || c == 'p')) {
                nextColor = Color::GREEN;
            } else if (inMapArea && (c == 'E' || c == 'e')) {
                nextColor = Color::RED;
            } else if (inMapArea && c == 'D') {
                nextColor = Color::YELLOW;
            } else if (inMapArea && c == '@') {
                nextColor = Color::BRIGHT_MAGENTA;
            } else if (activeUI) {
                if (y < mapHeight && x > mapWidth) {
                    const int sideArtCol = x - (mapWidth + 1);
                    const std::string color = activeUI->getSideArtColor(y, sideArtCol);
                    if (!color.empty()) {
                        nextColor = color;
                    }
                } else {
                    const int messageIndex = y - (mapHeight + 2);
                    const std::string color = activeUI->getMessageColor(messageIndex);
                    if (!color.empty() && c != ' ') {
                        nextColor = color;
                    }
                }
            }

            if (nextColor != currentColor) {
                std::cout << nextColor;
                currentColor = nextColor;
            }

            std::cout << c;
        }

        if (currentColor != Color::RESET) {
            std::cout << Color::RESET;
        }

        std::cout << '\n';
    }

    std::cout.flush();
}
