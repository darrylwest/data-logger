//
// 2025-02-24 23:40:03 dpw
//

#include <ranges>
#include <vector>
#include <map>
#include <string>
#include <ctime>
#include <ranges>

#include <spdlog/spdlog.h>
#include <vendor/httplib.h>

#include <app/cli.hpp>
#include <app/database.hpp>
#include <app/jsonkeys.hpp>
#include <app/logging.hpp>
#include <app/service.hpp>
#include <app/types.hpp>
#include <app/version.hpp>
#include <cstdio>
#include <iostream>
#include <nlohmann/json.hpp>
#include <app/webhandlers.hpp>
#include <datetimelib/datetimelib.hpp>

namespace app {
    namespace webhandlers {
        using json = nlohmann::json;
        using app::database::Database;

        struct TempsData {
            std::time_t ts;
            Str label;
            float tempC;
            float tempF;
            Str location;
        };

        auto map_temps_data(const std::map<Str, Str>& temps) {
            Vec<TempsData> result;
            result.reserve(temps.size());
        
            auto view = temps
                | std::views::transform([](const auto& pair) {
                    const auto& [key, tempC] = pair;
        
                    // Find the first and last dots to extract all parts
                    auto first_dot = key.find('.');
                    if (first_dot == std::string::npos)
                        throw std::runtime_error("bad format");
        
                    try {
                        std::time_t ts = std::stoll(key.substr(0, first_dot));
                        Str label = datetimelib::ts_to_local_isodate(ts, "%R %p");
                        float float_tempC = std::stof(tempC);
                        float tempF = float_tempC * 1.8 + 32;
                        // Extract everything after the first dot for the location
                        std::string location = key.substr(first_dot + 1);
        
                        return TempsData{ts, label, float_tempC, tempF, location};
                    } catch (const std::exception&) {
                        throw;
                    }
                })
                | std::views::filter([](const TempsData& tv) {
                    return tv.ts != 0; // Filter out invalid entries
                });
        
            std::ranges::copy(view, std::back_inserter(result));
            return result;
        }

        ChartData create_chart_data(const Database& db, const std::time_t end_ts) {
            ChartData chart;

            chart.end_date = datetimelib::ts_to_local_isodate(end_ts, "%d-%b-%Y");
            chart.start_date = chart.end_date;
            spdlog::info("create chart data for {}", chart.end_date);

            // TODO fix this so it uses the end_ts; maybe use db.filter?
            const auto temps = db.last(25);
            const auto data = map_temps_data(temps);
            spdlog::info("data size: {}", data.size());

            // create the labels
            for (const auto& d : data) {
                spdlog::info("{} {} {:.5f}°C {:.5f}°F {}", d.ts, d.label, d.tempC, d.tempF, d.location);
            }

            // transform the labels

            // transform the temp C with location = location + "-C"

            // transform the temp F with location = location + "-F"

            return chart;
        }
    }
}
