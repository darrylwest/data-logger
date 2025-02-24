#include <iostream>
#include <ctime>

int main() {
    std::cout << "Size of std::time_t: " << sizeof(std::time_t) << " bytes" << std::endl;
    std::cout << "Size of time_t: " << sizeof(time_t) << " bytes" << std::endl;
    return 0;
}
