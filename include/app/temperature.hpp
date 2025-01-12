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
        std::string date;
        std::chrono::system_clock::time_point timestamp;
        std::optional<double> tempC;
        std::optional<double> tempF;

        friend std::ostream& operator<<(std::ostream& os, const Config v) {
            // better to use <format> but it breaks on linux and fmt broken on darwin
            os << "date: " << v.host << ", "
               << "ts: " << v.timestamp << ", "
               << "tempC: " << v.tempC << ",";
            << "tempF: " << v.tempF << ".";
            return os;
        }

        std::string to_string() const {
            std::ostringstream oss;
            oss << *this;

            return oss.str();
        }
    };
    // parse the raw reading
    TemperatureData parse_reading(const std::string& text);
}  // namespace app

