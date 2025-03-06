//
// Created by Darryl West on 2/27/25.
//
#include <catch2/catch_all.hpp>  // For Catch2 v3
#include <vector>
#include <app/types.hpp>
#include <spdlog/spdlog.h>
#include <app/cli.hpp>
#include <app/jsonkeys.hpp>
#include "test_helpers.hpp"

struct CLITestSetup {
    CLITestSetup() {
        spdlog::set_level(spdlog::level::off); // Setup: Disable logging
    }

    ~CLITestSetup() {
        spdlog::set_level(spdlog::level::off); // Teardown: Restore logging
    }
};

app::config::WebConfig call_parse_cli(Vec<Str> args, Func<void(int)> shutdown = [](int) {}) {
    Vec<char*> argv;
    argv.push_back(const_cast<char*>("test_binary")); // argv[0] is usually the program name

    for (auto& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    argv.push_back(nullptr); // Null-terminate the argument list

    const auto params = app::config::CliParams {
        .argc = static_cast<int>(argv.size() - 1),
        .argv = argv.data(),
        .config = helpers::default_web_config,
        .shutdown = std::move(shutdown),
    };

    return app::config::parse_cli(params);
}

// TODO modify this to pull Rstring from helper
TEST_CASE_METHOD(CLITestSetup, "CLI::json format", "[cli][parse_json]") {
    using namespace app::jsonkeys;
    using json = nlohmann::json;

    const auto text = helpers::full_config_json_text;
    const json app_config = json::parse(text);
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

TEST_CASE_METHOD(CLITestSetup, "parse_cli handles version flag", "[parse_cli][help]") {

    const auto output = helpers::capture_stdout([]() {
        const Func<void(int code)>shutdown = [](int code) {
            INFO("return code should be zero");
            REQUIRE(code == 0);
        };

        const app::config::WebConfig config = call_parse_cli({"--help"}, shutdown);

        REQUIRE(config.verbose == false);  // Ensure WebConfig has a `verbose` flag

    });

    INFO(output);
    REQUIRE(output.find("this help") != Str::npos);
}

TEST_CASE_METHOD(CLITestSetup, "parse_cli handles help flag", "[parse_cli][version]") {

    const auto output = helpers::capture_stdout([]() {
        const Func<void(int code)>shutdown = [](int code) {
            INFO("return code should be zero");
            REQUIRE(code == 0);
        };

        const app::config::WebConfig config = call_parse_cli({"--version"}, shutdown);

        REQUIRE(config.verbose == false);  // Ensure WebConfig has a `verbose` flag
    });

    INFO(output);
    REQUIRE(output.find("Version:") != Str::npos);
}

TEST_CASE_METHOD(CLITestSetup, "parse_cli with bad flag", "[parse_cli][bad]") {
    const auto output = helpers::capture_stdout([]() {
        const Func<void(int code)>shutdown = [](int code) {
            INFO("return code should be zero");
            REQUIRE(code == 1);
        };

        const app::config::WebConfig config = call_parse_cli({"--bad"}, shutdown);

        REQUIRE(config.verbose == false);  // Ensure WebConfig has a `verbose` flag
    });

    INFO(output);
    REQUIRE(output.empty());
}

