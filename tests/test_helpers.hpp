//
// dpw
//
#pragma once

#include <app/types.hpp>
#include <app/cli.hpp>
#include <spdlog/spdlog.h>
#include "precompiled.hpp"

namespace helpers {
    constexpr double EPSILON = 1e-5;

    inline std::random_device random_device;  // Obtain a random number from hardware
    inline std::mt19937 generator(random_device());

    inline auto random_int(int min = 1'000'000, int max = 9'999'999) {
        std::uniform_int_distribution<int> distr(min, max);
        return distr(generator);
    }

    inline float random_float(float min = -20.0, float max = 100.0) {
        std::uniform_real_distribution<float> distr(min, max);
        return distr(generator);
    }


    // create a random filename
    inline auto create_temp_path(const Str& name = "tmp-file_") {
        // Get the system's temporary directory
        std::filesystem::path tempDir = std::filesystem::temp_directory_path();

        // Generate a unique filename
        Str filename = name + std::to_string(random_int()) + ".tmp"; // Simple unique filename

        // Create a temporary file path
        FilePath path = tempDir / filename;

        // Create the temporary file
        std::ofstream ofs(path);
        if (!ofs) {
            throw std::runtime_error("Failed to create temporary file");
        }

        // Close the file (optional, as it will be closed when the ofstream goes out of scope)
        ofs.close();

        return path;
    }

    inline auto remove_temp_path(const FilePath& path) {
        try {
            std::filesystem::remove_all(path);
        } catch (const std::exception& e) {
            spdlog::error("Failed to remove temporary file {}", e.what());
        }
    }


    inline std::string capture_stdout(const Func<void()>& func) {
        std::ostringstream oss;
        std::streambuf* old_cout = std::cout.rdbuf(oss.rdbuf());  // Redirect std::cout

        func();  // Execute function that prints to stdout

        std::cout.rdbuf(old_cout);  // Restore std::cout
        return oss.str();
    }

    // sample data
    constexpr StrView raw_temps_data = "1740422400.cottage.0=15.343750\n1740422700.cottage.0=15.458330\n1740423000.cottage.0=15.651040\n1740423300.cottage.0=15.739580\n1740423600.cottage.0=16.010420\n1740423900.cottage.0=16.192711\n1740424200.cottage.0=16.244789\n1740424500.cottage.0=16.062500\n1740424800.cottage.0=16.041670\n1740425100.cottage.0=15.833330\n1740425400.cottage.0=15.812500\n1740425700.cottage.0=16.234381";

    inline app::config::WebConfig default_web_config = {
        .scheme = "http",
        .host = "0.0.0.0",
        .port = 9090,
        .www = "html",
        .cert_file = "cert.pem",
        .key_file = "cert.pem",
        .verbose = false,
    };

    const Str mock_reading = R"(
        {"reading_at": 1738362466,
        "probes":[
            {"sensor":0,"location":"cottage-south","enabled":true,"millis":349548023,"tempC":10.88542,"tempF":51.59375},
            {"sensor":1,"location":"cottage-east","enabled":false,"millis":349548023,"tempC":-127.0,"tempF":51.66576}
        ]
    })";

    const Str mock_client_config = R"({
        "location": "cottage",
        "ip": "10.0.1.198",
        "port": 2031,
        "active": true,
        "sensors": [
            {
                "type": "temperature",
                "path": "/temps",
                "probes": [
                    { "sensor": 0, "location": "cottage-south", "enabled": true },
                    { "sensor": 1, "location": "cottage-east", "enabled": false}
                ]
            }
        ]
    })";

    const Str mock_client_status = R"(
        {"status":
            {"version":"0.6.28-139",
            "ts":1738453678,
            "location": "front-porch",
            "started":1738012925,
            "uptime":"5 days, 02:25:53",
            "access":8247,
            "errors":0}
        }
    )";

    inline app::client::ClientNode create_test_client() {
        app::client::ClientStatus status = app::client::parse_status(mock_client_status);
        const auto node = app::client::ClientNode{
            .location = "test",
            .ip = "10.0.1.115",
            .port = 2090,
            .active = true,
            .last_access = 0,
            .probes = {},
            .status = status,
        };

        return node;
    }

    const Str full_config_json_text = R"(
    {
        "config-version": "0.6.1-106",
        "home": "{{HOME}}/.data-logger",
        "logging": {
            "level": "info",
            "folder": "logs",
            "file": "none"
        },
        "data": {
            "folder": "data",
            "temperature": "/temperature",
            "status": "/status"
        },
        "webservice": {
            "scheme": "http",
            "host": "0.0.0.0",
            "port": 9090,
            "www": "html",
            "tls_cert_file": ".ssh/cert.pem",
            "tls_key_file": ".ssh/key.pem",
            "headers": [ ]
        },
        "clients": [
            {
                "location": "cottage",
                "ip": "10.0.1.197",
                "port": 2030,
                "active": true,
                "sensors": [
                    {
                        "type": "temperature",
                        "path": "/temps",
                        "probes": [
                            { "sensor": 0, "location": "cottage-south", "enabled": true },
                            { "sensor": 1, "location": "cottage-east", "enabled": false}
                        ]
                    }
                ]
            },
            {
                "location": "deck",
                "ip": "10.0.1.177",
                "port": 2030,
                "active": true,
                "sensors": [
                    {
                        "type": "temperature",
                        "path": "/temps",
                        "probes": [
                            { "sensor": 0, "location": "deck-west", "enabled": true },
                            { "sensor": 1, "location": "deck-east", "enabled": false }
                        ]
                    }
                ]
            },
            {
                "location": "shed",
                "ip": "10.0.1.115",
                "port": 2030,
                "active": true,
                "sensors": [
                    {
                        "type": "temperature",
                        "path": "/temps",
                        "probes": [
                            { "sensor": 0, "location": "shed-inside", "enabled": true },
                            { "sensor": 1, "location": "shed-west", "enabled": false }
                        ]
                    }
                ]
            }
        ]
    })";

}
