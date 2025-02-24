
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>

std::string unixTimestampToLocalISO8601(std::time_t unix_timestamp) {
    // Convert time_t to std::tm (local time)
    std::tm local_tm = *std::localtime(&unix_timestamp);

    // Format the local time as ISO8601
    std::ostringstream oss;
    oss << std::put_time(&local_tm, "%FT%T%z");

    return oss.str();
}

int main() {
    std::time_t unix_timestamp = 1740357970; // Example Unix timestamp
    std::string local_iso8601 = unixTimestampToLocalISO8601(unix_timestamp);

    std::cout << "Local ISO8601 date: " << local_iso8601 << std::endl;
    return 0;
}
