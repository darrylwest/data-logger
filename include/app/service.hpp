
//
// 2025-02-01 18:27:44 dpw
//

#pragma once

#include <vendor/httplib.h>

#include <app/cli.hpp>
#include <app/database.hpp>
#include <app/types.hpp>

namespace app {

    // Function to set up the server and apply configurations
    bool setup_service(httplib::Server &svr, const config::WebConfig &config, database::Database &db);

    // Function to run the server
    bool run_service(const config::WebConfig &config);

}  // namespace app
