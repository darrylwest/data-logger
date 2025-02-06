//
// 2025-02-01 dpw
//

#include <app/client.hpp>
#include <app/temperature.hpp>
#include <nlohmann/json.hpp>

namespace app {
    namespace client {
        using json = nlohmann::json;

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
            const auto url = "http://" + node.ip + ":" + std::to_string(node.port);
            httplib::Client client(url);

            spdlog::info("fetch from url: {}", url);

            if (auto res = client.Get("/status")) {
                if (res->status == 200) {
                    return parse_status(res->body);
                } else {
                    spdlog::error("request faild to {}/status with status: {}", url, res->status);
                }
            } else {
                auto err = httplib::to_string(res.error());
                spdlog::error("Status request faild to {}, {} {}", url, err, res->status);
            }

            // TODO error handling
            ClientStatus status;
            return status;
        }

        app::TemperatureData fetch_temps(ClientNode& node) {
            const auto url = "http://" + node.ip + ":" + std::to_string(node.port);
            httplib::Client client(url);

            spdlog::info("fetch from url: {}/temps", url);

            if (auto res = client.Get("/temps")) {
                if (res->status == 200) {
                    return app::temperature::parse_reading(res->body);
                } else {
                    spdlog::error("request faild to {}/status with status: {}", url, res->status);
                }
            } else {
                auto err = httplib::to_string(res.error());
                spdlog::error("request faild to {}, {} {}", url, err, res->status);
            }

            // TODO error handling
            TemperatureData data;
            return data;
        }

        // create and return the client nodes; (should read from config.toml)
        Vec<ClientNode> create_nodes() {
            Vec<ClientNode> nodes;
            auto node = ClientNode{
                .location = "cottage",
                .ip = "10.0.1.197",
                .port = 2030,
                .active = true,
                .last_access = 0,
            };

            nodes.push_back(node);

            return nodes;
        }

    }  // namespace client
}  // namespace app
