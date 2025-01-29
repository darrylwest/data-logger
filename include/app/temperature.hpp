//
//
// 2025-01-12 23:05:09 dpw
//

#pragma once

#include <spdlog/spdlog.h>

#include <chrono>
#include <iostream>
#include <optional>
#include <regex>
#include <sstream>
#include <string>

namespace app {
    struct TemperatureData {
        std::string location = "Unknown";
        std::string reading_date;
        std::chrono::system_clock::time_point timestamp;
        double tempC = 0.0;
        double tempF = 0.0;
        bool isValid = false;

        friend std::ostream& operator<<(std::ostream& os, const TemperatureData v) {
            // better to use <format> but it breaks on linux and fmt broken on darwin
            std::string valid = v.isValid ? "true" : "false";
            os << "location: " << v.location << ", "
               << "date: " << v.reading_date << ", "
               << "ts: " << std::chrono::system_clock::to_time_t(v.timestamp) << ", "
               << "tempC: " << v.tempC << ","
               << "tempF: " << v.tempF << "."
               << "valid: " << valid << ".";
            return os;
        }

        std::string to_string() const {
            std::ostringstream oss;
            oss << *this;

            return oss.str();
        }
    };
    // parse the raw reading
    TemperatureData parse_reading(const std::string& location, const std::string& text);
}  // namespace app
