//
// 2024-12-09 10:28:16 dpw
//

#include <spdlog/spdlog.h>

#include <app/cli.hpp>
#include <app/jsonkeys.hpp>
#include <app/version.hpp>
#include <fstream>
#include <nlohmann/json.hpp>
#include <vendor/cxxopts.hpp>

namespace app {
    namespace config {
        using json = nlohmann::json;

        // locate the config file and return the name
        Str find_config_filename() { return "./config/config.json"; }

        // parse and return the config file
        auto parse_config() {
            const auto filename = find_config_filename();
            spdlog::info("reading/parsing config from file: {}", filename);
            std::ifstream fin(filename);
            return json::parse(fin);
        }

        // initialize config with json file
        Config webservice_from_json(const auto& jcfg) {
            using namespace app::jsonkeys;

            spdlog::info("cfg: {}", jcfg.dump());
            return Config{
                .scheme = jcfg[SCHEME],
                .host = jcfg[HOST],
                .port = jcfg[PORT],
                .www = jcfg[WWW],
                .cert_file = jcfg[TLS_CERT_FILE],
                .key_file = jcfg[TLS_KEY_FILE],
            };
        }

        /*
         * parse the command line
         */
        Config parse_cli(const int argc, char** argv) {
            // parse config/config.json and set defaults
            auto cfg = parse_config();
            auto config = webservice_from_json(cfg[jsonkeys::WEBSERVICE]);

            try {
                // first, read the standard config file

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
                const auto result = options.parse(argc, argv);
                if (result.count("version")) {
                    std::cout << "Version: " << app::Version() << std::endl;
                    exit(0);
                }

                if (result.count("help")) {
                    std::cout << "Version: " << version << std::endl;
                    std::cout << options.help() << std::endl;
                    exit(0);
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

            } catch (const std::exception& exp) {
                spdlog::error("error parsing cli options: {}", exp.what());
                exit(1);
            }

            return config;
        }

    }  // namespace config
}  // namespace app
