//
//
// 2025-02-01 23:05:09 dpw
//

#pragma once

#include <app/database.hpp>
#include <app/temperature.hpp>
#include <app/types.hpp>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
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
            std::map<int, Str> probes;
            ClientStatus status;

            friend std::ostream& operator<<(std::ostream& os, const ClientNode v) {
                Str active = v.active ? "true" : "false";
                os << "location: " << v.location << ", ip: " << v.ip << ", port: " << v.port
                   << ", active: " << active << ", probes: " << v.probes.size()
                   << ", last_access: " << v.last_access << ", status: " << v.status;

                return os;
            }

            Str to_string() const {
                std::ostringstream oss;
                oss << *this;

                return oss.str();
            }
        };

        // parse status as returned from a client query
        ClientStatus parse_status(const Str& json_text);

        // fetch the client status
        ClientStatus fetch_status(ClientNode& node);

        // fetch temperatures from the client's probes
        TemperatureData fetch_temps(ClientNode& node);

        // send client node reading to web server, if server is available, else return false
        bool put_temps(const Str& url, const app::database::DbKey& key,
                       const TemperatureProbe& probe);

        // create the client node list as read from config.json
        Vec<ClientNode> create_nodes();

        // parse the single client from json; return the client node
        ClientNode parse_client_node(const json& jclient);

    }  // namespace client
}  // namespace app
