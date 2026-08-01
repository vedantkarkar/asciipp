#include "GameEngine.h"

#include <chrono>
#include <thread>
#include <iostream>

// Forward includes (to avoid circular dependencies)
#include "../rendering/Renderer.h"
#include "../input/InputHandler.h"
#include "../world/Map.h"
#include "../entity/Entity.h"
#include "../systems/UIManager.h"
#include "../systems/EventSystem.h"

GameEngine::GameEngine()
    : isRunning(false),
      renderer(nullptr),
      input(nullptr),
      currentMap(nullptr),
      ui(nullptr),
      eventSystem(nullptr),
      deltaTime(0.0f),
      requiredWidth(0),
      requiredHeight(0),
      mode(EngineMode::GAMEPLAY),
      activeScript(nullptr) {}

GameEngine::~GameEngine() {
    shutdown();
}

void GameEngine::enterAltScreen() {
    // Enter alternate screen
    std::cout << "\033[?1049h";

    // Hide cursor
    std::cout << "\033[?25l";

    std::cout.flush();
}

void GameEngine::exitAltScreen() {
    // Show cursor
    std::cout << "\033[?25h";

    // Exit alternate screen
    std::cout << "\033[?1049l";

    std::cout.flush();
}

void GameEngine::init(int reqW, int reqH) {
    requiredWidth = reqW;
    requiredHeight = reqH;

    isRunning = true;
}

void GameEngine::run() {
    enterAltScreen();

    using clock = std::chrono::high_resolution_clock;
    auto lastTime = clock::now();

    while (isRunning) {

        // SCRIPT MODE
        if (mode == EngineMode::SCRIPT) {

            if (activeScript) {
                activeScript();     // run script fully
                activeScript = nullptr;
            }

            mode = EngineMode::GAMEPLAY;
            continue;
        }

        // NORMAL GAMEPLAY

        terminal.updateSize();
        if (!terminal.isPlayable(requiredWidth, requiredHeight)) {
            terminal.waitForValidSize(requiredWidth, requiredHeight);
        }

        auto currentTime = clock::now();
        std::chrono::duration<float> elapsed = currentTime - lastTime;
        deltaTime = elapsed.count();
        lastTime = currentTime;

        if (input) {
            input->pollInput();
        }

        update(deltaTime);
        render();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    exitAltScreen();
}

void GameEngine::update(float dt) {
    // Update all entities
    for (auto entity : entities) {
        if (entity) {
            entity->update(dt);
        }
    }

    // Process events
    if (eventSystem) {
        eventSystem->processEvents();
    }
}

void GameEngine::render() {
    if (!renderer) return;

    renderer->clear();

    if (currentMap) {
        renderer->drawMap(*currentMap);
    }

    for (auto entity : entities) {
        if (entity) {
            renderer->drawEntity(*entity);
        }
    }

    if (ui) {
        renderer->drawUI(*ui);
    }

    renderer->present();
}

void GameEngine::shutdown() {
    isRunning = false;
}

void GameEngine::addEntity(Entity* entity) {
    entities.push_back(entity);
}

// --- Dependency Injection Setters ---

void GameEngine::setRenderer(Renderer* r) {
    renderer = r;
}

void GameEngine::setInputHandler(InputHandler* i) {
    input = i;
}

void GameEngine::setMap(Map* m) {
    currentMap = m;
}

void GameEngine::setUI(UIManager* u) {
    ui = u;
}

void GameEngine::setEventSystem(EventSystem* e) {
    eventSystem = e;
}


// activates a script mode where the main loop will call the provided script function instead of the normal update/render flow.
// This allows for custom scripted sequences or behaviors without modifying the core engine loop. 
void GameEngine::runScript(std::function<void()> script) {
    activeScript = script;
    mode = EngineMode::SCRIPT;
}

// needs safe handling as scripts should run in cannonical mode not polled input mode. 
/* Safe handlingof a triiger: 
auto runSafeScript = [&](auto func) {
    engine.runScript([&]() {
        input.restore();
        func();
        input.init();
    });
};

call: 
runSafeScript(intro);
*/