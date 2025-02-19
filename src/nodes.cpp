//
// nodes.cpp
//
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>

#include <app/cli.hpp>
#include <app/database.hpp>
#include <app/jsonkeys.hpp>
#include <app/nodes.hpp>
#include <datetimelib/datetimelib.hpp>

namespace app {
    namespace nodes {
        using namespace app::taskrunner;
        using namespace app::client;

        // return true if this node is active based on current config
        bool is_node_active(const Str& location, const auto& jcfg) {
            using namespace app::jsonkeys;
            for (const auto& jclient : jcfg[CLIENTS]) {
                if (jclient[LOCATION] == location) {
                    bool active = jclient[ACTIVE].template get<bool>();
                    spdlog::info("client node: {} active: {}", location, active);
                    return active;
                }
            }

            spdlog::error("could not find client node for location: {}", location);

            return false;
        }

        // create the temps worker task
        Task create_temps_task(ClientNode& node, int period) {
            unsigned long error_count = 0;

            auto worker = [&]() mutable {
                try {
                    // read from config on each iteration
                    const auto jcfg = app::config::parse_config();
                    if (!is_node_active(node.location, jcfg)) {
                        spdlog::info("skipping node {}; currently inactive...", node.location);
                        return;
                    }

                    // read config for this node
                    auto data = app::client::fetch_temps(node);

                    spdlog::info("temps: {}, at: {}", data.to_string(), data.reading_at);

                    // TODO get port from jcfg
                    const auto cfg = app::config::webservice_from_json(jcfg);
                    const Str host = (cfg.host == "0.0.0.0") ? "localhost" : cfg.host;
                    const auto url = fmt::format("{}://{}:{}", cfg.scheme, host, cfg.port);
                    for (const auto& probe : data.probes) {
                        if (!probe.enabled) continue;

                        auto key = app::database::create_key(data.reading_at, probe.location);

                        // TODO create a method for this? pull data folder from config...
                        const auto filename = "data/temperature/current." + probe.location + ".db";
                        spdlog::info("file: {}, {}={}", filename, key.to_string(), probe.tempC);

                        app::database::append_key_value(filename, key, std::to_string(probe.tempC));

                        if (!app::client::put_temps(url, key, probe)) {
                            spdlog::warn("web service down at: {}", url);
                        }
                    }

                    node.last_access = datetimelib::timestamp_seconds();
                } catch (std::exception& e) {
                    error_count++;
                    spdlog::error("worker: {} data access failed: {}", node.location, e.what());
                    spdlog::warn("temps worker error count: {}", error_count);
                }
            };

            auto task = createTask(node.location.c_str(), period, worker);
            return task;
        }

        // create a list of temps tasks from the node list
        void append_temps_tasks(Vec<ClientNode>& nodes, Vec<Task>& tasks) {
            for (auto& node : nodes) {
                tasks.push_back(create_temps_task(node));
            }
        }

        // create a task to query the client status
        Task create_status_task(ClientNode& node, const int period) {
            int error_count = 0;
            auto worker = [&]() {
                try {
                    // read config to see if the client is still active
                    if (!node.active) {
                        return;
                    }

                    const auto status = app::client::fetch_status(node);

                    const auto isodate = datetimelib::local_iso_datetime(status.timestamp);
                    spdlog::info("ts: {}, uptime: {}, access: {}, errors: {}", status.timestamp,
                                 status.uptime, status.access_count, status.errors);

                    auto key = app::database::create_key(status.timestamp, node.location);
                    const auto filename = "data/status/current." + node.location + ".db";
                    spdlog::info("file: {}, k/v: {}={}", filename, key.to_string(),
                                 status.to_string());

                    app::database::append_key_value(filename, key, status.to_string());
                } catch (const std::exception& e) {
                    error_count++;
                    spdlog::error("worker: {} data access failed: {}", node.location, e.what());
                    spdlog::warn("status worker error count: {}", error_count);
                }
            };

            auto task = createTask(node.location.c_str(), period, worker);
            return task;
        }

        // create a list of status tasks from the node list
        void append_status_tasks(Vec<ClientNode>& nodes, Vec<Task>& tasks) {
            for (auto& node : nodes) {
                tasks.push_back(create_status_task(node));
            }
        }
    }  // namespace nodes
}  // namespace app
