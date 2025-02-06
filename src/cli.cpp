//
// 2024-12-09 10:28:16 dpw
//

#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include <app/cli.hpp>
#include <app/version.hpp>
#include <toml.hpp>
#include <vendor/cxxopts.hpp>

namespace app {

    /*
     * parse the command line
     */
    Config parse_cli(const int argc, char** argv) {
        auto config = Config();

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
            const auto result = options.parse(argc, argv);
            if (result.count("version")) {
                fmt::println("Version: {}", app::Version().to_string());
                exit(0);
            }

            if (result.count("help")) {
                fmt::println("Version: {}", app::Version().to_string());
                fmt::println("{}", options.help());
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

            // @see https://github.com/ToruNiina/toml11 for docs
            auto data = toml::parse("config/config.toml");
            // Str name = data.at("main").at("name").as_string();
            // fmt::println("My name from config: {}", name);

        } catch (const std::exception& exp) {
            spdlog::error("error parsing cli options: {}", exp.what());
            exit(1);
        }

        return config;
    }

}  // namespace app
