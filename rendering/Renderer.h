#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <string>

class Map;
class Entity;
class UIManager;

class Renderer {
private:
    int width;
    int height;
    int mapRenderWidth;
    int mapRenderHeight;
    const UIManager* activeUI;

    std::vector<std::string> buffer;

public:
    Renderer(int w, int h);

    void clear();

    void drawMap(const Map& map);
    void drawEntity(const Entity& entity);
    void drawUI(const UIManager& ui);

    void drawAt(int x, int y, char c);

    void present();
};

#endif
