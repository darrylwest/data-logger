//
// Created by Darryl West on 2/27/25.
//
#include <catch2/catch_all.hpp>  // For Catch2 v3
#include <app/types.hpp>
#include <datetimelib/datetimelib.hpp>

TEST_CASE("DateTimeLib::timestamp_seconds()", "[datetimelib][seconds]") {
    auto unix_ts = datetimelib::timestamp_seconds();
    std::time_t tsz = 1738888855;
    INFO("tsz is in the past, 2025-02-06");
    REQUIRE(tsz < unix_ts);
}

TEST_CASE("DateTimeLib::timestamp_millis()", "[datetimelib][millis]") {
    std::time_t tszl = 1738888855000;
    auto mts = datetimelib::timestamp_millis();
    spdlog::info("millis ts: {}", mts);
    INFO("millis ts should be in the future");
    REQUIRE(mts > tszl);
}

TEST_CASE("DateTimeLib Timestamp Tests", "[datetimelib][seconds][millis]") {
    std::time_t tms = datetimelib::timestamp_millis();
    std::time_t tss = datetimelib::timestamp_seconds();
    spdlog::info("tms: {}, tss: {}", tms, tss);
    INFO( "times should match");
    REQUIRE(tms / 10000 == tss / 10);
}

TEST_CASE("DateTimeLib Format Tests", "[datetimelib][format]") {
    // format with std::put_time
    std::time_t tsz = 1738888855;
    auto iso_dt = datetimelib::ts_to_local_isodate(tsz, "%F");
    spdlog::info("ts: {}, iso: {}", tsz, iso_dt);
    INFO("just the date using formatting");
    REQUIRE(iso_dt == "2025-02-06");

    auto label = datetimelib::ts_to_local_isodate(tsz, "%R%p");
    std::transform(label.end() - 2, label.end(), label.end() - 2, ::tolower);
    spdlog::info("ts: {}, hh:mm am/pm: {}", tsz, label);
    INFO("shoud by 16:40pm");
    REQUIRE(label == "16:40pm");
}
