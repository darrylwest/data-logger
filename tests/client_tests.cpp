//
// Created by Darryl West on 2/27/25.
//
#include <app/client.hpp>
#include <app/types.hpp>
#include <catch2/catch_all.hpp>  // For Catch2 v3

#include "test_helpers.hpp"
#include "mock_http_client.hpp"

using namespace app;

struct ClientTestSetup {
    ClientTestSetup() {
        spdlog::set_level(spdlog::level::info);
    }

    ~ClientTestSetup() {
        spdlog::set_level(spdlog::level::off);
    }
};

const app::client::ClientNode create_test_client() {
    const Str json_text
        = R"({"status":{"version":"test","ts":1738453678,"started":1738012925,"uptime":"0 days, 00:00:00","access":0,"errors":0}})";

    app::client::ClientStatus status = app::client::parse_status(json_text);
    const auto node = app::client::ClientNode{
        .location = "test",
        .ip = "10.0.1.115",
        .port = 2030,
        .active = true,
        .last_access = 0,
        .probes = {},
        .status = status,
    };

    return node;
}


TEST_CASE_METHOD(ClientTestSetup, "Client Tests", "[client][fetch_status]") {
    // TODO create mock client node to test fetch_temps, put_temps, fetch_status
    client::http_client_creator = [](const Str& url) -> httplib::Client {
        httplib::Client client(url);

        return client;
    };

    using namespace app::client;
    auto node = create_test_client();

    auto status = app::client::fetch_status(node);
    spdlog::info("errors: {}", status.to_string());

    REQUIRE(true);
}

TEST_CASE_METHOD(ClientTestSetup, "Client Tests", "[client][fetch_temps]") {
    // TODO create mock client node to test fetch_temps, put_temps, fetch_status
    REQUIRE(true);
}

TEST_CASE_METHOD(ClientTestSetup, "Client Tests", "[client][put_temps]") {
    // TODO create mock client node to test fetch_temps, put_temps, fetch_status
    REQUIRE(true);
}

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
