//
// g++ -std=c++20 -o truncate-minutes truncate-minutes.cpp
//
#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

std::string truncate_to_nearest(const std::string& iso8601, const int minute = 5) {
    // Parse the ISO 8601 datetime string
    std::tm tm = {};
    std::istringstream ss(iso8601);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    
    // Convert to time_point
    auto timePoint = std::chrono::system_clock::from_time_t(std::mktime(&tm));

    // Truncate to nearest 5 minutes
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(timePoint.time_since_epoch()) % minute;
    timePoint -= minutes;

    // Convert back to time_t
    std::time_t truncatedTime = std::chrono::system_clock::to_time_t(timePoint);
    std::tm* truncatedTm = std::localtime(&truncatedTime);

    // Format the result back to ISO 8601 without seconds
    std::ostringstream result;
    result << std::put_time(truncatedTm, "%Y-%m-%dT%H:%M");
    return result.str();
}

int main() {
    std::string iso8601 = "2025-02-05T07:49:22";
    std::string truncated = truncate_to_nearest(iso8601, 10);
    std::cout << iso8601 << " -> " << truncated << std::endl;

    iso8601 = "2025-02-05T07:51:22";
    truncated = truncate_to_nearest(iso8601, 10);
    std::cout << iso8601 << " -> " << truncated << std::endl;

    iso8601 = "2025-02-05T08:01:22";
    truncated = truncate_to_nearest(iso8601, 10);
    std::cout << iso8601 << " -> " << truncated << std::endl;

    return 0;
}

