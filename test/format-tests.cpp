#include <fmt/core.h>     // for basic formatting
#include <fmt/chrono.h>   // for time formatting (optional)
// #include <vector>

int main() {
    // Basic usage
    std::string name = "Darryl";
    int value = 42;
    
    // Simple printing with arguments
    fmt::print("Hello, {}! The answer is {}\n", name, value);
    
    // Named arguments
    fmt::print("Hello, {name}! The answer is {val}\n", 
               fmt::arg("name", name), 
               fmt::arg("val", value));
    
    // Current time formatting (using your timestamp)
    auto now = "2025-02-18 00:04:41";
    fmt::print("Current time: {}\n", now);

    // Number formatting
    fmt::print("Integer: {:d}, Float: {:.2f}\n", 42, 3.14159);  // prints "Integer: 42, Float: 3.14"

    // Width and alignment
    fmt::print("{:>10}\n", "right");   // right-aligned, width 10
    fmt::print("{:<10}\n", "left");    // left-aligned, width 10
    fmt::print("{:^10}\n", "center");  // center-aligned, width 10

    // Debug formatting (shows type information)
    // std::vector<int> v = {1, 2, 3};
    // fmt::print("{:?}\n", v);  // prints vector contents with debug formatting
    
    return 0;
}
