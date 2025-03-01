
#pragma once

#include <app/types.hpp>
#include <app/cli.hpp>

namespace helpers {
    const MainTestSetup setup = MainTestSetup();

    std::string capture_stdout(Func<void()> func) {
        std::ostringstream oss;
        std::streambuf* old_cout = std::cout.rdbuf(oss.rdbuf());  // Redirect std::cout

        func();  // Execute function that prints to stdout

        std::cout.rdbuf(old_cout);  // Restore std::cout
        return oss.str();
    }

    app::config::WebConfig default_web_config = {
        .scheme = "http",
        .host = "0.0.0.0",
        .port = 9090,
        .www = "html",
        .cert_file = "cert.pem",
        .key_file = "cert.pem",
        .verbose = false,
    };

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
