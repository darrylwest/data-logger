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
                    // p.enabled = probe[ENABLED];
                    p.tempC = probe[TEMP_C];
                    p.tempF = probe[TEMP_F];

                    p.enabled = (p.tempC < -140) ? false : true;

                    data.probes.push_back(p);
                }

                spdlog::info("parsed to: {}", data.to_string());

            } catch (json::parse_error& e) {
                // should track errors centrally
                spdlog::error("Error parsing JSON: {}", e.what());
            }

            return data;
        }

        // TODO : generate json output for webapp/UI

    }  // namespace temperature
}  // namespace app
