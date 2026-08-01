# asciipp

**asciipp** is a modern C++ library for building terminal games and interactive console applications.

Instead of repeatedly writing low-level terminal code for every project, asciipp provides a reusable engine that lets you focus on gameplay and application logic.

Built using only the C++ Standard Library and ANSI terminal features, the library aims to make terminal application development clean, modular, and extensible.

## Philosophy

asciipp follows a simple principle:

> Build terminal applications the same way modern game engines build graphical games.

Instead of manually printing text every frame, the engine separates rendering, input, entities, UI, and game logic into reusable systems.

The goal is not to replace graphical engines, but to provide an elegant framework for rapid development of terminal-based games, visualizations, simulations, educational projects, and developer tools.

**Build terminal applications like games. Build terminal games like engines.**

## Workflow

The intended consumer workflow is:

- keep the engine code as the reusable library
- write your game in `programs/<your_game>.cpp`
- include only `#include "asciipp_engine.h"`
- build your program with the provided `Makefile`

## Features

- Linux terminal rendering with ANSI escape sequences
- Alternate-screen game loop with hidden cursor
- Non-blocking `termios` input polling
- ASCII map storage and rendering
- Base `Entity` class plus `Player` and `Enemy` examples
- Trigger tiles for map-driven events
- HUD and side-panel UI support
- Event queue for deferred actions
- Script utilities for dialogues and cutscenes
- ANSI color helpers including standard, bright, background, 256-color, and RGB modes

## Requirements

- Linux
- ANSI-compatible terminal
- C++17 compiler such as `g++`
- `make`

## Reusable Library Layout

```text
asciipp_engine.h   Public umbrella header for users
core/              Engine loop
entity/            Entity base class and example entities
input/             Raw keyboard input handling
rendering/         ASCII frame rendering
systems/           UI manager and event queue
utils/             Terminal, color, and script helpers
world/             Map, collision, and triggers
programs/          User game entry points
Makefile           Library and program build workflow
```

## Public Include

User code can include the entire library with:

```cpp
#include "asciipp_engine.h"
```

This umbrella header includes:

- `GameEngine`
- `Renderer`
- `InputHandler`
- `Map`
- `Entity`
- `Player`
- `Enemy`
- `UIManager`
- `EventSystem`
- `TerminalUtils`
- `ScriptUtils`
- `Color`

## Build Workflow

### Build the default program

```bash
make
```

This builds:

- `build/lib/libasciipp_engine.a`
- `build/bin/game`

### Run the default program

```bash
make run
```

### Rename the output program

To choose your own binary name:

```bash
make PROGRAM=my_game
```

That produces:

```text
build/bin/my_game
```

### Build a different source file from `programs/`

If your game source is `programs/maze.cpp`:

```bash
make PROGRAM=maze PROGRAM_SRC=programs/maze.cpp
```

### Build only the library

```bash
make library
```

### Clean build artifacts

```bash
make clean
```

### Show Makefile usage

```bash
make help
```

## Using Make Efficiently

For normal development, you do not need to rebuild everything manually each time.

Useful habits:

- Run `make` for incremental builds. `make` recompiles only the files that changed.
- Run `make run` when you want to build and immediately launch the current program.
- Run `make PROGRAM=my_game` when you want the output binary to match your game's name.
- Run `make PROGRAM=my_game PROGRAM_SRC=programs/my_game.cpp` when switching between multiple games in `programs/`.
- Run `make library` if you only want to refresh the engine archive without linking a program.
- Run `make clean` only when you want a full rebuild or when stale build artifacts are suspected.

Typical workflows:

Build the default example incrementally:

```bash
make
```

Build and run your own game:

```bash
make PROGRAM=my_game PROGRAM_SRC=programs/my_game.cpp
make run PROGRAM=my_game PROGRAM_SRC=programs/my_game.cpp
```

Force a fully clean rebuild:

```bash
make clean
make PROGRAM=my_game PROGRAM_SRC=programs/my_game.cpp
```

Important note:

- `PROGRAM` changes the output binary name
- `PROGRAM_SRC` changes which `programs/*.cpp` file is compiled as the game's entry point
- engine source files are still compiled only when needed

## How to Use the Library in This Repository

The recommended user workflow is:

1. Create a new source file in `programs/`
2. Include `asciipp_engine.h`
3. Write your `main()`
4. Build with `make PROGRAM=<name> PROGRAM_SRC=programs/<file>.cpp`

Example:

```bash
touch programs/my_game.cpp
make PROGRAM=my_game PROGRAM_SRC=programs/my_game.cpp
```

## Beginner Quick Start

Minimal user program:

```cpp
#include "asciipp_engine.h"

int main() {
    GameEngine engine;
    engine.init(100, 30);

    Renderer renderer(100, 30);
    InputHandler input;
    Map map;
    UIManager ui;
    EventSystem events;

    input.init();

    engine.setRenderer(&renderer);
    engine.setInputHandler(&input);
    engine.setMap(&map);
    engine.setUI(&ui);
    engine.setEventSystem(&events);

    map.load({
        "##########",
        "#........#",
        "#...D....#",
        "#........#",
        "##########"
    });

    Player player(1, 1);
    player.attachInput(&input);
    player.attachMap(&map);
    engine.addEntity(&player);

    ui.setStats(player.getHealth(), 100, player.getScore(), 60);
    ui.setMessages({
        "WASD to move",
        "Reach D"
    });

    engine.run();
    return 0;
}
```

Build it:

```bash
make PROGRAM=my_game PROGRAM_SRC=programs/my_game.cpp
```

The example program included with the repository is [programs/main.cpp].

If you also use `utils/Color.h`, prefer `Color::...` directly and avoid broad `using namespace ScriptUtils;` imports in the same file, because `ScriptUtils` also exposes a nested `Color` namespace for its own helpers.

## Engine Setup Sequence

Most games follow this order:

1. Construct `GameEngine`
2. Set required terminal size with `engine.init(width, height)`
3. Construct `Renderer`, `InputHandler`, `Map`, `UIManager`, and `EventSystem`
4. Call `input.init()`
5. Inject systems into the engine
6. Load the map
7. Create entities and attach their dependencies
8. Configure UI and triggers
9. Call `engine.run()`

## Core API Overview

### `GameEngine`

Header: [core/GameEngine.h].

Main responsibilities:

- runs the main loop
- checks terminal size
- updates entities
- processes queued events
- renders map, entities, and UI
- manages alternate screen mode

Main methods:

- `init(int reqW, int reqH)`
- `run()`
- `shutdown()`
- `addEntity(Entity* entity)`
- `setRenderer(Renderer*)`
- `setInputHandler(InputHandler*)`
- `setMap(Map*)`
- `setUI(UIManager*)`
- `setEventSystem(EventSystem*)`
- `runScript(std::function<void()>)`

### `Renderer`

Header: [rendering/Renderer.h].

Main methods:

- `clear()`
- `drawMap(const Map&)`
- `drawEntity(const Entity&)`
- `drawUI(const UIManager&)`
- `drawAt(int x, int y, char c)`
- `present()`

Current rendering notes:

- the frame buffer currently stores plain characters, not styled cells
- colors are currently applied as a quick workaround inside `present()`
- colorized glyphs in the map region currently include:
  - `P` / `p` -> green
  - `E` / `e` -> red
  - `D` -> yellow
  - `@` -> bright magenta

If you want another map character to be colored right now, add a rule in [rendering/Renderer.cpp].

### `Map`

Header: [world/Map.h].

Main methods:

- `load(const std::vector<std::string>& mapData)`
- `getGrid() const`
- `getTile(int x, int y) const`
- `setTile(int x, int y, char value)`
- `isBlocked(int x, int y) const`
- `addTrigger(char symbol, std::function<void()> callback)`
- `checkTrigger(int x, int y)`

Current defaults:

- `'#'` is blocked
- trigger tiles are consumed and replaced with `'.'` when activated

### `Entity`

Header: [entity/Entity.h].

Base requirements:

- implement `update(float dt)`

Optional override:

- `getRenderSymbol() const`

Useful inherited methods:

- `updateAnimation(float dt)`
- `getX() const`
- `getY() const`
- `setPosition(int newX, int newY)`
- `alive() const`
- `kill()`

### `Player`

Header: [entity/Player.h].

Main methods:

- `attachInput(InputHandler*)`
- `attachMap(Map*)`
- `getHealth() const`
- `getScore() const`
- `enableGlitch(int factor)`
- `disableGlitch()`

Current behavior:

- reads `W`, `A`, `S`, `D`
- supports animated `P` / `p` rendering
- handles collision against the map
- checks trigger tiles after movement

### `Enemy`

Header: [entity/Enemy.h].

Main methods:

- `attachPlayer(Player*)`
- `attachMap(Map*)`

Current behavior types:

- `0` random movement
- `1` patrol placeholder
- `2` move toward player within detection radius

### `InputHandler`

Header: [input/InputHandler.h].

Main methods:

- `init()`
- `restore()`
- `pollInput()`
- `isKeyPressed(char key) const`

Important:

- call `init()` before gameplay if you want real-time input
- call `restore()` before using blocking input such as `std::cin`

### `UIManager`

Header: [systems/UIManager.h].

Main methods:

- `UIManager(int uiH = 4, int sideW = 30)`
- `setStats(int h, int maxH, int s, int t)`
- `setMessages(const std::vector<std::string>& msgs)`
- `setSideArt(const std::vector<std::string>& art)`
- `getUIHeight() const`
- `getSideWidth() const`

What it draws:

- bottom status area
- health bar
- score and time
- multi-line messages
- right side panel art

### `EventSystem`

Header: [systems/EventSystem.h].

Main methods:

- `trigger(std::function<void()> func)`
- `processEvents()`
- `clear()`

Typical usage:

```cpp
map.addTrigger('D', [&]() {
    events.trigger([&]() {
        ui.setMessages({"Door reached"});
    });
});
```

### `ScriptUtils`

Headers:

- [utils/ScriptUtils.h].
- [utils/Color.h].

Main methods:

- `typeText(const std::string& text, int delayMs = 30)`
- `wait(int ms)`
- `waitForEnter(int& enterCount)`
- `getChoice(const std::vector<std::string>& options)`
- `colorText(const std::string& text, const std::string& color)`

Important integration rule:

- gameplay input uses raw non-canonical mode
- `ScriptUtils` uses blocking terminal input
- restore input before a script, then re-enable raw input after the script

Safe pattern:

```cpp
engine.runScript([&]() {
    input.restore();
    testScript();
    input.init();
});
```

## Creating Your Own Entity

Example:

```cpp
#include "asciipp_engine.h"

class Coin : public Entity {
public:
    Coin(int x, int y) : Entity(x, y, '$', 0.4f) {}

    void update(float dt) override {
        updateAnimation(dt);
    }

    char getRenderSymbol() const override {
        return animState ? '$' : 'o';
    }
};
```

Then register it:

```cpp
Coin coin(12, 8);
engine.addEntity(&coin);
```

## Color Utility

Header: [utils/Color.h].

Available categories:

- standard foreground colors
- bright foreground colors
- standard background colors
- bright background colors
- text effects such as `BOLD`, `DIM`, and `UNDERLINE`
- effect reset codes
- 256-color helpers
- RGB color helpers

Examples:

```cpp
std::cout << Color::BOLD << Color::BRIGHT_GREEN << "Ready" << Color::RESET << "\n";
std::cout << Color::fg256(208) << "Orange-ish text" << Color::RESET << "\n";
std::cout << Color::bgRGB(25, 25, 80) << Color::WHITE << "Panel" << Color::RESET << "\n";
```

## Current Limitations

- Linux-only implementation
- no install packaging yet
- current renderer buffer stores characters only, not per-cell style data
- gameplay and UI still share one plain character buffer
- no camera or viewport system
- no smart-pointer ownership model for entities
- no built-in save/load system
- no built-in pathfinding
- no formal test suite

## Troubleshooting

### The terminal is too small

Resize the terminal to at least the size passed to `engine.init(width, height)`.

### Script input behaves incorrectly

You likely used blocking input without restoring terminal mode. Use:

```cpp
input.restore();
// script or menu code
input.init();
```

### Colors are not what I expected

Check:

- your terminal supports ANSI colors
- you reset styles with `Color::RESET`
- for map glyph colors, the current workaround rule exists in `Renderer::present()`

### The terminal looks broken after a crash

Run:

```bash
reset
```

## Recommended Next Refactors

1. Replace the character-only render buffer with a real cell buffer storing glyph and style.
2. Add safer entity ownership with smart pointers.
3. Separate UI rendering from gameplay rendering more explicitly.
4. Add install-friendly packaging such as `include/`, `src/`, and optionally `CMakeLists.txt`.

## License

No license file is currently present in this repository. Add one before distributing the library to other users.
