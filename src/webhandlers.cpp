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
#include <iostream>
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
                                Str label = datetimelib::ts_to_local_isodate(ts, "%I:%M%p");
                                // 
                                if (label.ends_with("AM")) {
                                    label = label.substr(0,5) + "a";
                                } else {
                                    label = label.substr(0,5) + "p";
                                }
                                
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
            const auto day_temps = db.last(300);

            SortedMap temps;
            auto it = day_temps.begin();
            for (std::size_t i = 0; i < day_temps.size(); i++) {
                if (i % 12 == 11) {
                    temps.emplace(*it);
                }
                ++it;
            }
            spdlog::info("temps: {}", temps.size());

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
            HashMap<Str, Vec<float>> readings;
            if (false) { 
                readings = {{locC, tempsC}, {locF, tempsF}};
            } else {
                readings = {{locF, tempsF}};
            }

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

            // Initialize an index to keep track of the current color
            size_t index = 0;

            // Create a lambda function to return the next color
            auto next_color = [&]() {
                // Get the current color
                Str color = common_colors[index];
                // Increment the index, wrapping around if necessary
                index = (index + 1) % common_colors.size();
                return color;
            };

            Vec<json> data;
            data.reserve(chart.temps.size());

            for (const auto& [key, values] : chart.temps) {
                const json sensor = {{"sensor_id", key}, {"label", key}, {"data", values}, {"borderColor", next_color()}, {"fill", false}};
                data.push_back(sensor);
            }

            // Add datasets and end_date
            j[DATASETS] = data;
            j[END_DATE] = chart.end_date;

            return j.dump();
        };
    }  // namespace webhandlers
}  // namespace app
