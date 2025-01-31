//
// 2025-01-12 23:11:44 dpw
//

// #include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#include <iostream>
#include <nlohmann/json.hpp>

#include <app/temperature.hpp>

// read json response with (see data-logger/data-reader/parse_ndjson.cpp)
namespace app {
    using json = nlohmann::json;

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

    // TODO : generate json output
}  // namespace app
