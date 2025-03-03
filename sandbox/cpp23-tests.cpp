//
// dpw
//

#include <cstdio>
// #include <print> dies on linux
#include <spdlog/fmt/fmt.h>
#include <vendor/ansi_colors.hpp>

using namespace colors;

int main() {
    std::puts("this is a puts console output...");

    fmt::print("{}formatting {}{:.3f}{}\n", green, yellow, 10.45678, reset);

    return 0;
}
