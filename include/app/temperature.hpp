//
//
// 2025-01-12 23:05:09 dpw
//

#pragma once

#include <app/types.hpp>
#include <iostream>
#include <sstream>

namespace app {
    struct TemperatureProbe {
        int sensor;
        Str location;
        float tempC;
        float tempF;
    };

    struct TemperatureData {
        Str reading_at;
        int timestamp;
        Vec<TemperatureProbe> probes;

        friend std::ostream& operator<<(std::ostream& os, const TemperatureData v) {
            os << "reading_at: " << v.reading_at << ", ts: " << v.timestamp;

            for (const auto& probe : v.probes)
                os << ", sensor: " << probe.sensor << ", location: " << probe.location
                   << ", tempC: " << probe.tempC << ", tempF: " << probe.tempF;

            return os;
        }

        Str to_string() const {
            std::ostringstream oss;
            oss << *this;

            return oss.str();
        }
    };

    namespace temperature {
        // parse the json reading response
        TemperatureData parse_reading(const Str& json_text);

    }  // namespace temperature
}  // namespace app
