#ifndef TERMINAL_UTILS_H
#define TERMINAL_UTILS_H

#include <iostream>

class TerminalUtils {
private:
    int termWidth;
    int termHeight;

public:
    TerminalUtils();
    
    void updateSize();
    int getWidth() const;
    int getHeight() const;

    bool isPlayable(int requiredWidth, int requiredHeight) const;

    void waitForValidSize(int requiredWidth, int requiredHeight);

    void clearScreen() const;
};

#endif