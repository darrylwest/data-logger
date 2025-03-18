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
}

TEST_CASE("WebHandlers Tests", "[webhandlers][temps_response]") {
    // TODO implement
    const Str ss = "test string";
    REQUIRE(ss == "test string");
}
