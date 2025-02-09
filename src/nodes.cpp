//
// nodes.cpp
//
#include <spdlog/spdlog.h>

#include <app/cli.hpp>
#include <app/datetimelib.hpp>
#include <app/nodes.hpp>

namespace app {
    namespace nodes {
        using namespace app::taskrunner;
        using namespace app::client;

        bool is_active(const Str& location) {
            const auto& cfg = app::config::parse_config();

            for (const auto& jclient : cfg["clients"]) {
                if (jclient["location"] == location) {
                    bool active = jclient["active"].template get<bool>();
                    spdlog::info("client node: {} active: {}", location, active);
                    return active;
                }
            }

            spdlog::error("could not find client node for location: {}", location);

            return false;
        }

        Task create_temps_task(ClientNode& node, int period) {
            int error_count = 0;

            auto worker = [&]() mutable {
                try {
                    // read from config on each iteration
                    if (!is_active(node.location)) return;

                    // read config for this node
                    auto data = app::client::fetch_temps(node);
                    int ts = datetimelib::timestamp_seconds();

                    Str at = datetimelib::truncate_to_minutes(data.reading_at);
                    spdlog::info("ts {}, temps: {}, at: {}", ts, data.to_string(), at);

                    for (const auto probe : data.probes) {
                        // db.(key, data);
                        auto key = app::database::create_key(at, probe.location);

                        // TODO create a method for this? pull data folder from config...
                        const auto filename = "data/temperature/current." + probe.location + ".db";
                        spdlog::info("file: {}, k/v: {}={}", filename, key.to_string(),
                                     probe.tempC);

                        app::database::append_key_value(filename, key, std::to_string(probe.tempC));
                    }

                    node.last_access = ts;
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

                    auto key = app::database::create_key(isodate, node.location);
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
