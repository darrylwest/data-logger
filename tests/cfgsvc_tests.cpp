//
// Created by Darryl West on 2/27/25.
//
#include <catch2/catch_all.hpp>  // For Catch2 v3

#include <app/types.hpp>
#include <app/cfgsvc.hpp>
#include <spdlog/spdlog.h>
#include "test_helpers.hpp"
#include <nlohmann/json.hpp>
#include <app/jsonkeys.hpp>

TEST_CASE("Config Service Tests", "[cfgsrv][main]") {
    spdlog::info("Config Service Tests: version");

    using namespace app;
    auto const jvers = cfgsvc::get_node(jsonkeys::CONFIG_VERSION);

    spdlog::info("{}", jvers.dump(4));
    const Str vers = jvers.get<Str>();

    INFO(vers);
    REQUIRE(vers.starts_with("0.6."));
}

TEST_CASE("Config Service Tests", "[cfgsrv][client]") {
    spdlog::info("Config Service Tests: client");

    using namespace app;
    using namespace jsonkeys;
    auto const client_cfg = cfgsvc::client("front-porch");

    spdlog::debug("{}", client_cfg.dump(4));

    REQUIRE(client_cfg.contains(ACTIVE));
    REQUIRE(client_cfg.contains(IP));
    REQUIRE(client_cfg.contains(PORT));
    REQUIRE(client_cfg.contains(SENSORS));

}

TEST_CASE("Config Service Tests", "[cfgsrv][clients]") {
    spdlog::info("Config Service Tests: clients");

    using namespace app;
    using namespace jsonkeys;
    auto const clients = cfgsvc::clients();

    spdlog::debug("{}", clients.dump(4));

    REQUIRE(clients.size() == 3);
    for (const auto& client : clients) {
        REQUIRE(client.contains(ACTIVE));
        REQUIRE(client.contains(IP));
        REQUIRE(client.contains(PORT));
        REQUIRE(client.contains(SENSORS));

        REQUIRE(client[jsonkeys::PORT] == 2030);
    }
}

TEST_CASE("Config Service Tests", "[cfgsvs][data-node]") {
    spdlog::set_level(spdlog::level::critical);
    spdlog::info("Config Service Tests: data");

    using namespace app;
    using namespace jsonkeys;

    auto const jdata = cfgsvc::data_node();

    REQUIRE(jdata.contains(FOLDER));
    REQUIRE(jdata.contains(TEMPERATURE));
    REQUIRE(jdata.contains( CLIENT_STATUS));
    REQUIRE(jdata[FOLDER] == "data");
    REQUIRE(jdata[TEMPERATURE] == "/current.temps.db");
    REQUIRE(jdata[CLIENT_STATUS] == "/current.status.db");

    spdlog::set_level(spdlog::level::critical);
}
