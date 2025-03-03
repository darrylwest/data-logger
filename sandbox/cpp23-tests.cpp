//
// dpw
//

#include <cstdio>
#include <print>
#include <vendor/ansi_colors.hpp>

using namespace colors;

int main() {
    std::puts("this is a puts console output...");

    std::print("{}formatting {}{:.3f}{}\n", green, yellow, 10.45678, reset);

    return 0;
}
