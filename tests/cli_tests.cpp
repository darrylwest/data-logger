//
// Created by Darryl West on 2/27/25.
//
#include <catch2/catch_all.hpp>  // For Catch2 v3
#include <app/types.hpp>
#include <spdlog/spdlog.h>
#include <app/cli.hpp>

struct CLITestSetup {
    using json = nlohmann::json;
    CLITestSetup() {
        spdlog::set_level(spdlog::level::info); // Setup: Disable logging
    }

    ~CLITestSetup() {
        spdlog::set_level(spdlog::level::off); // Teardown: Restore logging
    }
};

TEST_CASE_METHOD(CLITestSetup, "CLI::webconfig_from_jsonTests", "[cli][parse_json]") {
    // read in the config file
    using namespace app::jsonkeys;

    std::ifstream ifs("config/config.json");
    const json app_config = json::parse(ifs);
    const json cfg = app_config[WEBSERVICE];

    spdlog::info("parse web json tests");
    // where's the json?
    REQUIRE(cfg.contains(HOST));
    REQUIRE(cfg.contains(SCHEME));
    REQUIRE(cfg.contains(PORT));
    REQUIRE(cfg.contains(WWW));
    REQUIRE(cfg.contains(TLS_CERT_FILE));
    REQUIRE(cfg.contains(TLS_KEY_FILE));

    const auto webconfig = app::config::webconfig_from_json(cfg);
    REQUIRE(webconfig.host == cfg[HOST]);
    REQUIRE(webconfig.scheme== cfg[SCHEME]);
    REQUIRE(webconfig.port == cfg[PORT]);
    REQUIRE(webconfig.www == cfg[WWW]);
    REQUIRE(webconfig.verbose == false);
}
TEST_CASE_METHOD(CLITestSetup, "CLI::parse_cli", "[cli][parse_cli]") {
    spdlog::info("parse_cli tests");
}
