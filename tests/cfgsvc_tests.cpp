//
// Created by Darryl West on 2/27/25.
//
#include <catch2/catch_all.hpp>  // For Catch2 v3

#include <app/types.hpp>
#include <app/cfgsvc.hpp>
#include <spdlog/spdlog.h>
#include "test_helpers.hpp"
#include <nlohmann/json.hpp>

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
    auto const client_cfg = cfgsvc::client("front-porch");

    spdlog::debug("{}", client_cfg.dump(4));

    REQUIRE(client_cfg.contains("active"));
    REQUIRE(client_cfg.contains("ip"));
    REQUIRE(client_cfg.contains("port"));
    REQUIRE(client_cfg.contains("sensors"));

}

TEST_CASE("Config Service Tests", "[cfgsrv][clients]") {
    spdlog::info("Config Service Tests: clients");

    using namespace app;
    auto const clients = cfgsvc::clients();

    spdlog::debug("{}", clients.dump(4));

    REQUIRE(clients.size() == 3);
    for (const auto& client : clients) {
        REQUIRE(client.contains("active"));
        REQUIRE(client.contains("ip"));
        REQUIRE(client.contains("port"));
        REQUIRE(client.contains("sensors"));

        REQUIRE(client[jsonkeys::PORT] == 2030);
    }
}
