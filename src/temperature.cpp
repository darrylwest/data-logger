//
// 2025-01-12 23:11:44 dpw
//

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include <app/temperature.hpp>

namespace app {
    using json = nlohmann::json;

    // parse json reading/probe response
    TemperatureData parse_reading(const std::string& json_text) {
        spdlog::info("parsed text: {}", json_text);

        TemperatureData data;
        try {
            json j = json::parse(json_text);

            auto read_at = j["reading_at"];
            data.reading_at = read_at["time"];
            data.timestamp = read_at["ts"];
            auto probes = j["probes"];
            for (const auto& probe : probes) {
                TemperatureProbe p;
                p.sensor = probe["sensor"];
                p.location = probe["location"];
                p.tempC = probe["tempC"];
                p.tempF = probe["tempF"];

                data.probes.push_back(p);
            }

            spdlog::info("parsed to: {}", data.to_string());

        } catch(json::parse_error& e) {
            // should track errors centrally
            spdlog::error("Error parsing JSON: {}", e.what());
        }

        return data;
    }

    // TODO : generate json output for webapp/UI

}  // namespace app
