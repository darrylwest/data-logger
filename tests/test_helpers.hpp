
#pragma once

#include <app/types.hpp>
#include <app/cli.hpp>

namespace helpers {
    constexpr double EPSILON = 1e-5;

    inline std::string capture_stdout(Func<void()> func) {
        std::ostringstream oss;
        std::streambuf* old_cout = std::cout.rdbuf(oss.rdbuf());  // Redirect std::cout

        func();  // Execute function that prints to stdout

        std::cout.rdbuf(old_cout);  // Restore std::cout
        return oss.str();
    }

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
    })";

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
