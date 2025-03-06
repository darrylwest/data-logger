//
// Created by Darryl West on 2/27/25.
//
#include <app/client.hpp>
#include <app/types.hpp>
#include <catch2/catch_all.hpp>  // For Catch2 v3
#include <app/http_client.hpp>

#include "test_helpers.hpp"

using namespace app;
using namespace soxlib;

struct ClientTestSetup {
    ClientTestSetup() {
        spdlog::set_level(spdlog::level::critical);
    }

    ~ClientTestSetup() {
        spdlog::set_level(spdlog::level::off);
    }
};

const client::ClientNode create_test_client() {
    const Str json_text
        = R"({"status":{"version":"test","ts":1738453678,"started":1738012925,"uptime":"0 days, 00:00:00","access":0,"errors":0}})";

    client::ClientStatus status = client::parse_status(json_text);
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

    client::http_client_creator = [](const Str& url) {
        HttpClient client(url);
        spdlog::warn("not a mock {}", url);
        return client;
    };

    using namespace app::client;
    auto node = create_test_client();

    auto status = fetch_status(node);
    spdlog::info("errors: {}", status.to_string());

    REQUIRE(status.errors == 0);
    REQUIRE(status.version.starts_with("0.6."));
}

TEST_CASE_METHOD(ClientTestSetup, "Client Tests", "[client][fetch_temps]") {
    // TODO create mock client node to test fetch_temps, put_temps, fetch_status
    auto creator= client::http_client_creator = [](const Str& url) {
        HttpClient client(url);
        spdlog::warn("not a mock {}", url);
        return client;
    };

    // use this to point to an alternate; a mock when it's ready
    app::client::http_client_creator = creator;

    using namespace app::client;
    auto node = create_test_client();

    auto data = fetch_temps(node);
    spdlog::info("errors: {}", data.to_string());

    REQUIRE(data.probes.size() == 2);

    auto probe = data.probes[0];
    REQUIRE(probe.enabled == false);
    REQUIRE(probe.location == "deck-west");
    REQUIRE(probe.tempC < -100);
}

TEST_CASE_METHOD(ClientTestSetup, "Client Tests", "[client][put_temps]") {
    // TODO create mock client node to test fetch_temps, put_temps, fetch_status
    auto creator= client::http_client_creator = [](const Str& url) {
        HttpClient client(url);
        spdlog::warn("not a mock {}", url);
        return client;
    };

    // use this to point to an alternate; a mock when it's ready
    app::client::http_client_creator = creator;

    using namespace app::client;

    auto url = "http://badhost:9090";
    auto node = create_test_client();
    auto data = app::client::fetch_temps(node);
    time_t timestamp = 1739563051;
    auto key = app::database::create_key(timestamp, "tmp.0");
    spdlog::info("temps: {}", data.to_string());

    // r.equals(data.probes.size() > 0, "probe count");
    auto probe = data.probes.at(0);
    bool ok = app::client::put_temps(url, key, probe);

    REQUIRE(!ok);
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
