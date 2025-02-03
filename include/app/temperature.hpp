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
    struct TemperatureProbe {
        int sensor;
        std::string location;
        float tempC;
        float tempF;
    };

    struct TemperatureData {
        std::string reading_at;
        int timestamp;
        std::vector<TemperatureProbe> probes;

        friend std::ostream& operator<<(std::ostream& os, const TemperatureData v) {
            os << "reading_at: " << v.reading_at << ", ts: " << v.timestamp;

            for (const auto probe : v.probes)
                os << ", sensor: " << probe.sensor << ", location: " << probe.location
                   << ", tempC: " << probe.tempC << ", tempF: " << probe.tempF;

            return os;
        }

        std::string to_string() const {
            std::ostringstream oss;
            oss << *this;

            return oss.str();
        }
    };

    namespace temperature {
        // parse the json reading response
        TemperatureData parse_reading(const std::string& json_text);

    }  // namespace temperature
}  // namespace app
