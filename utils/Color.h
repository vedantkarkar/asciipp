#ifndef COLOR_H
#define COLOR_H

#include <string>
#include <cstdint>

namespace Color {
    inline const std::string RESET = "\033[0m";

    // Text effects
    inline const std::string BOLD = "\033[1m";
    inline const std::string DIM = "\033[2m";
    inline const std::string ITALIC = "\033[3m";
    inline const std::string UNDERLINE = "\033[4m";
    inline const std::string SLOW_BLINK = "\033[5m";
    inline const std::string RAPID_BLINK = "\033[6m";
    inline const std::string REVERSE = "\033[7m";
    inline const std::string HIDDEN = "\033[8m";
    inline const std::string STRIKETHROUGH = "\033[9m";

    // Effect reset codes
    inline const std::string RESET_BOLD_DIM = "\033[22m";
    inline const std::string RESET_ITALIC = "\033[23m";
    inline const std::string RESET_UNDERLINE = "\033[24m";
    inline const std::string RESET_BLINK = "\033[25m";
    inline const std::string RESET_REVERSE = "\033[27m";
    inline const std::string RESET_HIDDEN = "\033[28m";
    inline const std::string RESET_STRIKETHROUGH = "\033[29m";

    // Standard foreground colors
    inline const std::string BLACK = "\033[30m";
    inline const std::string RED = "\033[31m";
    inline const std::string GREEN = "\033[32m";
    inline const std::string YELLOW = "\033[33m";
    inline const std::string BLUE = "\033[34m";
    inline const std::string MAGENTA = "\033[35m";
    inline const std::string CYAN = "\033[36m";
    inline const std::string WHITE = "\033[37m";
    inline const std::string DEFAULT = "\033[39m";

    // Bright foreground colors
    inline const std::string BRIGHT_BLACK = "\033[90m";
    inline const std::string BRIGHT_RED = "\033[91m";
    inline const std::string BRIGHT_GREEN = "\033[92m";
    inline const std::string BRIGHT_YELLOW = "\033[93m";
    inline const std::string BRIGHT_BLUE = "\033[94m";
    inline const std::string BRIGHT_MAGENTA = "\033[95m";
    inline const std::string BRIGHT_CYAN = "\033[96m";
    inline const std::string BRIGHT_WHITE = "\033[97m";

    // Standard background colors
    inline const std::string BG_BLACK = "\033[40m";
    inline const std::string BG_RED = "\033[41m";
    inline const std::string BG_GREEN = "\033[42m";
    inline const std::string BG_YELLOW = "\033[43m";
    inline const std::string BG_BLUE = "\033[44m";
    inline const std::string BG_MAGENTA = "\033[45m";
    inline const std::string BG_CYAN = "\033[46m";
    inline const std::string BG_WHITE = "\033[47m";
    inline const std::string BG_DEFAULT = "\033[49m";

    // Bright background colors
    inline const std::string BG_BRIGHT_BLACK = "\033[100m";
    inline const std::string BG_BRIGHT_RED = "\033[101m";
    inline const std::string BG_BRIGHT_GREEN = "\033[102m";
    inline const std::string BG_BRIGHT_YELLOW = "\033[103m";
    inline const std::string BG_BRIGHT_BLUE = "\033[104m";
    inline const std::string BG_BRIGHT_MAGENTA = "\033[105m";
    inline const std::string BG_BRIGHT_CYAN = "\033[106m";
    inline const std::string BG_BRIGHT_WHITE = "\033[107m";

    // Helpers for 256-color and truecolor output.
    inline std::string fg256(std::uint8_t code) {
        return "\033[38;5;" + std::to_string(static_cast<int>(code)) + "m";
    }

    inline std::string bg256(std::uint8_t code) {
        return "\033[48;5;" + std::to_string(static_cast<int>(code)) + "m";
    }

    inline std::string fgRGB(std::uint8_t r, std::uint8_t g, std::uint8_t b) {
        return "\033[38;2;" + std::to_string(static_cast<int>(r)) + ";" +
               std::to_string(static_cast<int>(g)) + ";" +
               std::to_string(static_cast<int>(b)) + "m";
    }

    inline std::string bgRGB(std::uint8_t r, std::uint8_t g, std::uint8_t b) {
        return "\033[48;2;" + std::to_string(static_cast<int>(r)) + ";" +
               std::to_string(static_cast<int>(g)) + ";" +
               std::to_string(static_cast<int>(b)) + "m";
    }
}

#endif
