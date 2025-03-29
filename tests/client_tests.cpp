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

void cleanup() {
    client::http_client_creator = [](const Str& url) {
        return HttpClient{url};
    };
}

TEST_CASE("Client Tests", "[client][fetch_status]") {
    client::http_client_creator = [](const Str& url) {
        HttpClient client(url);

        const auto resp = HttpResponse{200, helpers::mock_client_status};
        client.set_handler(resp);

        return client;
    };

    auto node = helpers::create_test_client();

    auto status = fetch_status(node);
    spdlog::info("errors: {}", status.to_string());

    REQUIRE(status.errors == 0);
    REQUIRE(status.version.starts_with("0.6."));
    REQUIRE(status.timestamp == 1738453678);
    REQUIRE(status.location == "front-porch");
    REQUIRE(status.access_count == 8247);

    cleanup();
}

TEST_CASE("Client Tests", "[client][fetch_temps]") {
    auto creator= client::http_client_creator = [](const Str& url) {
        HttpClient client(url);

        const auto resp = HttpResponse{200, helpers::mock_reading};
        client.set_handler(resp);
        spdlog::info("mock {}", url);

        return client;
    };

    // use this to point to an alternate; a mock when it's ready
    client::http_client_creator = creator;

    auto node = helpers::create_test_client();

    auto data = fetch_temps(node);
    spdlog::info("errors: {}", data.to_string());

    REQUIRE(data.probes.size() == 2);

    auto probe0 = data.probes[0];
    REQUIRE(probe0.enabled);
    REQUIRE(probe0.location == "cottage-south");
    REQUIRE(std::abs(probe0.tempC - 10.88542) < helpers::EPSILON);

    auto probe1 = data.probes[1];
    REQUIRE(probe1.enabled == false);
    REQUIRE(probe1.location == "cottage-east");

    cleanup();
}

TEST_CASE("Client Tests", "[client][fetch_temps_404]") {
    auto creator= client::http_client_creator = [](const Str& url) {
        HttpClient client(url);

        const auto resp = HttpResponse{404, "not found"};
        client.set_handler(resp);
        spdlog::info("mock {}", url);

        return client;
    };

    INFO("this should throw an exception on mock 404");

    try {
        client::http_client_creator = creator;
        auto node = helpers::create_test_client();
        auto data = fetch_temps(node);

        REQUIRE(false);
    } catch (const std::exception& e) {
        spdlog::info("errors: {}", e.what());
        REQUIRE(true);
    }

    cleanup();
}

TEST_CASE("Client Tests", "[client][put_temps][bad_host]") {
    auto node = helpers::create_test_client();
    auto data = temperature::parse_reading(helpers::mock_reading);
    auto key = database::create_key(1739563051, "tmp.0");
    spdlog::info("temps: {}", data.to_string());

    // r.equals(data.probes.size() > 0, "probe count");
    auto probe = data.probes.at(0);
    bool ok = client::put_temps("http://badhost:9444", key, probe);

    REQUIRE(!ok);
}

TEST_CASE("Client Tests", "[client][put_temps][mock]") {
    // auto url = "http://badhost:9090";
    auto node = helpers::create_test_client();
    auto data = temperature::parse_reading(helpers::mock_reading);
    auto key = database::create_key(1739563051, "tmp.0");
    spdlog::info("temps: {}", data.to_string());

    auto creator= client::http_client_creator = [](const Str& url) {
        HttpClient client(url);

        const auto resp = HttpResponse{200, "ok"};
        client.set_handler(resp);
        spdlog::info("mock {}", url);

        return client;
    };

    // use this to point to an alternate; a mock when it's ready
    client::http_client_creator = creator;

    auto probe = data.probes.at(0);
    bool ok = client::put_temps("http://ok.host:8080", key, probe);

    REQUIRE(ok);

    cleanup();
}

TEST_CASE("Client Tests", "[client][parse_status]") {
    const Str json = helpers::mock_client_status;
    const auto status = client::parse_status(json);

    REQUIRE(status.version == "0.6.28-139");
    REQUIRE(status.timestamp == 1738453678);
    REQUIRE(status.started == 1738012925);
    REQUIRE(status.uptime == "5 days, 02:25:53");
    REQUIRE(status.access_count == 8247);
    REQUIRE(status.errors == 0);
}

TEST_CASE("Client Tests", "[client][create_nodes]") {
    const auto nodes = client::create_nodes();
    REQUIRE(nodes.size() == 3);

    for (const auto& node : nodes) {
        spdlog::info("node: {}", node.to_string());
        REQUIRE(node.port == 2030);
        REQUIRE(node.ip.starts_with("10.0.1."));
        REQUIRE(node.probes.size() == 2);
        REQUIRE(node.last_access == 0);
        REQUIRE(node.probes.size() == 2);
        // REQUIRE(node.active == false); // sometimes active, other times not

        const auto status = node.status;
        REQUIRE(status.version == "");
        REQUIRE(status.timestamp == 0);
        REQUIRE(status.uptime == "");
        REQUIRE(status.access_count == 0);
        REQUIRE(status.errors == 0);
    }
}
