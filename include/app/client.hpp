//
//
// 2025-02-01 23:05:09 dpw
//

#pragma once

#include <nlohmann/json.hpp>
#include <app/temperature.hpp>
#include <app/types.hpp>
#include <iostream>
#include <sstream>

namespace app {
    namespace client {
        using json = nlohmann::json;

        struct ClientStatus {
            Str version;
            int timestamp;
            int started;
            Str uptime;
            int access_count;
            int errors;

            friend std::ostream& operator<<(std::ostream& os, const ClientStatus v) {
                os << "version: " << v.version << ", ts: " << v.timestamp
                   << ", started: " << v.started << ", uptime: " << v.uptime
                   << ", accessed: " << v.access_count << ", errors: " << v.errors;

                return os;
            }

            Str to_string() const {
                std::ostringstream oss;
                oss << *this;

                return oss.str();
            }
        };

        struct ClientNode {
            Str location;
            Str ip;
            int port;
            bool active;
            int last_access;
            ClientStatus status;

            friend std::ostream& operator<<(std::ostream& os, const ClientNode v) {
                os << "location: " << v.location << ", ip: " << v.ip << ", port: " << v.port
                   << ", active: " << v.active  // ? "yes" : "no"
                   << ", last_access: " << v.last_access << ", status: " << v.status;

                return os;
            }

            Str to_string() const {
                std::ostringstream oss;
                oss << *this;

                return oss.str();
            }
        };

        ClientStatus parse_status(const Str& json_text);
        ClientStatus fetch_status(ClientNode& node);
        TemperatureData fetch_temps(ClientNode& node);
        Vec<ClientNode> create_nodes();
        ClientNode parse_client_node(const json& jclient);
        ClientNode find_client_node(const json& jclients, const Str& location);

    }  // namespace client
}  // namespace app
