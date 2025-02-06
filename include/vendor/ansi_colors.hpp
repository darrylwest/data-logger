//
// 2024-12-01 05:59:48 dpw
//
// Install: cp ansi_colors.hpp /usr/local/includes/
//

#ifndef ANSI_COLORS_INCLUDE
#define ANSI_COLORS_INCLUDE

#include <string>

// references: 
//  https://en.wikipedia.org/wiki/ANSI_escape_code#Colors
//  https://linux.die.net/man/5/terminfo
//  https://stackoverflow.com/questions/2616906/how-do-i-output-coloured-text-to-a-linux-terminal
namespace colors {
    constexpr std::string red = "\033[0;31m";
    constexpr std::string green = "\033[0;32m";
    constexpr std::string yellow = "\033[0;33m";
    constexpr std::string blue = "\033[0;34m";
    constexpr std::string magenta = "\033[0;35m";
    constexpr std::string cyan = "\033[0;36m";
    constexpr std::string white = "\033[0;37m";

    // foreground colors
    namespace bright {
        constexpr std::string red = "\033[1;31m";
        constexpr std::string green = "\033[1;32m";
        constexpr std::string yellow = "\033[1;33m";
        constexpr std::string blue = "\033[1;34m";
        constexpr std::string magenta = "\033[1;35m";
        constexpr std::string cyan = "\033[1;36m";
        constexpr std::string white = "\033[1;37m";
    }

    constexpr std::string reset = "\033[0m";
}

#endif
