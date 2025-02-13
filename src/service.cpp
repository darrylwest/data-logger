//
// 2025-02-01 18:27:44 dpw
//

#include <spdlog/spdlog.h>
#include <vendor/httplib.h>

#include <app/cli.hpp>
#include <app/database.hpp>
#include <app/jsonkeys.hpp>
#include <app/logging.hpp>
#include <app/service.hpp>
#include <app/types.hpp>
#include <app/version.hpp>
#include <cstdio>
#include <iostream>
#include <nlohmann/json.hpp>

namespace app {
    using namespace httplib;
    using json = nlohmann::json;

    // Function to set up the server and apply configurations
    bool setup_service(Server &svr, const config::Config &config) {

        // open the server's database; read all current data
        auto db = database::Database();
        database::read_current_data(db);

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
            // pull this from params
            const Str end_date = "2025-02-06";

            // TODO : create a function to generate these based on end date and interval.
            // this is set to 30 minutes; there are always 25 labels; move this to temperature?
            const Vec<Str> labels
                = {"09:30", "10:00", "10:30", "11:00", "11:30", "12:00", "12:30", "13:00", "13:30",
                   "14:00", "14:30", "15:00", "15:30", "16:00", "16:30", "17:00", "17:30", "18:00",
                   "18:30", "19:00", "19:30", "20:00", "20:30", "21:00", "21:30"};

            const auto json_text = create_temps_response(labels, end_date);
            // parse the interval and end_date
            // spdlog::info("info content: {}", data);

            res.set_content(json_text, "application/json");
        });

        // TODO add a status endpoint to report on the client nodes, probes, etc...

        // TODO add a Post endpoint /api/temp to insert new temp reading

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
    bool run_service(const config::Config &config) {
        Server svr;

        // Add CORS headers to every response
        svr.set_pre_routing_handler([](const httplib::Request &, httplib::Response &res) {
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

    Str create_temps_response(const Vec<Str> &labels, const Str end_date) {
        json j;

        using namespace app::jsonkeys;

        // Add labels
        j[LABELS] = labels;

        // Sensor 1 data
        const json sensor1
            = {{"sensor_id", "sensor_1"},
               {"label", "cottage-south"},
               {"data", {49.5781,  50.2531,  50.8156,  50.7406,  51.5281,  51.8094,  51.9688,
                         51.9969,  52.3906,  52.5312,  52.5219,  53.6375,  52.7,     52.82187,
                         52.82187, 49.71875, 47.75937, 46.46563, 45.94062, 44.51563, 44.51563,
                         43.95313, 43.95313, 43.86875, 43.86875}},
               {"borderColor", "red"},
               {"fill", false}};

        // Sensor 2 data
        const json sensor2 = {
            {"sensor_id", "sensor_2"},
            {"label", "shed-west"},
            {"data", {49.32900,  49.521116, 50.02136,  51.18444,  52.293116, 51.43887,  52.48388,
                      52.980233, 53.10152,  52.00872,  52.635993, 54.16272,  52.29799,  53.557073,
                      51.98579,  48.73718,  48.225931, 45.70019,  45.66436,  44.396992, 44.27969,
                      43.71025,  43.768674, 44.14863,  43.34765}},
            {"borderColor", "blue"},
            {"fill", false}};

        Vec<json> data;
        data.push_back(sensor1);
        data.push_back(sensor2);

        // Add datasets and end_date
        j[DATASETS] = data;
        j[END_DATE] = end_date;

        return j.dump();
    };
}  // namespace app
