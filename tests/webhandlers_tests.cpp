//
// Created by Darryl West on 2/27/25.
//
#include <catch2/catch_all.hpp>  // For Catch2 v3
#include <app/types.hpp>
#include <app/webhandlers.hpp>

using namespace app;

TEST_CASE("WebHandlers Tests", "[webhandlers][chart_data]") {

    database::Database db;
    size_t size = 350;
    populate_database(db, size);
    REQUIRE(db.size() == size);
    const time_t ts = datetimelib::timestamp_seconds();
    const Vec<Str> locations = {"shed.0"};

    const webhandlers::ChartConfig cfg{ .end_ts = ts, .locations = locations };

    const auto data = webhandlers::create_chart_data(db, cfg);
    REQUIRE(data.labels.size() == cfg.data_points);

    for (const auto& label : data.labels) {
        spdlog::info("{}", label);
        REQUIRE(label.contains(":"));
        REQUIRE(label.size() == 6);
    }

    const auto temps = data.temps;

    for (const auto& [key, value] : temps) {
        REQUIRE(key == "shed.0.F");
        REQUIRE(value.size() == cfg.data_points);
        spdlog::info("{} {}", key, value.size());
    }
}

TEST_CASE("WebHandlers Tests", "[webhandlers][temps_response]") {
    using namespace app::jsonkeys;

    database::Database db;
    size_t size = 350;
    populate_database(db, size);
    REQUIRE(db.size() == size);
    const time_t ts = datetimelib::timestamp_seconds();
    const Vec<Str> locations = {"shed.0"};

    const webhandlers::ChartConfig cfg{ .end_ts = ts, .locations = locations };

    const auto chart = webhandlers::create_chart_data(db, cfg);
    REQUIRE(chart.labels.size() == cfg.data_points);

    const auto jstr = webhandlers::create_temps_response(chart);

    const json j = json::parse(jstr);
    std::println("{}", j.dump(4));

    REQUIRE(j[DATASETS].size() == 1); // one active probe
    const auto data = j[DATASETS].at(0);
    REQUIRE(data["sensor_id"] == "shed.0.F");

    const auto labels  = j[LABELS].get<Vec<Str>>();
    REQUIRE(labels.size() == cfg.data_points);
    REQUIRE(j[END_DATE] == chart.end_date);

}
