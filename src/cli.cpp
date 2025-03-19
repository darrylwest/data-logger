//
// 2024-12-09 10:28:16 dpw
//

#include <spdlog/spdlog.h>

#include <app/cli.hpp>
#include <app/jsonkeys.hpp>
#include <app/version.hpp>
#include <nlohmann/json.hpp>
#include <vendor/cxxopts.hpp>

#include "precompiled.hpp"

namespace app::config {
    using json = nlohmann::json;

    // initialize config with json file
    WebConfig webconfig_from_json(const json& ws_cfg) {
        using namespace app::jsonkeys;

        spdlog::debug("ws_cfg: {}", ws_cfg.dump());
        return WebConfig{
            .scheme = ws_cfg[SCHEME],
            .host = ws_cfg[HOST],
            .port = ws_cfg[PORT],
            .www = ws_cfg[WWW],
            .cert_file = ws_cfg[TLS_CERT_FILE],
            .key_file = ws_cfg[TLS_KEY_FILE],
        };
    }

    /*
     * parse config and the command line;
     */
    WebConfig parse_cli(const CliParams& params) {
        auto config = params.config;
        try {
            cxxopts::Options options("DataLogger", "Log some readings.");
            // clang-format off
            options
                .add_options()
                    ("p,port", "listening port", cxxopts::value<int>())
                    ("H,host", "listening host", cxxopts::value<Str>())
                    ("w,www", "web base folder",  cxxopts::value<Str>())
                    ("C,cert", "the cert pem file", cxxopts::value<Str>())
                    ("K,key", "the key pem file", cxxopts::value<Str>())
                    ("v,verbose", "verbose")
                    ("V,version", "Show the current version and exit")
                    ("h,help", "Show this help")
                ;

            // clang-format on
            const auto version = app::Version();
            const auto result = options.parse(params.argc, params.argv);
            if (result.count("version")) {
                std::cout << "Version: " << app::Version() << std::endl;
                params.shutdown(0);
            }

            if (result.count("help")) {
                std::cout << "Version: " << version << '\n';
                std::cout << options.help() << std::endl;
                params.shutdown(0);
            }

            if (result.count("port")) {
                config.port = result["port"].as<int>();
            }

            if (result.count("host")) {
                config.host = result["host"].as<Str>();
            }

            if (result.count("www")) {
                config.www = result["www"].as<Str>();
            }

            if (result.count("cert")) {
                config.cert_file = result["cert"].as<Str>();
            }

            if (result.count("key")) {
                config.key_file = result["key"].as<Str>();
            }

            if (result.count("verbose")) {
                config.verbose = true;
            }

        } catch (const std::exception& exp) {
            spdlog::error("error parsing cli options: {}", exp.what());
            params.shutdown(1);
        }

        return config;
    }
}  // namespace app::config
