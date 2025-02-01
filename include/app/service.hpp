
//
// 2025-02-01 18:27:44 dpw
//

#pragma once

#include <spdlog/spdlog.h>
#include <vendor/httplib.h>

#include <cstdio>
#include <iostream>
#include <app/cli.hpp>

namespace app {

    // Function to set up the server and apply configurations
    bool setup_service(httplib::SSLServer &svr, const Config &config);

    // Function to run the server
    bool run_service(const Config &config);

}  // namespace app
