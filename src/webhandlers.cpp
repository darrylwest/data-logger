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
#include <cstdio>
#include <iostream>
#include <nlohmann/json.hpp>
#include <app/webhandlers.hpp>
#include <datetimelib/datetimelib.hpp>

namespace app {
    namespace webhandlers {
        using json = nlohmann::json;
        using app::database::Database;

        ChartData create_chart_data(const Database& db, const std::time_t end_ts) {
            ChartData chart;

            chart.end_date = datetimelib::ts_to_local_isodate(end_ts);
            spdlog::info("create chart data for {}", chart.end_date);

            const auto temps = db.last(25);

            return chart;
        }
    }
}
