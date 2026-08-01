#include "ScriptUtils.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <limits>

namespace ScriptUtils {

    namespace Color {
        const std::string RESET  = "\033[0m";
        const std::string BOLD   = "\033[1m";

        const std::string RED    = "\033[31m";
        const std::string GREEN  = "\033[32m";
        const std::string YELLOW = "\033[33m";
        const std::string BLUE   = "\033[34m";
        const std::string MAGENTA= "\033[35m";
        const std::string CYAN   = "\033[36m";
        const std::string WHITE  = "\033[37m";

        const std::string DIM    = "\033[2m";
        const std::string BLINK  = "\033[5m";
    }

    void typeText(const std::string& text, int delayMs) {
        for (char c : text) {
            std::cout << c << std::flush;
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        }
    }

    void wait(int ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

    void waitForEnter(int& enterCount) {
        using namespace Color;

        enterCount++;
        std::cout << "\n";

        if (enterCount == 1) {
            std::cout << YELLOW << "[ Press Enter to continue... ]" << RESET;
        }
        else if (enterCount == 2) {
            std::cout << YELLOW << "[ Press Enter if you’re still there... ]" << RESET;
        }
        else if (enterCount == 3) {
            std::cout << RED << "[ Press Enter. ]" << RESET;
            std::cout << "\a";
        }
        else {
            std::cout << RED << "[ Pr3ss Ente#... ]" << RESET;
            std::cout << "\a";
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    int getChoice(const std::vector<std::string>& options) {
        int choice = -1;

        while (true) {
            std::cout << "\n";

            for (int i = 0; i < (int)options.size(); i++) {
                std::cout << i + 1 << ". " << options[i] << "\n";
            }

            std::cout << "\nEnter choice: ";
            std::cin >> choice;

            if (std::cin.fail() || choice < 1 || choice > (int)options.size()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid choice. Try again.\n";
                continue;
            }

            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return choice - 1;
        }
    }

    std::string colorText(const std::string& text, const std::string& color) {
        return color + text + Color::RESET;
    }

}