//
// 2024-12-09 10:28:16 dpw
//

#pragma once

#include <app/types.hpp>
#include <iostream>
#include <sstream>

namespace app {
    namespace config {

        struct Config {
            Str host = "0.0.0.0";
            int port = 9999;
            Str www = "./html";
            Str cert_file = ".ssh/cert.pem";
            Str key_file = ".ssh/key.pem";
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
    }  // namespace config
}  // namespace app
