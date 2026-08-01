#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <vector>
#include <functional>
#include "../utils/TerminalUtils.h"
//here only included the TerminalUtils header because GameEngine directly uses it as a member variable (called by value). Every other class is called by reference or pointer, so we can forward declare them instead to avoid circular dependencies and reduce compile times.

// Forward declarations (to avoid circular dependencies)
class Renderer;
class InputHandler;
class Map;
class Entity;
class UIManager;
class EventSystem;

class GameEngine {
private:
    bool isRunning;

    // Core Systems
    TerminalUtils terminal;
    Renderer* renderer;
    InputHandler* input;
    Map* currentMap;
    UIManager* ui;
    EventSystem* eventSystem;

    std::vector<Entity*> entities;

    // Timing
    float deltaTime;

    // Required terminal size
    int requiredWidth;
    int requiredHeight;

    // Alt screen management
    void enterAltScreen();
    void exitAltScreen();

    enum class EngineMode {
        GAMEPLAY,
        SCRIPT
    };

    EngineMode mode;
    std::function<void()> activeScript;

public:
    GameEngine();
    ~GameEngine();

    void init(int reqW, int reqH);
    void run();
    void update(float dt);
    void render();
    void shutdown();

    void addEntity(Entity* entity);

    // Setters for dependency injection (important for modularity)
    void setRenderer(Renderer* r);
    void setInputHandler(InputHandler* i);
    void setMap(Map* m);
    void setUI(UIManager* u);
    void setEventSystem(EventSystem* e);

    void runScript(std::function<void()> script);
};

#endif