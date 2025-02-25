//
// 2025-02-24 23:40:03 dpw
//

#include <spdlog/spdlog.h>
#include <vendor/httplib.h>

#include <app/cli.hpp>
#include <app/database.hpp>
#include <app/jsonkeys.hpp>
#include <app/logging.hpp>
#include <app/service.hpp>
#include <app/types.hpp>
#include <app/version.hpp>
#include <app/webhandlers.hpp>
#include <cstdio>
#include <ctime>
#include <datetimelib/datetimelib.hpp>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <ranges>
#include <string>
#include <vector>

namespace app {
    namespace webhandlers {
        using json = nlohmann::json;
        using namespace app::database;

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

            auto view = temps | std::views::transform([](const auto& pair) {
                            const auto& [key, tempC] = pair;

                            // Find the first and last dots to extract all parts
                            auto first_dot = key.find('.');
                            if (first_dot == std::string::npos) throw std::runtime_error("bad format");

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
                              return tv.ts != 0;  // Filter out invalid entries
                          });

            std::ranges::copy(view, std::back_inserter(result));
            return result;
        }

        ChartData create_chart_data(const database::Database& db, const ChartConfig& cfg) {
            const auto end_date = datetimelib::ts_to_local_isodate(cfg.end_ts, "%d-%b-%Y");
            auto start_date = end_date;
            spdlog::info("create chart data for end date {}", end_date);

            // TODO fix this so it uses the end_ts; maybe use db.filter?
            const auto temps = db.last(25);
            const auto data = map_temps_data(temps);
            spdlog::info("data size: {}", data.size());

            // create the labels
            if (false) {
                for (const auto& d : data) {
                    spdlog::info("{} {} {:.2f}°C {:.2f}°F {}", d.ts, d.label, d.tempC, d.tempF, d.location);
                }
            }

            // TODO pick the first entry to get the start date

            // transform the labels
            Vec<Str> labels;
            Str location;
            Vec<float> tempsC;
            Vec<float> tempsF;

            for (const auto& t : data) {
                labels.push_back(t.label);
                location = t.location;
                tempsC.push_back(t.tempC);
                tempsF.push_back(t.tempF);
            }

            Str locC = location + ".C";
            Str locF = location + ".F";
            HashMap<Str, Vec<float>> readings = {{locC, tempsC}, {locF, tempsF}};

            // TODO modify the start date if it's on a different day than end date

            ChartData chart = {
                .start_date = start_date,
                .end_date = end_date,
                .temps = readings,
                .labels = labels,
            };

            return chart;
        }

        // build a json response the the UI can work with
        Str create_temps_response(const ChartData& chart) {
            json j;

            using namespace app::jsonkeys;

            // Add labels
            j[LABELS] = chart.labels;

            Vec<json> data;

            // Sensor 1 data
            const json sensor1
                = {{"sensor_id", "sensor_1"},
                   {"label", "cottage-south"},
                   {"data", {49.5781,  50.2531,  50.8156,  50.7406,  51.5281,  51.8094,  51.9688,  51.9969,  52.3906,
                             52.5312,  52.5219,  53.6375,  52.7,     52.82187, 52.82187, 49.71875, 47.75937, 46.46563,
                             45.94062, 44.51563, 44.51563, 43.95313, 43.95313, 43.86875, 43.86875}},
                   {"borderColor", "red"},
                   {"fill", false}};

            // Sensor 2 data
            const json sensor2 = {
                {"sensor_id", "sensor_2"},
                {"label", "shed-west"},
                {"data", {49.32900, 49.521116, 50.02136, 51.18444, 52.293116, 51.43887, 52.48388, 52.980233, 53.10152,
                          52.00872, 52.635993, 54.16272, 52.29799, 53.557073, 51.98579, 48.73718, 48.225931, 45.70019,
                          45.66436, 44.396992, 44.27969, 43.71025, 43.768674, 44.14863, 43.34765}},
                {"borderColor", "blue"},
                {"fill", false}};

            data.push_back(sensor1);
            data.push_back(sensor2);

            // Add datasets and end_date
            j[DATASETS] = data;
            j[END_DATE] = chart.end_date;

            return j.dump();
        };
    }  // namespace webhandlers
}  // namespace app
