//
// Created by Darryl West on 2/27/25.
//
#include <catch2/catch_all.hpp>  // For Catch2 v3
#include <app/types.hpp>
#include <datetimelib/datetimelib.hpp>

TEST_CASE("DateTimeLib Tests", "[datetimelib]") {
    const Str ss = "test string";
    REQUIRE(ss == "test string");

    auto unix_ts = datetimelib::timestamp_seconds();
    std::time_t tsz = 1738888855;
    INFO("tsz is in the past, 2025-02-06");
    REQUIRE(tsz < unix_ts);
}
