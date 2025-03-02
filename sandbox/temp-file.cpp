
#include <iostream>
#include <filesystem>
#include <fstream>
#include <random>

std::filesystem::path getTempFilename() {
    // Get the system's temporary directory
    std::filesystem::path tempDir = std::filesystem::temp_directory_path();

    // Create a random device and a random number generator
    std::random_device rd;  // Obtain a random number from hardware
    std::mt19937 gen(rd()); // Seed the generator

    // Generate a unique filename
    std::uniform_int_distribution<int> distr(1'000'000, 9'999'999); // Random number between 100000 and 999999
    std::string filename = "tempfile_" + std::to_string(distr(gen)) + ".tmp"; // Unique filename

    // Create a temporary file path
    std::filesystem::path tempFilePath = tempDir / filename;

    // Create the temporary file
    std::ofstream ofs(tempFilePath);
    if (!ofs) {
        throw std::runtime_error("Failed to create temporary file");
    }

    // Close the file (optional, as it will be closed when the ofstream goes out of scope)
    ofs.close();

    return tempFilePath;
}

int main() {
    try {
        std::filesystem::path tempFile = getTempFilename();
        std::cout << "Temporary file created: " << tempFile << std::endl;

        // Optionally, you can write to the temporary file
        std::ofstream ofs(tempFile);
        ofs << "This is a temporary file." << std::endl;

        // Remember to delete the temporary file when done
        std::filesystem::remove(tempFile);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

