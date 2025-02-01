//
//
// 2025-02-01 23:05:09 dpw
//

#pragma once

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

#include <iostream>
#include <sstream>

namespace app {
    struct ClientStatus {
        std::string version;
        int timestamp;
        int started;
        std::string uptime;
        int access_count;
        int errors;

        friend std::ostream& operator<<(std::ostream& os, const ClientStatus v) {
            os << "version: " << v.version
               << ", ts: " << v.timestamp
               << ", started: " << v.started 
               << ", uptime: " << v.uptime
               << ", accessed: " << v.access_count
               << ", errors: " << v.errors;

            return os;
        }

        std::string to_string() const {
            std::ostringstream oss;
            oss << *this;

            return oss.str();
        }
    }

    struct ClientNode {
        std::string location;
        std::string ip;
        int port;
        bool active;
        int last_access;
        ClientStatus status;

        friend std::ostream& operator<<(std::ostream& os, const TemperatureData v) {
            os << "location: " << v.location
               << ", ip: " << v.ip
               << ", port: " << v.port
               << ", active: " << v.active ? "yes" : "no"
               << ", last_access: " << v.last_access
               << ", status: " << v.status
           ;

            return os;
        }

        std::string to_string() const {
            std::ostringstream oss;
            oss << *this;

            return oss.str();
        }
    };

    ClientStatus parse_status(const std::string& json_text);
    ClientStatus fetch_status(ClientNode& node);
    TemperatureData fetch_temps(ClientNode& node);

}  // namespace app


