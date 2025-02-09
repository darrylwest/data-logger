//
// 2024-12-09 10:28:16 dpw
//

#pragma once

#include <app/types.hpp>
#include <iostream>
#include <sstream>

namespace app {
    namespace config {

        // TODO move this defaults to config.json
        struct Config {
            Str host;
            int port;
            Str www;
            Str cert_file;
            Str key_file;
            bool verbose = false;

            friend std::ostream& operator<<(std::ostream& os, const Config v) {
                // better to use <format> but it breaks on linux and fmt broken on darwin
                os << "host: " << v.host << ", "
                   << "port: " << v.port << ", "
                   << "www : " << v.www << ", "
                   << "cert: " << v.cert_file << ", "
                   << "key : " << v.key_file << ", "
                   << "verbose: " << v.verbose << ".";
                return os;
            }

            Str to_string() const {
                std::ostringstream oss;
                oss << *this;

                return oss.str();
            }
        };

        // parse the CLI and return web config
        Config parse_cli(const int argc, char* argv[]);

        // parse and return the config file from filename
        auto parse_config(const Str filename);

        // locate and return the config filename
        Str find_config_filename();

        // use config.json to set webservice defaults
        Config init_webservice_defaults(const auto& json_cfg);
    }  // namespace config
}  // namespace app
