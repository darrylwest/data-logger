//
// Created by Darryl West on 2/27/25.
//
#include <catch2/catch_all.hpp>  // For Catch2 v3

#include <app/types.hpp>
#include <app/cfgsvc.hpp>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

TEST_CASE("Config Service Tests", "[cfgsv]") {
    spdlog::info("Config Service Tests");
    const Str ss = R"({"name":"config service"})";
    const auto j = nlohmann::json::parse(ss);
    spdlog::info("JSON {}", j.dump(4));
    REQUIRE(j["name"] == "config service");

}
