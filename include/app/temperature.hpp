//
//
// 2025-01-12 23:05:09 dpw
//

#pragma once

#include <app/types.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

namespace app {
    using json = nlohmann::json;

    struct TemperatureProbe {
        int sensor;
        Str location;
        bool enabled;
        float tempC;
        float tempF;
    };

    struct TemperatureData {
        time_t reading_at;
        Vec<TemperatureProbe> probes;

        friend std::ostream& operator<<(std::ostream& os, const TemperatureData v) {
            os << "reading_at: " << v.reading_at;

            for (const auto& probe : v.probes)
                os << ", sensor: " << probe.sensor << ", location: " << probe.location
                   << ", enabled: " << probe.enabled << ", tempC: " << probe.tempC
                   << ", tempF: " << probe.tempF;

            return os;
        }

        Str to_string() const {
            std::ostringstream oss;
            oss << *this;

            return oss.str();
        }
    };

    namespace temperature {
        // parse the probe node of json config
        Optional<TemperatureProbe> parse_probe(const json& data, const Str& location);

        // parse the json reading response
        TemperatureData parse_reading(const Str& json_text);

    }  // namespace temperature
}  // namespace app
