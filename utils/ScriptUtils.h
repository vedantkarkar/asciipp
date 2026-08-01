#ifndef SCRIPT_UTILS_H
#define SCRIPT_UTILS_H

#include <string>
#include <vector>

namespace ScriptUtils {

    namespace Color {
        extern const std::string RESET;
        extern const std::string BOLD;

        extern const std::string RED;
        extern const std::string GREEN;
        extern const std::string YELLOW;
        extern const std::string BLUE;
        extern const std::string MAGENTA;
        extern const std::string CYAN;
        extern const std::string WHITE;

        extern const std::string DIM;
        extern const std::string BLINK;
    }

    void typeText(const std::string& text, int delayMs = 30);
    void wait(int ms);
    void waitForEnter(int& enterCount);
    int getChoice(const std::vector<std::string>& options);

    // helper function to wrap text in color codes for easy colored output
    std::string colorText(const std::string& text, const std::string& color);

}

#endif