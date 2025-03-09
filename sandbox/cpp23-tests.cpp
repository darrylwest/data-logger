//
// dpw
//

#include <cstdio>
#include <print> // dies on linux
#include <format>
#include <spdlog/fmt/fmt.h>
#include <vendor/ansi_colors.hpp>

using namespace colors;

int main() {
    std::puts("this is a puts console output...");

    fmt::print("{}fmt.h formatting {}{:.3f}{}\n", green, yellow, 10.45678, reset);

    // what a pain!
    std::puts(std::format("{}format formatting {}{:.3f}{}", green, yellow, 10.876543, reset).c_str());

    std::println("this is a std::println test {}", 42);

    return 0;
}
