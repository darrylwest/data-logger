
//
// 2025-02-01 18:27:44 dpw
//

#pragma once

#include <vendor/httplib.h>

#include <app/cli.hpp>
#include <app/database.hpp>

namespace app::service {

    constexpr auto ACCESS_CONTROL_ALLOW_ORIGIN = "Access-Control-Allow-Origin";
    constexpr auto ACCESS_CONTROL_ALLOW_METHODS = "Access-Control-Allow-Methods";
    constexpr auto ACCESS_CONTROL_ALLOW_HEADERS = "Access-Control-Allow-Headeres";
    constexpr auto ACCESS_METHODS = "GET, POST, PUT, PATCH, DELETE, OPTIONS";

    // Function to set up the server and apply configurations
    bool setup_service(httplib::Server &svr, const config::WebConfig &config,
                       database::Database &db);

    // Function to run the server
    bool run_service(const config::WebConfig &config);

}  // namespace app::service
