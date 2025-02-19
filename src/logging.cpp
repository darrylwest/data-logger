//
// 2024-12-10 15:45:20 dpw
// 
// NOTE: This logging config is for httplib
//

#include <httplib.h>
#include <spdlog/spdlog.h>

#include <app/logging.hpp>
#include <iostream>

namespace app {

    // TODO : set_level

    void show_headers(const httplib::Headers &headers) {
        for (const auto &x : headers) {
            spdlog::debug("{}:{}", x.first.c_str(), x.second.c_str());
        }
    }

    void log_request(const httplib::Request &req, const httplib::Response &res) {
        spdlog::info("{} {} {}", req.method.c_str(), req.version.c_str(), req.path.c_str());

        for (auto it = req.params.begin(); it != req.params.end(); ++it) {
            const auto &x = *it;
            spdlog::info("{}={}", x.first.c_str(), x.second.c_str());
        }

        show_headers(req.headers);

        if (res.status > 299) {
            spdlog::error("Response status: {}", res.status);
        } else {
            spdlog::info("Response status: {}", res.status);
        }

        show_headers(res.headers);
    }

}  // namespace app
