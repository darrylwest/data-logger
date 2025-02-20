//
// 2025-01-12 23:11:44 dpw
//

#include <spdlog/spdlog.h>

#include <app/temperature.hpp>
#include <nlohmann/json.hpp>

namespace app {
    namespace temperature {
        using json = nlohmann::json;

        // parse json reading/probe response
        TemperatureData parse_reading(const Str& json_text) {
            spdlog::debug("parsed text: {}", json_text);

            TemperatureData data;
            try {
                json j = json::parse(json_text);
                using namespace app::jsonkeys;

                time_t read_at = j[READING_AT].template get<int>();
                data.reading_at = read_at;
                auto probes = j[PROBES];
                for (const auto& probe : probes) {
                    TemperatureProbe p;
                    p.sensor = probe[SENSOR];
                    p.location = probe[LOCATION];
                    p.tempC = probe[TEMP_C];
                    p.tempF = probe[TEMP_F];

                    if (probe.contains(ENABLED)) {
                        p.enabled = probe[ENABLED];
                    } else {
                        p.enabled = (p.tempC < -100) ? false : true;
                    }

                    data.probes.push_back(p);
                }

                spdlog::info("parsed to: {}", data.to_string());

            } catch (json::parse_error& e) {
                // should track errors centrally
                spdlog::error("Error parsing JSON: {}", e.what());
            }

            return data;
        }

        // parse the config for probe locations and enabled
        Optional<TemperatureProbe> parse_probe(const json& data, const Str& location) {
            using namespace app::jsonkeys;

            if (!data.contains(SENSORS) && data[SENSORS].is_array()) return std::nullopt;

            for (const auto& sensor_data : data[SENSORS]) {
                spdlog::info("j sensor {}", sensor_data.dump());
                if (sensor_data.contains(PROBES) && sensor_data[PROBES].is_array()) {
                    for (const auto& jprobe : sensor_data[PROBES]) {
                        if (jprobe.is_object() && jprobe[LOCATION] == location) {
                            TemperatureProbe probe;
                            probe.sensor = jprobe.value(SENSOR, -1);
                            probe.location = jprobe[LOCATION];
                            probe.enabled = jprobe.value(ENABLED, false);

                            return probe;
                        }
                    }
                }
            }

            return std::nullopt;
        }

    }  // namespace temperature
}  // namespace app
