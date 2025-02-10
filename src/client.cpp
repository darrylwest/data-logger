//
// 2025-02-01 dpw
//

#include <spdlog/spdlog.h>

#include <app/client.hpp>
#include <app/datetimelib.hpp>
#include <app/exceptions.hpp>
#include <app/jsonkeys.hpp>
#include <app/temperature.hpp>

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

            using namespace app::jsonkeys;

            auto j = jsn[CLIENT_STATUS];
            Str version = j[VERSION];

            ClientStatus status = {
                .version = j[VERSION],
                .timestamp = j[TS],
                .started = j[STARTED],
                .uptime = j[UPTIME],
                .access_count = j[ACCESS],
                .errors = j[ERRORS],
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

            using namespace app::jsonkeys;

            try {
                json cfg = app::config::parse_config();
                json jclients = cfg[CLIENTS];

                for (const auto& jclient : jclients) {
                    nodes.push_back(parse_client_node(jclient));
                }
            } catch (const std::exception& e) {
                Str msg = "error parsing config file: ";
                spdlog::error(msg);
                throw app::ParseException(msg);
            }

            spdlog::info("created {} client nodes", nodes.size());
            for (const auto& client : nodes) {
                spdlog::info("Client: {}, {}:{}, active: {}", client.location, client.ip,
                             client.port, client.active);
            }

            return nodes;
        }

        // parse the client from json and return the client node
        ClientNode parse_client_node(const json& jclient) {
            ClientNode client;

            using namespace app::jsonkeys;

            client.location = jclient[LOCATION];
            client.ip = jclient[IP];
            client.port = jclient[PORT].template get<int>();
            client.active = jclient[ACTIVE].template get<bool>();

            for (const auto& sensor : jclient[SENSORS]) {
                if (sensor[TYPE] != TEMPERATURE) continue;

                for (const auto& probe : sensor[PROBES]) {
                    int sensor = probe[SENSOR].template get<int>();
                    client.probes[sensor] = probe[LOCATION];
                }
            }

            return client;
        }

    }  // namespace client
}  // namespace app
