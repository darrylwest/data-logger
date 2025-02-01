//
// 2025-02-01 18:27:44 dpw
//

#include <httplib.h>
#include <spdlog/spdlog.h>

#include <app/cli.hpp>
#include <app/database.hpp>
#include <app/logging.hpp>
#include <app/service.hpp>
#include <cstdio>
#include <iostream>

using namespace httplib;

namespace app {
    auto db = Database();

    // Function to set up the server and apply configurations
    bool setup_service(Server &svr, const Config &config) {
        if (svr.is_valid() == 0) {
            spdlog::error("ERROR! Server is not valid. Check the cert/key files? Exiting...");
            return false;
        }

        // Error handler
        svr.set_error_handler([](const Request &req, Response &res) {
            spdlog::error("ERROR! bad request {} {}", req.method.c_str(), req.path.c_str());
            // TODO : replace this with spdlog::fmt...
            const char *fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
            char buf[BUFSIZ];
            snprintf(buf, sizeof(buf), fmt, res.status);
            res.set_content(buf, "text/html");
        });

        // Logger
        svr.set_logger([](const Request &req, const Response &res) { app::log_request(req, res); });

        // Mount point
        if (!svr.set_mount_point("/", config.www)) {
            spdlog::error("ERROR! The specified base directory {} doesn't exist...", config.www);
            return false;
        }

        //
        // add new/custom routes here
        //

        svr.Get("/api/temps", [](const Request &, Response &res) {
            const auto temps = db.getTemps();
            // parse the interval and end_date
            // spdlog::info("info content: {}", data);

            res.set_content(temps, "application/json");
        });

        // Shutdown hook
        svr.Delete("/shutdown", [&](const Request &, Response &res) {
            res.set_content("ok, shutting down...", "text/plain");
            spdlog::warn("Shutting down...");
            svr.stop();
        });

        svr.Get("/version", [](const Request &, Response &res) {
            auto vers = app::Version().to_string();
            res.set_content(vers, "text/plain");
            spdlog::warn("Version Request: {}", vers);
        });

        return true;
    }

    // Function to run the server
    bool run_service(const Config &config) {
        Server svr;

        // Add CORS headers to every response
        svr.set_pre_routing_handler([](const httplib::Request &req, httplib::Response &res) {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
            return httplib::Server::HandlerResponse::Unhandled;  // Continue processing the request
        });

        // Handle preflight (OPTIONS) requests
        svr.Options(".*", [](const httplib::Request &, httplib::Response &res) {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
            res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
            res.status = 204;  // No Content
        });

        // Set up the server
        if (!app::setup_service(svr, config)) {
            return false;
        }

        spdlog::info("Server starting at http://{}:{}", config.host, config.port);

        // Start the server
        return svr.listen(config.host, config.port);
    }

    /*
    // Function to run the server
    bool run_service(const Config &config) {
        SSLServer svr(config.cert_file.c_str(), config.key_file.c_str());

        // Set up the server
        if (!app::setup_service(svr, config)) {
            return false;
        }

        spdlog::info("Server starting at https://{}:{}", config.host, config.port);

        // Start the server
        return svr.listen(config.host, config.port);
    }
    */
}  // namespace app
