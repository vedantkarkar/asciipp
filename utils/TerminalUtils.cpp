#include "TerminalUtils.h"

#include <sys/ioctl.h>
#include <unistd.h>
#include <cstdlib>

//const after every function just promises compiler that the function will not modify any member variables of the class. 
//This is useful for functions that are meant to be read-only and do not change the state of the object.

TerminalUtils::TerminalUtils() : termWidth(0), termHeight(0) {
    updateSize();
}

void TerminalUtils::updateSize() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    termWidth = w.ws_col;
    termHeight = w.ws_row;
}

int TerminalUtils::getWidth() const {
    return termWidth;
}

int TerminalUtils::getHeight() const {
    return termHeight;
}

bool TerminalUtils::isPlayable(int requiredWidth, int requiredHeight) const {
    return termWidth >= requiredWidth && termHeight >= requiredHeight;
}

void TerminalUtils::waitForValidSize(int requiredWidth, int requiredHeight) {
    while (true) {
        updateSize();

        if (isPlayable(requiredWidth, requiredHeight)) {
            break;
        }

        clearScreen(); //execution if the screen is smaller than required size, clear the screen and display a message prompting the user to resize the terminal.

        std::cout << "\n[ TERMINAL SIZE TOO SMALL ]\n";
        std::cout << "Required: " << requiredWidth << " x " << requiredHeight << "\n";
        std::cout << "Current : " << termWidth << " x " << termHeight << "\n";
        std::cout << "\nResize terminal to continue...\n";

        usleep(200000); // 200 ms
    }

    clearScreen(); // clean resume
}

void TerminalUtils::clearScreen() const {
    std::cout << "\033[2J\033[1;1H"; //clear the terminal screen and move the cursor to the top-left corner using ANSI escape codes (faster than system calls).
}