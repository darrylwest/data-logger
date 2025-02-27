//
//
// 2025-01-12 23:05:09 dpw
//

#pragma once

#include <app/types.hpp>
#include <nlohmann/json.hpp>
#include <sstream>

namespace app::temperature {
    using json = nlohmann::json;

    constexpr float C2F_RATIO = 1.8f;
    constexpr float C2F_OFFSET = 32.0f;

    struct Probe {
        int sensor;
        Str location;
        bool enabled;
        float tempC;
        float tempF;
    };

    struct TemperatureData {
        time_t reading_at;
        Vec<Probe> probes;

        friend std::ostream& operator<<(std::ostream& os, const TemperatureData& v) {
            os << "reading_at: " << v.reading_at;

            for (const auto& probe : v.probes)
                os << ", sensor: " << probe.sensor << ", location: " << probe.location << ", enabled: " << probe.enabled
                   << ", tempC: " << probe.tempC << ", tempF: " << probe.tempF;

            return os;
        }

        [[nodiscard]] Str to_string() const {
            std::ostringstream oss;
            oss << *this;

            return oss.str();
        }
    };

    // parse the probe node of json config
    Optional<Probe> parse_probe(const json& data, const Str& location);

    // parse the json reading response
    TemperatureData parse_reading(const Str& json_text);

    // convert celsius to fahrenheit
    float celsius_to_fahrenheit(float celsius);

}  // namespace app::temperature
