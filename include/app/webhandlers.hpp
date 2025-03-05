//
// 2025-02-24 23:40:03 dpw
//

#pragma once

#include <app/database.hpp>
#include <app/types.hpp>
#include <iostream>
#include <nlohmann/json.hpp>

namespace app::webhandlers {
    using json = nlohmann::json;
    using app::database::Database;

    struct ChartConfig {
        std::time_t end_ts;
        Vec<Str> locations;
        int data_points = 25;
    };

    struct ChartData {
        Str start_date;
        Str end_date;
        HashMap<Str, Vec<float>> temps;
        Vec<Str> labels;
    };

    // compatible with Chart.js
    const std::array<Str, 8> common_colors
        = {"red", "green", "blue", "yellow", "black", "cyan", "magenta", "orange"};

    // read the temperature database and create the chart data
    ChartData create_chart_data(const Database& db, const ChartConfig& cfg);

    // create and return the json chart data
    Str create_temps_response(const ChartData& chart);

}  // namespace app::webhandlers
