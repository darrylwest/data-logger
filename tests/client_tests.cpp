//
// Created by Darryl West on 2/27/25.
//
#include <app/client.hpp>
#include <app/types.hpp>
#include <catch2/catch_all.hpp>  // For Catch2 v3

#include "test_helpers.hpp"

using namespace app;

struct ClientTestSetup {
    ClientTestSetup() {
        spdlog::set_level(spdlog::level::err);
    }

    ~ClientTestSetup() {
        spdlog::set_level(spdlog::level::off);
    }
};

// TODO create mock client node to test fetch_temps, put_temps, fetch_status

TEST_CASE_METHOD(ClientTestSetup, "Client Tests", "[client][parse_status]") {
    const Str json = helpers::mock_client_status;
    const auto status = client::parse_status(json);

    REQUIRE(status.version == "0.6.28-139");
    REQUIRE(status.timestamp == 1738453678);
    REQUIRE(status.started == 1738012925);
    REQUIRE(status.uptime == "5 days, 02:25:53");
    REQUIRE(status.access_count == 8247);
    REQUIRE(status.errors == 0);
}

TEST_CASE_METHOD(ClientTestSetup, "Client Tests", "[client][create_nodes]") {
    const auto nodes = client::create_nodes();
    REQUIRE(nodes.size() == 3);

    for (const auto& node : nodes) {
        spdlog::info("node: {}", node.to_string());
        REQUIRE(node.port == 2030);
        REQUIRE(node.ip.starts_with("10.0.1."));
        REQUIRE(node.probes.size() == 2);
        REQUIRE(node.last_access == 0);
        REQUIRE(node.probes.size() == 2);
        REQUIRE(node.active == true);

        const auto status = node.status;
        REQUIRE(status.version == "");
        REQUIRE(status.timestamp == 0);
        REQUIRE(status.uptime == "");
        REQUIRE(status.access_count == 0);
        REQUIRE(status.errors == 0);
    }
}
