//
// 2024-12-09 10:28:16 dpw
//

#pragma once

#include <spdlog/spdlog.h>

#include <app/version.hpp>
#include <iostream>

namespace app {

    struct Config {
        std::string host = "localhost";
        int port = 9999;
        bool verbose = false;

        friend std::ostream& operator<<(std::ostream& os, const Config v) {
            // better to use <format> but it breaks on linux and fmt broken on darwin
            os << "host: " << v.host << ", "
               << "port: " << v.port << ", "
               << "verbose: " << v.verbose << ".";
            return os;
        }

        std::string to_string() const {
            std::ostringstream oss;
            oss << *this;

            return oss.str();
        }
    };

    Config parse_cli(const int argc, char* argv[]);
}  // namespace app
