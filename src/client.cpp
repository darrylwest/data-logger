//
// 2025-02-01 dpw
//
// Data collection node client (fetch) and data relay client (put)
//

#include <spdlog/spdlog.h>

#include <app/cfgsvc.hpp>
#include <app/client.hpp>
#include <app/database.hpp>
#include <app/exceptions.hpp>
#include <app/jsonkeys.hpp>
#include <app/temperature.hpp>
#include <app/types.hpp>
#include <datetimelib/datetimelib.hpp>

namespace app::client {
    constexpr time_t TIMEOUT_MILLIS = 6000;
    using json = nlohmann::json;

    Func<httplib::Client(const Str& url)> http_client_creator = [](const Str& url) {
        httplib::Client client(url);

        // set the timeouts
        constexpr auto timeout = std::chrono::milliseconds{TIMEOUT_MILLIS};
        client.set_connection_timeout(timeout);
        client.set_read_timeout(timeout);
        client.set_write_timeout(timeout);

        return client;
    };

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

    // fetch status data from the node client
    ClientStatus fetch_status(ClientNode& node) {
        const auto url = fmt::format("http://{}:{}", node.ip, node.port);
        auto client = http_client_creator(url);

        const auto path = "/status";
        spdlog::info("fetch from url: {}{}", url, path);

        Str error_message;

        if (auto res = client.Get(path)) {
            if (res->status == 200) {
                return parse_status(res->body);
            } else {
                auto err = httplib::to_string(res.error());
                error_message
                    = fmt::format("1: request failed to {}/{}, status: {}", url, path, res->status);
                spdlog::error(error_message);
            }
        } else {
            auto err = httplib::to_string(res.error());
            error_message = fmt::format("2: status request failed to {}{}, {}", url, path, err);
            spdlog::error(error_message);
        }

        throw app::ServiceException(error_message);
    }

    // fetch temperature data from the node client
    temperature::TemperatureData fetch_temps(ClientNode& node) {
        const auto url = fmt::format("http://{}:{}", node.ip, node.port);
        auto client = http_client_creator(url);
        const auto path = "/temps";

        spdlog::info("fetch from url: {}{}", url, path);

        Str error_message;
        auto t0 = datetimelib::timestamp_millis();
        if (auto res = client.Get(path)) {
            if (res->status == 200) {
                auto t1 = datetimelib::timestamp_millis();
                spdlog::info("data fetched in {} millis", t1 - t0);
                // spdlog::info("body: {}", res->body);
                return temperature::parse_reading(res->body);
            } else {
                error_message
                    = fmt::format("1: request failed to {}/{}, status: {}", url, path, res->status);
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

    // send/put client node reading to web server (if server is available) else return false
    bool put_temps(const Str& url, const database::DbKey& key, const temperature::Probe& probe) {
        spdlog::info("put temps: to {}, {} {}C {}F", url, key.to_string(), probe.tempC,
                     probe.tempF);
        auto client = http_client_creator(url);

        const auto path = "/api/temperature";

        json jdata = {{"key", key.to_string()}, {"value", probe.tempC}};
        const auto body = jdata.dump();

        auto res = client.Put(path, body, "application/json");
        if (res) {
            if (res->status == 200) {
                spdlog::info("put data to {}{}", url, path);
                return true;
            } else {
                spdlog::error("put data to {}{} failed, status: {}", url, path, res->status);
            }
        } else {
            auto err = httplib::to_string(res.error());
            spdlog::warn("put data to {}{} failed, status: {}", url, path, err);
        }

        return false;
    }

    // create and return the client nodes; (should read from config.toml)
    Vec<ClientNode> create_nodes() {
        Vec<ClientNode> nodes;

        using namespace app::jsonkeys;

        try {
            json jclients = cfgsvc::clients();

            for (const auto& jclient : jclients) {
                nodes.push_back(parse_client_node(jclient));
            }
        } catch (const std::exception& e) {
            Str msg = fmt::format("error parsing config file: {}", e.what());
            spdlog::error(msg);
            throw app::ParseException(msg);
        }

        spdlog::info("created {} client nodes", nodes.size());
        for (const auto& client : nodes) {
            spdlog::info("Client: {}, {}:{}, active: {}", client.location, client.ip, client.port,
                         client.active);
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
        if (jclient.contains("last_access")) {
            client.last_access = jclient["last_access"].template get<int>();
        } else {
            client.last_access = 0;
        }

        for (const auto& sensor : jclient[SENSORS]) {
            if (sensor[TYPE] != TEMPERATURE) continue;

            for (const auto& probe : sensor[PROBES]) {
                int snr = probe[SENSOR].template get<int>();
                client.probes[snr] = probe[LOCATION];
            }
        }

        return client;
    }
}  // namespace app::client
