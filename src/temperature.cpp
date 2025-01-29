//
// 2025-01-12 23:11:44 dpw
//

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include <app/temperature.hpp>
#include <chrono>
#include <iostream>
#include <optional>
#include <regex>
#include <sstream>
#include <string>

namespace app {
    TemperatureData parse_reading(const std::string& location, const std::string& text) {
        app::TemperatureData data;
        data.location = location;

        std::istringstream iss(text);
        std::string line;

        while (std::getline(iss, line)) {
            // Extract readings
            if (line.starts_with("readings:")) {
                data.reading_date = line.substr(9);  // Extract substring after "readings:"
                data.isValid = true;
            }

            // Extract timestamp (ts)
            if (line.starts_with("ts:")) {
                try {
                    data.timestamp
                        = std::chrono::system_clock::from_time_t(std::stoi(line.substr(3)));
                } catch (const std::invalid_argument& e) {
                    std::cerr << "Error parsing timestamp: " << e.what() << std::endl;
                }
            }

            // Extract temperature
            if (line.starts_with("temp:")) {
                std::smatch matches;
                std::regex temp_regex(R"(tempC:([\d.]+), tempF:([\d.]+))");
                if (std::regex_search(line, matches, temp_regex)) {
                    data.tempC = std::stod(matches[1]);
                    data.tempF = std::stod(matches[2]);
                } else {
                    data.isValid = false;
                }
            }
        }

        return data;
    }

    // TODO : generate json output
}  // namespace app
