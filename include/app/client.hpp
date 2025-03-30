//
//
// 2025-02-01 23:05:09 dpw
//

#pragma once

#include <app/database.hpp>
#include <app/http_client.hpp>
#include <app/temperature.hpp>
#include <app/types.hpp>
#include <nlohmann/json.hpp>

#include "precompiled.hpp"

namespace app::client {
    using json = nlohmann::json;
    using namespace soxlib;

    inline Func<HttpClient(const Str&)> http_client_creator = [](const Str& url) {
        HttpClient client(url);

        return client;
    };

    struct ClientStatus {
        Str version;
        int timestamp = 0;
        Str location;
        int started = 0;
        Str uptime;
        int access_count = 0;
        int errors = 0;

        friend std::ostream& operator<<(std::ostream& os, const ClientStatus v) {
            os << "version: " << v.version << ", ts: " << v.timestamp
               << ", location: " << v.location << ", started: " << v.started
               << ", uptime: " << v.uptime << ", accessed: " << v.access_count
               << ", errors: " << v.errors;

            return os;
        }

        [[nodiscard]] auto to_string() const {
            std::ostringstream oss;
            oss << *this;

            return oss.str();
        }
        // to_json function
        json to_json() const {
            json j;
            j["version"] = version;
            j["timestamp"] = timestamp;
            j["location"] = location;
            j["started"] = started;
            j["uptime"] = uptime;
            j["access_count"] = access_count;
            j["errors"] = errors;

            return j;
        }
    };

    struct ClientNode {
        Str location;
        Str ip;
        int port;
        bool active;
        time_t last_access;
        std::map<int, Str> probes;
        ClientStatus status;

        friend std::ostream& operator<<(std::ostream& os, const ClientNode v) {
            Str active = v.active ? "true" : "false";
            os << "location: " << v.location << ", ip: " << v.ip << ", port: " << v.port
               << ", active: " << active << ", probes: " << v.probes.size()
               << ", last_access: " << v.last_access << ", status: " << v.status;

            return os;
        }

        [[nodiscard]] auto to_string() const {
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
    temperature::TemperatureData fetch_temps(ClientNode& node);

    // send client node reading to web server, if server is available, else return false
    bool put_temps(const Str& url, const database::DbKey& key, const temperature::Probe& probe);

    // create the client node list as read from config.json
    Vec<ClientNode> create_nodes();

    // parse the single client from json; return the client node
    ClientNode parse_client_node(const json& jclient);

}  // namespace app::client
