//
// 2024-12-09 10:28:16 dpw
//

#pragma once

#include <app/types.hpp>
#include <nlohmann/json.hpp>

#include "precompiled.hpp"

namespace app::config {
    using json = nlohmann::json;

    // TODO move this defaults to config.json
    struct WebConfig {
        Str scheme;
        Str host;
        int port;
        Str www;
        Str cert_file;
        Str key_file;
        bool verbose = false;

        friend std::ostream& operator<<(std::ostream& os, const WebConfig& v) {
            // better to use <format> but it breaks on linux and fmt broken on darwin
            os << "sheme: " << v.scheme << ", "
               << "host: " << v.host << ", "
               << "port: " << v.port << ", "
               << "www : " << v.www << ", "
               << "cert: " << v.cert_file << ", "
               << "key : " << v.key_file << ", "
               << "verbose: " << v.verbose << ".";
            return os;
        }

        [[nodiscard]] auto to_string() const {
            std::ostringstream oss;
            oss << *this;

            return oss.str();
        }
    };

    struct CliParams {
        int argc;
        char** argv;
        WebConfig config;
        Func<void(const int code)> shutdown;
    };

    // parse the CLI and return web config
    WebConfig parse_cli(const CliParams& params);

    // use config.json to set webservice defaults
    WebConfig webconfig_from_json(const json& wscfg);
}  // namespace app::config
