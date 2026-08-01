#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <termios.h>
#include <unordered_set>

class InputHandler {
private:
    std::unordered_set<char> keysPressed;

    struct termios originalTermios;
    int originalFlags;

public:
    InputHandler();
    ~InputHandler();

    void init();
    void restore();

    void pollInput();

    bool isKeyPressed(char key) const;
};

#endif