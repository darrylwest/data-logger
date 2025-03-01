//
// Created by Darryl West on 2/27/25.
//
#include <catch2/catch_all.hpp>  // For Catch2 v3
#include <vector>
#include <app/types.hpp>
#include <spdlog/spdlog.h>
#include <app/cli.hpp>

struct CLITestSetup {
    CLITestSetup() {
        spdlog::set_level(spdlog::level::off); // Setup: Disable logging
    }

    ~CLITestSetup() {
        spdlog::set_level(spdlog::level::off); // Teardown: Restore logging
    }
};

app::config::WebConfig default_config = {
    .scheme = "http",
    .host = "0.0.0.0",
    .port = 9090,
    .www = "html",
    .cert_file = "cert.pem",
    .key_file = "cert.pem",
    .verbose = false,
};

app::config::WebConfig call_parse_cli(Vec<Str> args) {
    Vec<char*> argv;
    argv.push_back(const_cast<char*>("test_binary")); // argv[0] is usually the program name

    for (auto& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    argv.push_back(nullptr); // Null-terminate the argument list

    const auto params = app::config::CliParams {
        .argc = static_cast<int>(argv.size() - 1),
        .argv = argv.data(),
        .config = default_config,
        .shutdown = [](int) { }
    };

    return app::config::parse_cli(params);
}

TEST_CASE_METHOD(CLITestSetup, "CLI::json format", "[cli][parse_json]") {
    using namespace app::jsonkeys;
    using json = nlohmann::json;

    std::ifstream ifs("config/config.json");
    const json app_config = json::parse(ifs);
    const json cfg = app_config[WEBSERVICE];

    spdlog::info("check web json keys from {}", cfg.dump());

    REQUIRE(cfg.contains(HOST));
    REQUIRE(cfg.contains(SCHEME));
    REQUIRE(cfg.contains(PORT));
    REQUIRE(cfg.contains(WWW));
    REQUIRE(cfg.contains(TLS_CERT_FILE));
    REQUIRE(cfg.contains(TLS_KEY_FILE));
}
TEST_CASE_METHOD(CLITestSetup, "CLI::webconfig_from_jsonTests", "[cli][parse_json]") {
    using json = nlohmann::json;

    const Str jtext = R"({"headers":[],
        "host":"0.1.2.3",
        "port":8080,
        "scheme":"https",
        "tls_cert_file":".ssh/cert.pem",
        "tls_key_file":".ssh/key.pem",
        "www":"html"}
    )";

    spdlog::info("parse web json tests from {}", jtext);

    const json j = json::parse(jtext);

    const auto config = app::config::webconfig_from_json(j);
    REQUIRE(config.host == "0.1.2.3");
    REQUIRE(config.scheme== "https");
    REQUIRE(config.port == 8080);
    REQUIRE(config.www == "html");
    REQUIRE(config.verbose == false);
}

TEST_CASE_METHOD(CLITestSetup, "parses basic arguments correctly", "[cli][parse_cli]") {
    spdlog::info("parse_cli with port host www");
    const app::config::WebConfig config = call_parse_cli({"--port", "8080", "--host", "127.0.0.1", "--www", "/var/www"});

    REQUIRE(config.port == 8080);
    REQUIRE(config.host == "127.0.0.1");
    REQUIRE(config.www == "/var/www");
}

TEST_CASE_METHOD(CLITestSetup, "parse_cli parses SSL certificate and key", "[cli][parse_cli]") {
    spdlog::info("parse_cli with cert key");
    const app::config::WebConfig config = call_parse_cli({"--cert", "cert.pem", "--key", "key.pem"});

    REQUIRE(config.cert_file == "cert.pem");
    REQUIRE(config.key_file == "key.pem");
}

TEST_CASE_METHOD(CLITestSetup, "parse_cli handles missing optional values", "[cli][parse_cli]") {
    spdlog::info("parse_cli with no flags");
    const app::config::WebConfig config = call_parse_cli({});

    REQUIRE(config.port == 9090);  // Assuming default is 0 or some predefined value
    REQUIRE(config.scheme == "http");
    REQUIRE(config.host == "0.0.0.0");
}

TEST_CASE_METHOD(CLITestSetup, "parse_cli handles verbose flag", "[cli][parse_cli]") {
    const app::config::WebConfig config = call_parse_cli({"--verbose"});

    REQUIRE(config.verbose == true);  // Ensure WebConfig has a `verbose` flag
}

// TEST_CASE_METHOD(CLITestSetup, "parse_cli handles version flag", "[cli]") {
//     REQUIRE_THROWS_WITH(call_parse_cli({"--version"}), Catch::Matchers::Contains("Version:"));
// }
//
// TEST_CASE_METHOD("parse_cli handles help flag", "[cli]") {
//     REQUIRE_THROWS_WITH(call_parse_cli({"--help"}), Catch::Matchers::Contains("Usage:"));
// }

