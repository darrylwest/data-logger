//
// 2024-12-10 15:45:20 dpw
//

#pragma once

#include <httplib.h>
#include <spdlog/spdlog.h>

#include <iostream>

namespace app {

    void show_headers(const httplib::Headers &headers);

    void log_request(const httplib::Request &req, const httplib::Response &res);

}  // namespace app
