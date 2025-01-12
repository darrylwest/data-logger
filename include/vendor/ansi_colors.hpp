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
    std::string red = "\033[0;31m";
    std::string green = "\033[0;32m";
    std::string yellow = "\033[0;33m";
    std::string blue = "\033[0;34m";
    std::string magenta = "\033[0;35m";
    std::string cyan = "\033[0;36m";
    std::string white = "\033[0;37m";

    // foreground colors
    namespace bright {
        std::string red = "\033[1;31m";
        std::string green = "\033[1;32m";
        std::string yellow = "\033[1;33m";
        std::string blue = "\033[1;34m";
        std::string magenta = "\033[1;35m";
        std::string cyan = "\033[1;36m";
        std::string white = "\033[1;37m";
    }

    std::string reset = "\033[0m";
}

#endif
