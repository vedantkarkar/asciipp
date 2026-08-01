#include "InputHandler.h"

#include <unistd.h>
#include <fcntl.h>

InputHandler::InputHandler() : originalFlags(0) {}

InputHandler::~InputHandler() {
    restore();
}

void InputHandler::init() {
    // Save terminal settings
    tcgetattr(STDIN_FILENO, &originalTermios);

    struct termios newTermios = originalTermios;

    // Disable canonical mode and echo
    newTermios.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);

    // Save file descriptor flags
    originalFlags = fcntl(STDIN_FILENO, F_GETFL, 0);

    // Enable non-blocking mode
    fcntl(STDIN_FILENO, F_SETFL, originalFlags | O_NONBLOCK);
}

void InputHandler::restore() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalTermios);
    fcntl(STDIN_FILENO, F_SETFL, originalFlags);
}

void InputHandler::pollInput() {
    keysPressed.clear();

    char ch;
    while (read(STDIN_FILENO, &ch, 1) > 0) {
        keysPressed.insert(ch);
    }
}

bool InputHandler::isKeyPressed(char key) const {
    return keysPressed.count(key) > 0;
}