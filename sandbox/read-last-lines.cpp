#include <iostream>
#include <fstream>
#include <string>
#include <deque>
#include <algorithm>

void read_last_lines(const std::string& filename, int num_lines) {
    std::ifstream file(filename, std::ios::in | std::ios::ate); // Open in read mode, initially positioned at the end
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file.\n";
        return;
    }

    std::streamoff file_size = file.tellg(); // Get file size (convert position to offset)
    std::string buffer;
    std::deque<std::string> lines;          // To store the last `num_lines` lines

    // Start seeking backward through the file
    for (std::streamoff pos = file_size - 1; pos >= 0; --pos) {
        file.seekg(pos, std::ios::beg); // Seek to position (convert offset)
        char c;
        file.get(c);

        if (c == '\n') { // Newline detected, push the buffer as a line
            if (!buffer.empty()) {
                std::reverse(buffer.begin(), buffer.end());  // Reverse string (read backward)
                lines.push_front(buffer);                   // Add line at the front
                buffer.clear();

                if (lines.size() > static_cast<size_t>(num_lines)) {
                    lines.pop_back(); // Remove any excess to store only the last `num_lines`
                }
            }
        } else {
            buffer += c; // Collect the characters
        }

        if (pos == 0) { // Handle edge case: Reading the very first characters of the file
            if (!buffer.empty()) {
                std::reverse(buffer.begin(), buffer.end());
                lines.push_front(buffer);
            }
        }
    }

    // Print the last lines
    for (const auto& line : lines) {
        std::cout << line << '\n';
    }
}

int main() {
    std::string filename = "../data/current.status.test"; // Replace with your file's name
    read_last_lines(filename, 6);
    return 0;
}

