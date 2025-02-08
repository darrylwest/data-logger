//
// 2025-02-01 dpw
//

#include <spdlog/fmt/fmt.h>

#include <app/client.hpp>
#include <app/datetimelib.hpp>
#include <app/exceptions.hpp>
#include <app/temperature.hpp>
#include <nlohmann/json.hpp>

namespace app {
    namespace client {
        using json = nlohmann::json;

        httplib::Client create_http_client(const Str url) {
            httplib::Client client(url);

            // set the timeouts
            auto timeout = std::chrono::milliseconds{3000};
            client.set_connection_timeout(timeout);
            client.set_read_timeout(timeout);
            client.set_write_timeout(timeout);

            return client;
        }

        ClientStatus parse_status(const Str& json_text) {
            json jsn = json::parse(json_text);

            auto j = jsn["status"];
            Str version = j["version"];

            ClientStatus status = {
                .version = j["version"],
                .timestamp = j["ts"],
                .started = j["started"],
                .uptime = j["uptime"],
                .access_count = j["access"],
                .errors = j["errors"],
            };

            // TODO error handling
            return status;
        }

        ClientStatus fetch_status(ClientNode& node) {
            const auto url = fmt::format("http://{}:{}", node.ip, node.port);
            auto client = create_http_client(url);

            const auto path = "/status";
            spdlog::info("fetch from url: {}{}", url, path);

            Str error_message;

            if (auto res = client.Get(path)) {
                if (res->status == 200) {
                    return parse_status(res->body);
                } else {
                    auto err = httplib::to_string(res.error());
                    error_message = fmt::format("1: request failed to {}/{}, status: {}", url, path,
                                                res->status);
                    spdlog::error(error_message);
                }
            } else {
                auto err = httplib::to_string(res.error());
                error_message = fmt::format("2: status request failed to {}{}, {}", url, path, err);
                spdlog::error(error_message);
            }

            throw app::ServiceException(error_message);
        }

        app::TemperatureData fetch_temps(ClientNode& node) {
            const auto url = fmt::format("http://{}:{}", node.ip, node.port);
            auto client = create_http_client(url);
            const auto path = "/temps";

            spdlog::info("fetch from url: {}{}", url, path);

            Str error_message;
            auto t0 = datetimelib::timestamp_millis();
            if (auto res = client.Get(path)) {
                if (res->status == 200) {
                    auto t1 = datetimelib::timestamp_millis();
                    spdlog::info("data fetched in {} millis", t1 - t0);
                    return app::temperature::parse_reading(res->body);
                } else {
                    error_message = fmt::format("1: request failed to {}/{}, status: {}", url, path,
                                                res->status);
                }
            } else {
                auto err = httplib::to_string(res.error());
                error_message = fmt::format("2: request failed to {}{}, err: {}", url, path, err);
                spdlog::error(error_message);
            }

            auto t1 = datetimelib::timestamp_millis();
            spdlog::info("data fetch failed after {} millis", t1 - t0);

            throw app::ServiceException(error_message);
        }

        // create and return the client nodes; (should read from config.toml)
        Vec<ClientNode> create_nodes() {
            Vec<ClientNode> nodes;
            nodes.emplace_back(ClientNode{
                .location = "deck",
                .ip = "10.0.1.197",
                .port = 2030,
                .active = true,
                .last_access = 0,
            });

            /*
            nodes.emplace_back(ClientNode{
                .location = "cottage",
                .ip = "10.0.1.115",
                .port = 2030,
                .active = true,
                .last_access = 0,
            });
            */

            spdlog::info("created {} client nodes", nodes.size());

            return nodes;
        }

    }  // namespace client
}  // namespace app
