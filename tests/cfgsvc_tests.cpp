//
// Created by Darryl West on 2/27/25.
//
#include <catch2/catch_all.hpp>  // For Catch2 v3

#include <app/types.hpp>
#include <app/cfgsvc.hpp>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

struct CfgSrvTestSetup {
    CfgSrvTestSetup() {
        spdlog::set_level(spdlog::level::off); // Setup: Disable logging
    }

    ~CfgSrvTestSetup() {
        spdlog::set_level(spdlog::level::off); // Teardown: Restore logging
    }
};

TEST_CASE_METHOD(CfgSrvTestSetup, "Config Service Tests", "[cfgsrv]") {
    spdlog::info("Config Service Tests");
    const Str ss = R"({"name":"config service"})";
    const auto j = nlohmann::json::parse(ss);
    spdlog::info("JSON {}", j.dump());
    REQUIRE(j["name"] == "config service");

}
