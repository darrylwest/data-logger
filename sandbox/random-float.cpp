#include <iostream>
#include <random>

// Function to generate a random integer in a specified range
int generateRandomInt(std::mt19937& gen, int min, int max) {
    std::uniform_int_distribution<> distr(min, max);
    return distr(gen);
}

// Function to generate a random float in a specified range
float generateRandomFloat(std::mt19937& gen, float min, float max) {
    std::uniform_real_distribution<float> distr(min, max);
    return distr(gen);
}

int main() {
    // Create a random device and a random number generator
    std::random_device rd;  // Obtain a random number from hardware
    std::mt19937 gen(rd());  // Seed the generator

    // Generate and print a random integer
    int random_int = generateRandomInt(gen, 1, 100);
    std::cout << "Random integer: " << random_int << std::endl;

    // Generate and print a random float
    float random_float = generateRandomFloat(gen, 1.0f, 100.0f);
    std::cout << "Random float: " << random_float << std::endl;

    return 0;
}

