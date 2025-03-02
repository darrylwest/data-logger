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
