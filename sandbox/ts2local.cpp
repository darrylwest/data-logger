//
// convert the unix timestamp to a local date
//

#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

std::string unixTimestampToLocal(std::time_t timestamp) {
    // Convert to chrono::system_clock::time_point
    std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(timestamp);

    // Convert to time_t for formatting
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);

    // Convert to local time
    std::tm local_tm = *std::localtime(&tt);

    // Format as ISO 8601
    std::ostringstream oss;
    oss << std::put_time(&local_tm, "%Y-%m-%dT%H:%M:%S%z");

    return oss.str();
}

std::string unixTimestampToZulu(std::time_t timestamp) {
    // Convert to chrono::system_clock::time_point
    std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(timestamp);

    // Convert to time_t for formatting
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);

    // Convert to zulu time
    std::tm utc_tm = *std::gmtime(&tt);

    // Format as ISO 8601
    std::ostringstream oss;
    oss << std::put_time(&utc_tm, "%Y-%m-%dT%H:%M:%SZ");

    return oss.str();
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Error: Use: " << argv[0] << " unix-timestamp" << std::endl;
        return -1;
    }

    std::string cmd = argv[0];

    try {
        // Convert command-line argument to std::time_t
        std::time_t timestamp = static_cast<std::time_t>(std::stoll(argv[1]));

        const auto dt = (cmd.find("local") != std::string::npos) ? unixTimestampToLocal(timestamp) : unixTimestampToZulu(timestamp);

        std::cout << dt << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Invalid timestamp: " << argv[1] << "\nError: " << e.what() << std::endl;
        return 1;
    }


    return 0;
}
