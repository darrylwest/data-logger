//
// 2025-02-01 18:27:44 dpw
//

#include <spdlog/spdlog.h>
#include <vendor/httplib.h>

#include <app/cli.hpp>
#include <app/database.hpp>
#include <app/jsonkeys.hpp>
#include <app/service.hpp>
#include <app/types.hpp>
#include <app/version.hpp>
#include <app/webhandlers.hpp>
#include <nlohmann/json.hpp>

#include "precompiled.hpp"

namespace app::service {
    using namespace httplib;
    using json = nlohmann::json;

    void show_headers(const httplib::Headers &headers) {
        for (const auto &x : headers) {
            spdlog::debug("{}:{}", x.first.c_str(), x.second.c_str());
        }
    }

    void log_request(const httplib::Request &req, const httplib::Response &res) {
        spdlog::info("{} {} {}", req.method.c_str(), req.version.c_str(), req.path.c_str());

        for (const auto &x : req.params) {
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

    // Function to set up the server and apply configurations
    bool setup_service(Server &svr, const config::WebConfig &config, database::Database &db) {
        // open the server's database; read all current data
        if (svr.is_valid() == 0) {
            spdlog::error("ERROR! Server is not valid. Check the cert/key files? Exiting...");
            return false;
        }

        // Error handler
        svr.set_error_handler([](const Request &req, Response &res) {
            spdlog::error("ERROR! bad request {} {}", req.method.c_str(), req.path.c_str());
            // TODO : replace this with spdlog::fmt...
            // TODO : if the request content is json, return plain text; else assume it's a browser
            const char *fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
            char buf[BUFSIZ];
            snprintf(buf, sizeof(buf), fmt, res.status);
            res.set_content(buf, "text/html");
        });

        // Logger
        svr.set_logger([](const Request &req, const Response &res) { log_request(req, res); });

        // Mount point
        if (!svr.set_mount_point("/", config.www)) {
            spdlog::error("ERROR! The specified base directory {} doesn't exist...", config.www);
            return false;
        }

        //
        // add new/custom routes here
        //

        svr.Get("/api/temps", [&](const Request &, Response &res) {
            using namespace app::webhandlers;

            // TODO read request start/end timestamps (or dates) ; default to now

            const Vec<Str> locations = {"front-porch.0"};
            const auto cfg = ChartConfig{
                .end_ts = 1740496800,  // datetimelib::timestamp_seconds(),
                .locations = locations,
                .data_points = 25,
            };

            // TODO pass in the data, not the database...
            const auto chart = create_chart_data(db, cfg);

            const auto json_text = create_temps_response(chart);

            res.set_content(json_text, "application/json");
        });

        // TODO implement calls to active clients
        svr.Get("/api/status", [&](const Request &, Response &res) {
            const auto json_text = app::webhandlers::fetch_active_client_status();
            res.set_content(json_text, "application/json");
        });

        svr.Put("/api/temperature", [&](const Request &req, Response &res) mutable {
            // TODO move to webhandlers
            try {
                auto json_body = json::parse(req.body);
                spdlog::info("json parsed: {}", json_body.dump());
                if (json_body.contains("key") && json_body.contains("value")) {
                    Str key = json_body["key"].get<Str>();
                    float value = json_body["value"].get<float>();
                    spdlog::info("values parsed key: {} value: {}", key, value);

                    // insert into db
                    Str sval = std::to_string(value);
                    spdlog::info("insert: {}={}, dbsize: {}", key, sval, db.size());

                    if (db.set(key, sval)) {
                        spdlog::info("inserted: {}={}, dbsize: {}", key, value, db.size());
                        res.set_content("ok", "text/plain");
                    } else {
                        spdlog::error("database error inserting k/v: {}={}", key, value);
                        res.status = 500;
                        res.set_content("database insert error", "text/plain");
                    }
                } else {
                    spdlog::warn("missing key or value from request: {}", req.body);
                    res.status = 400;
                    res.set_content("missing key or value from request body", "text/plain");
                }

            } catch (const std::exception &e) {
                spdlog::warn("bad put json: {}: {}", req.body, e.what());
                res.status = 400;
                res.set_content("Invalid JSON format", "text/plain");
            }
        });

        // Shutdown hook
        svr.Delete("/api/shutdown", [&](const Request &, Response &res) {
            res.set_content("ok, shutting down...", "text/plain");
            spdlog::warn("Shutting down...");
            // TODO set the shutdown flag
            svr.stop();
        });

        // return the application's version
        svr.Get("/api/version", [](const Request &, Response &res) {
            auto vers = Version().to_string();
            res.set_content(vers, "text/plain");
            spdlog::warn("Version Request: {}", vers);
        });

        return true;
    }

    // Function to run the server
    bool run_service(const config::WebConfig &config) {
        Server svr;

        // Add CORS headers to every response
        svr.set_pre_routing_handler([](const httplib::Request &, httplib::Response &res) {
            res.set_header(ACCESS_CONTROL_ALLOW_ORIGIN, "*");
            res.set_header(ACCESS_CONTROL_ALLOW_METHODS, ACCESS_METHODS);
            res.set_header(ACCESS_CONTROL_ALLOW_HEADERS, "Content-Type, Authorization");
            return httplib::Server::HandlerResponse::Unhandled;  // Continue processing the request
        });

        // Handle preflight (OPTIONS) requests
        svr.Options(".*", [](const httplib::Request &, httplib::Response &res) {
            res.set_header(ACCESS_CONTROL_ALLOW_ORIGIN, "*");
            res.set_header(ACCESS_CONTROL_ALLOW_METHODS, ACCESS_METHODS);
            res.set_header(ACCESS_CONTROL_ALLOW_HEADERS, "Content-Type, Authorization");
            res.status = 204;  // No Content
        });

        // TODO modify for multiple databases: tempsdb, lightdb, etc.

        // read the temps db data
        const auto jdata = cfgsvc::data_node();
        const auto folder = jdata[jsonkeys::FOLDER].get<Str>();
        const auto filename = jdata[jsonkeys::TEMPERATURE].get<Str>();
        auto tempsdb = database::Database();
        tempsdb.read(folder + filename);

        // Set up the server
        if (!setup_service(svr, config, tempsdb)) {
            return false;
        }

        spdlog::info("Server starting at {}://{}:{}", config.scheme, config.host, config.port);

        // Start the server
        return svr.listen(config.host, config.port);
    }
}  // namespace app::service
