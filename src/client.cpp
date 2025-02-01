//
// 2025-02-01 dpw
//

#include <httplib.h>
#include <spdlog/spdlog.h>

#include <app/client.hpp>
#include <app/temperature.hpp>
#include <iostream>
#include <nlohmann/json.hpp>

namespace app {

    ClientStatus parse_status(const std::string& json_text) {
        ClientStatus status;

        return status;
    }

    ClientStatus fetch_status(ClientNode& node) {
        ClientStatus status;

        return status;
    }

    app::TemperatureData fetch_temps(ClientNode& node) {
        app::TemperatureData data;

        return data;
    }

}  // namespace app
